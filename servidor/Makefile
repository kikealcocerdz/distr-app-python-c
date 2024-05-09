.PHONY: all clean run-servidor

CFLAGS := -g -I/usr/include/tirpc
LDFLAGS := -L. -lnsl -lpthread -ldl -ltirpc

all: servidor

servidor: servidor.o servidorweb_server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

servidor.o: servidor.c
	$(CC) $(CFLAGS) -c $<

servidorweb_server.o: servidorweb_server.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f servidor servidor.o servidorweb_server.o
