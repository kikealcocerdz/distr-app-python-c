#ifndef CLAVES_H
#define CLAVES_H

#include <stdio.h>

void init_serv(char *res);
void set_value_serv(int key, char *value1, int N_value2, char *V_value2, char *res);
void get_value_serv(int key, char *value1, int *N_value2, char *V_value2, char *res);
void modify_value_serv(int key, char *value1, int N_value2, char *V_value2, char *res);
void delete_value_serv(int key, char *res);
void exists_serv(int key, char *res);

#endif // CLAVES_H
