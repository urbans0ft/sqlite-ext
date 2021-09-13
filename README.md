# Table of Content

* [Usage](#usage)
    * [Loading Extensions](#loading-extensions)
        * [Cygwin](#cygwin)
        * [Windows](#windows)
        * [Linux](#linux)
    * [Example](#example)
* [Build](#build)
    * [Cygwin](#cygwin-64-bit)
    * [Windows](#windows-64-bit)
    * [Linux](#linux-64-bit)
* [References](#references)

# Usage

## Loading Extensions

### Cygwin

```
$ sqlite3
sqlite> .load 'bin/CYG64/uuid.so'
sqlite> .load 'bin/CYG64/regexp.so'
```

### Windows

```
$ sqlite3
sqlite> .load 'bin/WIN64/uuid.dll'
sqlite> .load 'bin/WIN64/regexp.dll'
```

### Linux

```
$ sqlite3
sqlite> .load 'bin/UNIX64/uuid.so'
sqlite> .load 'bin/UNIX64/regexp.so'
```

## Example

The extensions need to be [loaded](#loading-extensions) beforehand.

### uuid & regexp

```
sqlite> SELECT uuid();
afe6ae32-320b-44c2-bff8-f9cb4c7aa6b6
sqlite> SELECT uuid();
48c5fe0f-5488-41f7-aa40-706cdb98d080
sqlite> SELECT regexp('^[0-9a-f]{8}(-[0-9a-f]{4}){4}[0-9a-f]{8}$', uuid());
1
sqlite> SELECT regexp('^[0-9a-f]{8}(-[0-9a-f]{4}){4}[0-9a-f]{8}$', 'I am not a UUID');
0
```

### udp

#### Server

```
$ nc -kluv4w0 localhost 27015
```

#### Client

```
SELECT udp('Hello World!');
```

# Build

## Cygwin 64-bit

```
$ make
gcc -o bin/CYG64/regexp.so -g -fPIC -shared -Isqlite regexp/regexp.c
Built bin/CYG64/regexp.so
gcc -o bin/CYG64/uuid.so -g -fPIC -shared -Isqlite uuid/uuid.c
Built bin/CYG64/uuid.so
Built for os 'CYG64' with sqlite header file version 3.36.0.
```

## Linux 64-bit

```
$ make TARGET_OS=UNIX64
gcc -o bin/UNIX64/regexp.so -g -fPIC -shared -Isqlite regexp/regexp.c
Built bin/UNIX64/regexp.so
gcc -o bin/UNIX64/uuid.so -g -fPIC -shared -Isqlite uuid/uuid.c
Built bin/UNIX64/uuid.so
Built for os 'UNIX64' with sqlite header file version 3.36.0.
```

## Windows 64-bit

```
$ make TARGET_OS=WIN64
x86_64-w64-mingw32-gcc -o bin/WIN64/regexp.dll -g -shared -Isqlite regexp/regexp.c
Built bin/WIN64/regexp.dll
x86_64-w64-mingw32-gcc -o bin/WIN64/uuid.dll -g -shared -Isqlite uuid/uuid.c
Built bin/WIN64/uuid.dll
Built for os 'WIN64' with sqlite header file version 3.36.0.
```

# References

* [sqlite GitHub](https://github.com/sqlite/sqlite)
* [sqlite Miscellaneous Extensions](https://github.com/sqlite/sqlite/tree/master/ext/misc)
* [sqlite Run-Time Loadable Extension](https://www.sqlite.org/loadext.html)
