#ifndef CLAVES_H
#define CLAVES_H

#include <stdio.h>

void init_serv(char *res);
void register_serv(char *username, char *res);
void unregister_serv(char *username, char *res);
void connect_serv(char *username, char *free_port, char *res);
void publish_serv(char *username, char *filename, char *description, char *res);
void delete_serv(char *username, char *filename, char *res);

#endif // CLAVES_H
