# 389ds-plugin-ldapsonly

Allow binds only over TLS/SSL connection on 389DS

## Compilation

```sh
$ apt-get install 389-ds-base-dev libnspr4-dev
$ make clean
$ make
```

## Instalation

```sh
$ cp ldapsonly.so /usr/lib/x86_64-linux-gnu/dirsrv/plugins
$ cat ldapsonly.ldif | ldapadd -H ldap://localhost -D "cn=Directory Manager" -W
$ systemctl restart dirsrv@ldap
```

## Configuration

IP addreses with exception can be configured by nsslapd-pluginarg**n**
attribute. IPv4 addreses needs to be configured with `::ffff:` prefix.

Examplesof log entries produced by plugin:

```
[21/Sep/2020:15:52:23.531119282 +0200] - ERR - ldapsonly_bind - User 'uid=semik' from IP '::ffff:192.168.1.1' is not using SSL(0). Bind refused.


[21/Sep/2020:15:54:28.375478877 +0200] - DEBUG - ldapsonly_bind - User 'uid=semik' from IP '::ffff:192.168.1.1' is not using SSL(0). Bind is permitted because he comes from configured IP '::ffff:192.168.1.1'.
```