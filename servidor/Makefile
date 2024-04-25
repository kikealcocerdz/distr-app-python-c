.PHONY: all clean run cliente servidor

all: servidor

servidor: clavesservidor.c servidor.c comm.c
	gcc clavesservidor.c comm.c servidor.c -L. -o servidor

clean:
	rm -f servidor

run-servidor: servidor
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(SOURCE_DIR) ./servidor 3000

