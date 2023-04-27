# Table of Content

* [Usage](#usage)
   * [Loading Extensions](#loading-extensions)
      * [Cygwin](#cygwin)
      * [Windows](#windows)
      * [Linux](#linux)
   * [Example](#example)
      * [uuid &amp; regexp](#uuid--regexp)
      * [udp](#udp)
         * [Server](#server)
         * [Client](#client)
* [Development](#development)
   * [Windows 64-bit Environment](#windows-64-bit-environment)
   * [Build](#build)
      * [Release](#release)
      * [Debug](#debug)
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

# Development

## Windows 64-bit Environment

Install the latest [Cygwin](https://www.cygwin.com/setup-x86_64.exe) with the following packages:

- `make`
- `gcc-core`  
  GNU Compiler Collection (C-language for Cygwin builds)
- `mingw64-i686-gcc-core`  
  GNU Compiler Collection (C-language for Win32 builds)
- `mingw64-x86_64-gcc-core`  
  GNU Compiler Collection (C-language for Win64 builds)

You might use the following script to setup the development environment:

```
if not exist C:\Cygwin mkdir C:\Cygwin
curl -L "https://www.cygwin.com/setup-x86_64.exe" --output "C:\Cygwin\setup-x86_64.exe"
C:\Cygwin\setup-x86_64.exe -f -g -v -l C:\Cygwin -R C:\Cygwin -n -O -s http://cygwin.mirror.constant.com/ -q -P gcc-core,make,mingw64-i686-gcc-core,mingw64-x86_64-gcc-core
```

You can start a new _Cygwin_ terminal by pressing `<win>+r` and running the following command:

```
C:\Cygwin\bin\mintty.exe -i /Cygwin-Terminal.ico -
```

## Build

Start a _Cygwin_ and navigate to the `sqlite-ext` project folder. Check the available
build targets with `make`:

```
$ make ls-os
cygwin unix win32 win64
```

Choose one of the build targets to build either a release or debug version (see below). 

### Release

```
$ make win64
x86_64-w64-mingw32-gcc -o bin/win64/regexp.dll -Isqlite -Wall -ffunction-sections -fdata-sections regexp/regexp.c -s -shared -static -Wl,--subsystem,windows,--gc-sections
Built bin/win64/regexp.dll
x86_64-w64-mingw32-gcc -o bin/win64/uuid.dll -Isqlite -Wall -ffunction-sections -fdata-sections uuid/uuid.c -s -shared -static -Wl,--subsystem,windows,--gc-sections
Built bin/win64/uuid.dll
x86_64-w64-mingw32-gcc -o bin/win64/udp.dll -Isqlite -Wall -ffunction-sections -fdata-sections udp/udp.c -s -shared -static -Wl,--subsystem,windows,--gc-sections -lWs2_32
Built bin/win64/udp.dll
Built for os 'win64' with sqlite header file version 3.36.0.
```

### Debug

```
$ make cygwin DEBUG=1
gcc -o bin/cygwin/regexp.so -Isqlite -Wall -DDEBUG -fPIC regexp/regexp.c -s -shared
Built bin/cygwin/regexp.so
gcc -o bin/cygwin/uuid.so -Isqlite -Wall -DDEBUG -fPIC uuid/uuid.c -s -shared
Built bin/cygwin/uuid.so
gcc -o bin/cygwin/udp.so -Isqlite -Wall -DDEBUG -fPIC udp/udp.c -s -shared -lWs2_32
Built bin/cygwin/udp.so
Built for os 'cygwin' with sqlite header file version 3.36.0.
```


# References

* [Cygwin](https://www.cygwin.com)
* [sqlite GitHub](https://github.com/sqlite/sqlite)
* [sqlite Miscellaneous Extensions](https://github.com/sqlite/sqlite/tree/master/ext/misc)
* [sqlite Run-Time Loadable Extension](https://www.sqlite.org/loadext.html)
