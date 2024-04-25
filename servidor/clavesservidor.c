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


void register_serv(char *username, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);  // Convertimos el nombre de usuario en string y añadimos el directorio
    // Check if the folder exists
    if (access(foldername, F_OK) == 0) {
        perror("Usuario registrado previamente\n");
        sprintf(res, "1");
        return;
    }
    
    // Create the folder
    if (mkdir(foldername, 0777) == -1) {
        perror("Error al crear el directorio");
        sprintf(res, "2");
        return;
    }
    sprintf(res, "0");
    return;
}
void unregister_serv(char *username, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Check if the folder exists
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Attempt to remove the folder
    if (rmdir(foldername) != 0) {
        // Error occurred while removing the folder
        perror("Error al eliminar el directorio\n");
        sprintf(res, "2");
        return;
    }

    // Folder successfully removed
    sprintf(res, "0");
}

void connect_serv(char *username, char *free_port, char *res) {
    printf("Cliente %s ha pasado el puerto %s\n", username, free_port);
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Check if the folder exists
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "3");
        return;
    }

    char user[256];
    while (fscanf(conectadosFile, "%s", user) == 1) {
        if (strcmp(user, username) == 0) {
            fclose(conectadosFile);
            sprintf(res, "2");
            return;
        }
    }
    fclose(conectadosFile);
    sprintf(res, "0");
    return;
}

void disconnect_serv(char *username, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Check if the folder exists
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "3");
        return;
    }

    char user[256];
    while (fscanf(conectadosFile, "%s", user) == 1) {
        if (strcmp(user, username) == 0) {
            fclose(conectadosFile);
            sprintf(res, "0");
            return;
        }
    }
    fclose(conectadosFile);
    sprintf(res, "2");
    return;
}

void publish_serv(char *username, char *fileName, char *description, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Check if the folder exists
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "3");
        return;
    }

    char user[256];
    while (fscanf(conectadosFile, "%s", user) == 1) {
        if (strcmp(user, username) == 0) {
            fclose(conectadosFile);
            // Verificar si el archivo ya está publicado
            char filepath[100];
            sprintf(filepath, "%s/%s", foldername, fileName);
            if (access(filepath, F_OK) == 0) {
                // Archivo ya publicado
                perror("El archivo ya está publicado\n");
                sprintf(res, "3");
                return;
            }

            // Crear el archivo en la carpeta del usuario
            FILE *file = fopen(filepath, "w");
            if (file == NULL) {
                perror("Error al crear el archivo\n");
                sprintf(res, "4");
                return;
            }

            // Escribir la descripción en el archivo
            fprintf(file, "%s", description);

            // Cerrar el archivo
            fclose(file);

            // Actualizar el archivo de registros si es necesario

            sprintf(res, "0");
            return;
        }
    }
    fclose(conectadosFile);
    sprintf(res, "2");
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
    //set_value_serv(key, value1, N_value2, V_value2, res);
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