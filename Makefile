CC = gcc
LD = gcc
CFLAGS = -Wall -fPIC -I/usr/include/dirsrv -I/usr/include/nspr -I/usr/include/mozldap
LDFLAGS = -shared
OBJS = ldapsonly.o
all: ldapsonly.so
ldapsonly.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	-rm -f $(OBJS) ldapsonly.so
