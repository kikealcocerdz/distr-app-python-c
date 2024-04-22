#include <stdio.h>
#include "clavesservidor.h"
#include <mqueue.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>

int claves;
int key_leida;
char valor1[100];
int N_value2;
double V_value2[100];

void init_serv(char *res) {

    // Eliminar el directorio /claves si existe
    if (access("./claves", F_OK) == 0) {
        DIR *dir = opendir("./claves");
        if (dir) {
            // Si el directorio está lleno, elimina su contenido
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    char path[1024];
                    snprintf(path, sizeof(path), "%s/%s", "./claves", entry->d_name);
                    remove(path);
                }
            }
            closedir(dir);
        }
        // Eliminar el directorio /claves
        if (remove("./claves") == -1) {
            perror("Error al eliminar ./claves\n");
            sprintf(res, "-1");
            return; 
        }
    }

    // Crear el directorio /claves
    if (mkdir("./claves", 0777) == -1) {
        perror("Error al eliminar ./claves\n");
        sprintf(res, "-1");
        return;  
    }

    sprintf(res, "0");
    return; 
}

void set_value_serv(int key, char *value1, int N_value2, char *V_value2, char *res) {
    char filename[20]; 
    sprintf(filename, "./claves/%d.txt", key);  // Convertimos la clave en string y añadimos el directorio
    FILE *checkExistFile = fopen(filename, "r");
    if (checkExistFile != NULL) {
        fclose(checkExistFile);
        perror("Clave utilizada previamente, escoja otra \n");
        sprintf(res, "-1");
        return;
    }
    
    FILE *clavesFile = fopen(filename, "w+");
    if (clavesFile == NULL) {
        perror("Error al abrir el archivo");
        sprintf(res, "-1");
        return;
    }

    // Insertar la nueva clave-valor
    fprintf(clavesFile, "%d %s %d %s", key, value1, N_value2, V_value2); 
    fprintf(clavesFile, "\n"); 
    fclose(clavesFile);
    printf("%d %s %d %s", key, value1, N_value2, V_value2); 
    sprintf(res, "0");
    return;
}

void get_value_serv(int key, char *value1, int *N_value2, char *V_value2, char *res) {
    char filename[20]; 
    sprintf(filename, "./claves/%d.txt", key);
    FILE *clavesFile = fopen(filename, "r");
    
    if (clavesFile == NULL) {
        perror("Error al abrir claves file\n");
        sprintf(res, "-1");
        return;
    }

    // Leer el contenido del archivo y almacenarlo en las variables
    if (fscanf(clavesFile, "%d %s %d %s", &key, value1, N_value2, V_value2) != 4) {
        fclose(clavesFile);
        perror("Error get_value valores esperados\n");
        sprintf(res, "-1");
        return;
    }

    fclose(clavesFile);
    sprintf(res, "0/%s/%d/%s", value1, *N_value2, V_value2);
    return;
}

void delete_value_serv(int key, char *res) {
    char filename[20]; 
    sprintf(filename, "./claves/%d.txt", key); 

    // Verificar si el archivo existe
    if (access(filename, F_OK) != 0) {
        sprintf(res, "-1");
        return;
    }

    if (unlink(filename) == -1) {
        sprintf(res, "-1");
        return;
    }

    sprintf(res, "0");
    return;
}

void modify_value_serv(int key, char *value1, int N_value2, char *V_value2, char *res) {
    
    delete_value_serv(key, res);
    set_value_serv(key, value1, N_value2, V_value2, res);
    return;
}


void exists_serv(int key, char*res) {
    char filename[20]; 
    sprintf(filename, "./claves/%d.txt", key); 

    FILE *clavesFile = fopen(filename, "r");
    if (clavesFile == NULL) {
        sprintf(res, "0");
        return;
    }
    while (fscanf(clavesFile, "%d %s %d", &key_leida, valor1, &N_value2) == 3) {
        if (key_leida == key) {
            fclose(clavesFile);
            sprintf(res, "1");
            return;
        }
    }
    
    fclose(clavesFile);
    sprintf(res, "-1");
    return;
}