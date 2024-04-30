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

    printf("Usuario %s registrado\n", username);
    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "a+");

    char user[256];
    while (fscanf(conectadosFile, "%s", user) == 1) {
        if (strcmp(username, user) == 0) {
            fclose(conectadosFile);
            sprintf(res, "2");
            return;
        }
    }

    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "3");
        return;
    }

    printf("Archivo conectados.txt abierto\n");
    fprintf(conectadosFile, "%s %s \n", username, free_port);
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

    // Write the file inside the user folder
    char filepath[256];
    sprintf(filepath, "%s/%s.txt", foldername, fileName);
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo\n");
        sprintf(res, "4");
        return;
    }
    fprintf(file, "%s\n", description);
    fclose(file);

    sprintf(res, "0");
    return;
}

void delete_serv(char *username, char *fileName, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);
    printf("Foldername: %s\n", foldername);
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

    // Write the file inside the user folder
    char filepath[256];
    sprintf(filepath, "%s/%s.txt", foldername, fileName);
    if (remove(filepath) != 0) {
        perror("Error al eliminar el archivo\n");
        sprintf(res, "4");
        return;
    }

    sprintf(res, "0");
    return;
}

void list_users_serv(char *username, char *res) {
    char foldername[20]; 
    sprintf(foldername, "../usuarios/%s", username);
    printf("Foldername: %s\n", foldername);
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

    // Write the file inside the user folder
    char filepath[256];
    sprintf(filepath, "%s/%s.txt", foldername, fileName);
    if (remove(filepath) != 0) {
        perror("Error al eliminar el archivo\n");
        sprintf(res, "4");
        return;
    }

    sprintf(res, "0");
    return;
}