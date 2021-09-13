# Table of Content

* [Usage](#usage)
    * [Cygwin](#cygwin)
    * [Windows](#windows)
    * [Linux](#linux)
* [Build](#build)
    * [Cygwin](#cygwin-64-bit)
    * [Windows](#windows-64-bit)
    * [Linux](#linux-64-bit)
* [References](#references)

# Usage

## Cygwin

```
$ sqlite3
sqlite> .load 'bin/CYG64/uuid.so'
sqlite> .load 'bin/CYG64/regexp.so'
```

## Windows

```
$ sqlite3
sqlite> .load 'bin/WIN64/uuid.dll'
sqlite> .load 'bin/WIN64/regexp.dll'
```

## Linux

```
$ sqlite3
sqlite> .load 'bin/UNIX64/uuid.so'
sqlite> .load 'bin/UNIX64/regexp.so'
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
