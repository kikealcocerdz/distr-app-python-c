# Parámetros
SERVER = ./servicio-web/servidorweb_server
CC = gcc

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = ./servicio-web/servidorweb.x

TARGETS_SVC.c = servicio-web/servidorweb_svc.c servicio-web/servidorweb_server.c servicio-web/servidorweb_xdr.c 
TARGETS_CLNT.c = servicio-web/servidorweb_clnt.c servicio-web/servidorweb_xdr.c 
TARGETS = servicio-web/servidorweb.h servicio-web/servidorweb_xdr.c servicio-web/servidorweb_clnt.c servicio-web/servidorweb_svc.c  servicio-web/servidorweb_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)

# Flags del compilador
CPPFLAGS += -D_REENTRANT
CFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -lpthread -ldl -ltirpc
RPCGENFLAGS = 

# Objetivos
all : $(SERVER) servidor

$(TARGETS) : $(SOURCES.x) 
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

#$(CLIENT) : $(OBJECTS_CLNT) 
#	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(CC) $(CFLAGS) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

servidor: servidor/clavesservidor.c servidor/servidor.c servidor/comm.c $(OBJECTS_CLNT) 
	$(CC) $(CFLAGS) -o servidor/servidor servidor/clavesservidor.c servidor/servidor.c servidor/comm.c servicio-web/servidorweb_clnt.c servicio-web/servidorweb_xdr.c  $(LDLIBS)

clean:
	$(RM) core $(OBJECTS_CLNT) $(OBJECTS_SVC)  $(SERVER) servidor/clavesservidor

.PHONY: all clean
