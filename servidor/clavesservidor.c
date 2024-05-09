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

void connect_serv(char *username, char *free_port, char *free_server, char *res) {
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
    fprintf(conectadosFile, "%s %s %s \n", username, free_server, free_port);
    fclose(conectadosFile);

    sprintf(res, "0");
    return;
}

void disconnect_serv(char *username, char *res) {
    char foldername[20]; 
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r+");
    FILE *tempFile = fopen("../usuarios/temp.txt", "w");
    sprintf(foldername, "../usuarios/%s", username);

    // Check if the folder exists
    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        sprintf(res, "1");
        fclose(conectadosFile);
        return;
    }

    // Check if conectados.txt exists
    if (conectadosFile == NULL) {
        // Error opening conectados file
        sprintf(res, "3");
        return;
    }

    char line[256];
    int userFound = 0;

    // Read file line by line
    while (fgets(line, sizeof(line), conectadosFile)) {
        if (strstr(line, username) != NULL) {
            // Skip the line if it contains the username
            userFound = 1;
            continue;
        }
        // Write the line to the temporary file
        fputs(line, tempFile);
    }

    fclose(conectadosFile);
    fclose(tempFile);

    // Replace the original file with the temporary one
    remove("../usuarios/conectados.txt");
    rename("../usuarios/temp.txt", "../usuarios/conectados.txt");

    if (userFound) {
        // Notify success
        sprintf(res, "0");
    } else {
        // Notify failure
        sprintf(res, "2");
    }
}

void publish_serv(char *username, char *fileName, char *description, char *res) {
    char foldername[50]; 
    char checkfile[100];
    sprintf(foldername, "../usuarios/%s", username);
    sprintf(checkfile, "%s/%s.txt", foldername, fileName); // Full path to the file

    if (access(foldername, F_OK) != 0) {
        // Folder does not exist
        perror("Usuario no registrado\n");
        sprintf(res, "1");
        return;
    }

    // Check if the file exists
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

    char foldername[60]; 
    if (strlen(fileName) == 0 || strlen(fileName) > 256){
        sprintf(res, "4");
        return;
    }

    if (strlen(fileName) == 0 || strlen(fileName) > 256){
        sprintf(res, "4");
        return;
    }


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
        sprintf(res, "2");
        return;
    }

    // Write the file inside the user folder
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

    // Check if username is on conectados.txt
    FILE *conectadosFile = fopen("../usuarios/conectados.txt", "r");
    if (conectadosFile == NULL) {
        perror("Error al abrir conectados file\n");
        sprintf(res, "3");
        return;
    }

    char line[256];
    int lineCount = 0;

    while (fgets(line, sizeof(line), conectadosFile)) {
        lineCount++;
    }

    fclose(conectadosFile);
    sprintf(res, "0");
    *res2 = lineCount; // Update the value pointed to by res2
    return;
} 

void list_content_serv(char *username, char *username_folder, char *res, int *res2, char *res_username) {
    char foldername[50];
    sprintf(foldername, "../usuarios/%s", username_folder);

    // Check if the folder exists
    DIR *dir = opendir(foldername);
    if (dir == NULL) {
        // Folder does not exist
        sprintf(res, "Usuario no registrado\n");
        *res2 = -1; // Indicar que ocurrió un error
        return;
    }

    int fileCount = 0;
    struct dirent *entry;

    // Count files in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            fileCount++;
        }
    }

    closedir(dir);

    // Set the results
    sprintf(res, "0");
    *res2 = fileCount;
    sprintf(res_username, "%s", username_folder);
    return;
}
