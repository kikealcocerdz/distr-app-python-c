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

char valor1[100];
int N_value2;
double V_value2[100];


void register_serv(char *username, char *res) {
    // Comprobamos si el usuario es nulo
    if (strlen(username) == 0 || strlen(username) > 256) {
        sprintf(res, "2");
        return;
    }
    else {
    char foldername[1024]; 
    sprintf(foldername, "../usuarios/%s", username);  // Convertimos el nombre de usuario en string y añadimos el directorio

    // Comprobamos si el usuario ya existe
    if (access(foldername, F_OK) == 0) {
        perror("Usuario registrado previamente\n");
        sprintf(res, "1");
        return;
    }
    
    // CCreamos el directorio
    if (mkdir(foldername, 0777) == -1) {
        perror("Error al crear el directorio");
        sprintf(res, "2");
        return;
    }
    sprintf(res, "0");
    return;
    }
}

void unregister_serv(char *username, char *res) {
    char foldername[256]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Comprobamos si está registrado
    if (access(foldername, F_OK) != 0) {
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Borramos el contenido del directorio
    DIR *dir = opendir(foldername);
    if (dir == NULL) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            char filepath[1024];
            sprintf(filepath, "%s/%s", foldername, entry->d_name);
            if (remove(filepath) != 0) {
                perror("Error al eliminar el archivo\n");
                sprintf(res, "2");
                closedir(dir);
                return;
            }
        }
    }
    closedir(dir);

    // Borramos el directorio
    if (rmdir(foldername) != 0) {
        // Error occurred while removing the folder
        perror("Error al eliminar el directorio\n");
        sprintf(res, "2");
        return;
    }

    sprintf(res, "0");
}

void connect_serv(char *username, char *free_port, char *free_server, char *res) {
    char foldername[256]; 
    sprintf(foldername, "../usuarios/%s", username);

    // Comprobamos si el usuario está registrado
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    };

    // Comprobamos si el usuario ya está conectado
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
    fprintf(conectadosFile, "%s %s %s \n", username, free_server, free_port);
    fclose(conectadosFile);

    sprintf(res, "0");
    return;
}

void disconnect_serv(char *username, char *res) {
    char foldername[256]; 
    
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r+");
    FILE *tempFile = fopen("../usuarios/temp.txt", "w");
    sprintf(foldername, "../usuarios/%s", username);

    if (access(foldername, F_OK) != 0) {
        sprintf(res, "1");
        fclose(conectadosFile);
        return;
    }

    if (conectadosFile == NULL) {
        sprintf(res, "3");
        return;
    }

    char line[256];
    int userFound = 0;

    // Leer el archivo conectados.txt y escribir en temp.txt
    while (fgets(line, sizeof(line), conectadosFile)) {
        if (strstr(line, username) != NULL) {
            // User found
            userFound = 1;
            continue;
        }
        // Escrbir la línea en el archivo temporal
        fputs(line, tempFile);
    }

    fclose(conectadosFile);
    fclose(tempFile);

    // Reemplazar el archivo conectados.txt con temp.txt
    remove("../usuarios/conectados.txt");
    rename("../usuarios/temp.txt", "../usuarios/conectados.txt");

    if (userFound) {
        sprintf(res, "0");
    } else {
        sprintf(res, "2");
    }
}

void publish_serv(char *username, char *fileName, char *description, char *res) {
    char foldername[50]; 
    char checkfile[100];
    sprintf(foldername, "../usuarios/%s", username);
    
    // Creamos un archivo con el nombre del archivo
    sprintf(checkfile, "%s/%s.txt", foldername, fileName); 

    if (access(foldername, F_OK) != 0) {
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    if (access(checkfile, F_OK) == 0) {
        sprintf(res, "3");
        return;
    } 

    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "2");
        return;
    }

    while (fscanf(conectadosFile, "%s", valor1) == 1) {
        if (strcmp(username, valor1) == 0) {
            fclose(conectadosFile);
            break;
        }
    }

    char filepath[256];

    // Si el archivo no existe, salir   
    if (strcmp(fileName, "notfound") == 0) {
        sprintf(res, "4");
        return;
    }
    else {
        sprintf(filepath, "%s/%s.txt", foldername, fileName);
        FILE *file = fopen(filepath, "w");
        if (file == NULL) {
            perror("Error al abrir el archivo\n");
            sprintf(res, "4");
            return;
        }
        fprintf(file, "%s\n", description);
        fclose(file);
    }

    sprintf(res, "0");
    return;
}


void delete_serv(char *username, char *fileName, char *res) {

    // Comprombamos si el usuario es nulo
    char foldername[60]; 
    if (strlen(fileName) == 0 || strlen(fileName) > 256){
        sprintf(res, "4");
        return;
    }

    sprintf(foldername, "../usuarios/%s", username);
    printf("Foldername: %s\n", foldername);
    
    if (access(foldername, F_OK) != 0) {
        
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }


    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "2");
        return;
    }


    char filepath[256];

    

    sprintf(filepath, "%s/%s.txt", foldername, fileName);
    if (remove(filepath) != 0) {
        perror("Error al eliminar el archivo\n");
        sprintf(res, "3");
        return;
    }

    sprintf(res, "0");
    return;
}

void list_users_serv(char *username, char *res, int *res2) {
    char foldername[20]; 

    // Comprobamos si el usuario es nulo
    if (strcmp(username, "") == 0) {
        sprintf(res, "2");
        return;
    }

    sprintf(foldername, "../usuarios/%s", username);
    
    if (access(foldername, F_OK) != 0) {
        
        sprintf(res, "1");
        return;
    }
    
    
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "2");
        return;
    }

    char line[256];
    int lineCount = 0;

    // Contar las líneas del archivo conectados.txt
    while (fgets(line, sizeof(line), conectadosFile)) {
        lineCount++;
    }

    fclose(conectadosFile);
    sprintf(res, "0");
    *res2 = lineCount; 
    return;
} 

void list_content_serv(char *username, char *username_folder, char *res, int *res2, char *res_username) {
    char foldername[50];
    sprintf(foldername, "../usuarios/%s", username_folder);


    DIR *dir = opendir(foldername);
    if (dir == NULL) {
        
        sprintf(res, "Usuario no registrado\n");
        *res2 = -1; 
        return;
    }

    int fileCount = 0;
    struct dirent *entry;

    // Contar los archivos en el directorio
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            fileCount++;
        }
    }

    closedir(dir);
    
    sprintf(res, "0");
    *res2 = fileCount;
    sprintf(res_username, "%s", username_folder);
    return;
}
