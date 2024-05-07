#ifndef CLAVES_H
#define CLAVES_H

#include <stdio.h>

void init_serv(char *res);
void register_serv(char *username, char *res);
void unregister_serv(char *username, char *res);
void connect_serv(char *username, char *free_port, char *free_server, char *res);
void disconnect_serv(char *username, char *res);
void publish_serv(char *username, char *filename, char *description, char *res);
void delete_serv(char *username, char *filename, char *res);
void list_users_serv(char *username, char *res, int *res2);
void list_content_serv(char *username, char *username_folder, char *res, int *res2, char *res_username);

#endif // CLAVES_H
