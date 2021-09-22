UPD - Extension
===============

Usage
-----

### Inception

Load the udp extension in the following example the extension which was build for the
Cygwin (64-bit) environment is loaded.

```
sqlite> .load bin/CYG64/udp.so
sqlite> select * from _udp;
localhost|27015
```

If not present `.load udp.so` ensures the creation of a `_udp` table which contains
the server and port which are used.

### Sending

Sending is done via the `udp(TEXT)` function.

```
sqlite> SELECT udp("Hello World!" || char(13) || char(10));
0
```

You can test this by starting a `netcat` UDP/IPv4 server. Start the server
first (see next code bock) and use the `udp` function afterwards:

```
$ nc -kluv4w0 localhost 27015
Hello World!
```

### Configuration

To change port and/or server simply update the `_udp` table.

```
UPDATE _udp SET Server = "127.0.0.1", Port = 12345;
```

Trigger prohibit deletion as well as insertion of further rows to the `_udp` table.  
**The sqlite3 process needs to be exited and the extension must be loaded again for the
configuration to becaume active!**

### Misch

Further functions include:

- `upd_port()`
	```
	sqlite> SELECT udp_port();
	27015
	```

- `udp_version()`

	```
	sqlite> SELECT udp_version();
	0.1.0 Sep 22 2021 13:46:56
	```