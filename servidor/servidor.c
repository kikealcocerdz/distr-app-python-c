#include "clavesservidor.h"
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <dirent.h>
#include "comm.h"
#include "../servicio-web/servidorweb.h"

#define MAXSIZE 256

pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = true;
pthread_cond_t cond_mensaje;

void tratar_mensaje(void *arg) {
    int sc = *(int *)arg;
    int ret;
    int ret2;
    int res2;
    char op='\0';
    char fecha[256]="";
    char recibido[256]="";
    char value1[256]="", operacion[256]="", res[256]="", res_clients[256]="", res_username[50]="", attr2[256]="", attr3[256]="", attr4[256]="";
    char V_Value2[256]="";
    int N_Value2, key;
    CLIENT *clnt;
	enum clnt_stat retval;
    int result_1;
    char *host = "localhost";

    pthread_mutex_lock(&mutex_mensaje);

    if (readLine(sc, (char *)&operacion, MAXSIZE) == -1) {
        perror("error al recvMessage 2");
        pthread_mutex_unlock(&mutex_mensaje);
        return;
    }
    //Asignamos que operacion se ha de realizar.
    printf("operacion recibida: %s\n", operacion);
    if (strcmp(operacion, "REGISTER") == 0) {
        op = '0';}
    else if (strcmp(operacion, "UNREGISTER") == 0) {
        op = '1';}
    else if (strcmp(operacion, "CONNECT") == 0) {
        op = '2';}
    else if (strcmp(operacion, "PUBLISH") == 0) {
        op = '3';}
    else if (strcmp(operacion, "DELETE") == 0) {
        op = '4';}
    else if (strcmp(operacion, "LIST_USERS") == 0) {
        op = '5';}
    else if (strcmp(operacion, "LIST_CONTENT") == 0) {
        op = '6';}
    else if (strcmp(operacion, "DISCONNECT") == 0) {
        op = '7';}
    else if (strcmp(operacion,"GET_FILE") == 0 ){
        op = '8';}
    

    clnt = clnt_create(host, SERVIDOR_RPC, SERVIDOR_RPCVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror("Error al crear el cliente");
        return;
    }
    
    switch (op) {
        case '0':
            printf("REGISTER2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            printf("fechilla %s user %s\n", fecha, attr2);
            register_serv(attr2, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            printf("estoy joya");
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);   
            printf("Usuario recibido: %s\n", attr2);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
        case '1':
            printf("UNREGISTER2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            unregister_serv(attr2, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);   
            
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
        case '2':
            printf("CONNECT2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&attr3, MAXSIZE) == -1) {
                perror("error al recvMessage 3");
                return;
            }
            if (readLine(sc, (char *)&attr4, MAXSIZE) == -1) {
                perror("error al recvMessage 3");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            connect_serv(attr2, attr3, attr4, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);
            printf("Respuesta: %s\n", res);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
        case '3':
            printf("PUBLISH2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&attr3, MAXSIZE) == -1) {
                perror("error al recvMessage 3");
                return;
            }
            if (readLine(sc, (char *)&attr4, MAXSIZE) == -1) {
                perror("error al recvMessage 3");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            publish_serv(attr2, attr3, attr4, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);
            
            printf("Fichero recibido para publicar: %s\n", attr2);
            printf("Respuesta: %s\n", res);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
            
        case '4':
            printf("DELETING2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&attr3, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            delete_serv(attr2, attr3, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);
            
            printf("Respuesta: %s\n", res);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
            
        case '5':
            printf("LIST_USERS2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            list_users_serv(attr2, res, &res2);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);

            printf("Respuesta: %s\n", res);
            printf("Respuesta: %d\n", res2);
            sprintf(res_clients, "%d", res2);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            sleep(1);
            ret2 = sendMessage(sc, res_clients, strlen(res_clients) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }

            for (int i = 0; i < res2; i++) {
                int ret_cliente;
                char res_cliente[256];
                FILE *fp = fopen("../usuarios/conectados.txt", "r");
                if (fp == NULL) {
                    perror("Error opening conectados file\n");
                    return;
                }
                
                // Skip lines until reaching the desired line
                for (int j = 0; j < i; j++) {
                    if (fgets(res_cliente, sizeof(res_cliente), fp) == NULL) {
                        fclose(fp);
                        return; // Error handling, maybe break the loop or handle accordingly
                    }
                }
                
                // Read the line to be sent
                if (fgets(res_cliente, sizeof(res_cliente), fp) != NULL) {
                    // Send the line
                    printf("Respuesta: %s\n", res_cliente);
                    ret_cliente = sendMessage(sc, res_cliente, strlen(res_cliente) + 1);
                    if (ret_cliente == -1) {
                        fclose(fp);
                        return; // Error handling, maybe break the loop or handle accordingly
                    } 
                }
                if (readLine(sc, (char *)&recibido, MAXSIZE) == -1) {
                    perror("error al recvMessage 2");
                    return;
                }   
                printf("Respuesta recibida: %s\n", recibido);   
                fclose(fp);
            }
            break;
        
        case '6':
            printf("LIST_CONTENT2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&attr3, MAXSIZE) == -1) {
                perror("error al recvMessage 3");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            printf("Username recibido: %s\n", attr2);
            list_content_serv(attr2, attr3, res, &res2, res_username);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);

            printf("Respuesta: %s\n", res);
            printf("Respuesta: %d\n", res2);
            printf("Respuesta usuario: %s\n", res_username);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            sprintf(res_clients, "%d", res2);
            ret2 = sendMessage(sc, res_clients, res2 + 1);
            if (ret2 == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }

            char ruta[100];
            int fileCount = 0;
            sprintf(ruta, "../usuarios/%s", res_username);
            DIR *dir;
            struct dirent *entry;
            if ((dir = opendir(ruta)) != NULL) {
                while ((entry = readdir(dir)) != NULL && fileCount < res2) {
                    int ret_cliente;
                    char res_cliente[256];
                    if (entry->d_type == DT_REG) { // Solo procesa archivos regulares
                        char file_path[60];
                        sprintf(file_path, "%s/%s", ruta, entry->d_name);
                        FILE *fp = fopen(file_path, "r");
                        if (fp != NULL) {
                            char description[256];
                            fgets(description, sizeof(description), fp); // Lee el contenido del archivo
                            // Concatenar el nombre del archivo y su contenido en una sola cadena
                            snprintf(res_cliente, sizeof(res_cliente), "%s  %s", entry->d_name, description);
                            // Enviar la cadena que contiene el nombre del archivo y su contenido
                            ret_cliente = sendMessage(sc, res_cliente, strlen(res_cliente) + 1);
                            if (ret_cliente == -1) {
                                fclose(fp);
                                return; // Manejo del error, tal vez romper el bucle o manejarlo de acuerdo
                            }
                            fclose(fp);
                            fileCount++;
                        } else {
                            printf("No se pudo abrir el archivo %s\n", file_path);
                        }
                    }
                }
                closedir(dir);
            } else {
                printf("No se pudo abrir el directorio %s\n", ruta);
                return;
            }
            break;

        case '7':
            printf("DISCONNECT2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            if (readLine(sc, (char *)&fecha, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            disconnect_serv(attr2, res);
            retval = terminal_rpc_1(op, fecha, attr2, &result_1, clnt);
            if (retval != RPC_SUCCESS) {
                clnt_perror(clnt, "Error al llamar al procedimiento remoto");
                return;
            }         
            clnt_destroy(clnt);
            printf("Respuesta: %s\n", res);
            ret = sendMessage(sc, res, strlen(res) + 1);
            if (ret == -1) {
                pthread_mutex_unlock(&mutex_mensaje);
                return;
            }
            break;
                
            default:
                strcpy(res, "Operación no válida");
                break;
        }
        mensaje_no_copiado = false;
        pthread_cond_signal(&cond_mensaje);
        pthread_mutex_unlock(&mutex_mensaje);
        close(sc); // Cerrar el socket después de enviar la respuesta

        pthread_exit(NULL);
    }

int main(int argc, char *argv[]) {
    int sd, sc;
    pthread_attr_t t_attr;
    pthread_t thid;

    // Verificar que se pasa el puerto como argumento
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return -1;
    }
    int PORT_SERVER = atoi(argv[1]);


    sd = serverSocket(INADDR_ANY, PORT_SERVER, SOCK_STREAM);
    if (sd < 0) {
        printf("SERVER: Error en serverSocket\n");
        return 0;
    }

    pthread_mutex_init(&mutex_mensaje, NULL);
    pthread_cond_init(&cond_mensaje, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    FILE *fp = fopen("../usuarios/conectados.txt", "w");
    if (fp != NULL) {
        fclose(fp);
    } else {
        printf("No se pudo abrir conectados.txt para borrar contenido.\n");
    }

    while (1) {
        sc = serverAccept(sd);
        printf("Sc tiene valor %d\n", sc);
        if (sc < 0) {
            printf("Error en serverAccept\n");
            continue;
        }
        // Crear un hilo para tratar el mensaje
        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, (void *)&sc) == 0) {
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado)
                pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            mensaje_no_copiado = true;
            pthread_mutex_unlock(&mutex_mensaje);
        }
    }

    return 0;
}