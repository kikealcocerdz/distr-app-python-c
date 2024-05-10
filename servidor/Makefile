.PHONY: all clean run cliente servidor


CFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -lpthread -ldl -ltirpc

all: servidor

servidor: clavesservidor.c servidor.c comm.c
	$(CC) $(CFLAGS) -o servidor clavesservidor.c servidor.c comm.c $(LDLIBS)

clean:
	rm -f servidor