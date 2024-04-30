#include "clavesservidor.h"
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "comm.h"

#define MAXSIZE 256

pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = true;
pthread_cond_t cond_mensaje;

void tratar_mensaje(void *arg) {
    int sc = *(int *)arg;
    int ret;
    char op='\0';
    char value1[256]="", operacion[256]="", res[256]="", attr2[256]="", attr3[256]="", attr4[256]="";
    char V_Value2[256]="";
    int N_Value2, key;

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
    

    

    switch (op) {
        case '0':
            printf("REGISTER2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            register_serv(attr2, res);
            printf("Usuario recibido: %s\n", attr2);
            break;
        case '1':
            printf("UNREGISTER2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            unregister_serv(attr2, res);
            printf("Usuario recibido para borrar su registro: %s\n", attr2);
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
            connect_serv(attr2, attr3, res);
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
            publish_serv(attr2, attr3, attr4, res);
            printf("Fichero recibido para publicar: %s\n", attr2);
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
            delete_serv(attr2, attr3, res);
            break;
            
        case '5':
            printf("LIST_USERS2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
            list_users_serv(attr2, res);
            break;

        case '7':
            printf("DISCONNECT2\n");
            if (readLine(sc, (char *)&attr2, MAXSIZE) == -1) {
                perror("error al recvMessage 2");
                return;
            }
        default:
            strcpy(res, "Operación no válida");
            break;
    }
    printf("Respuesta: %s\n", res);
    ret = sendMessage(sc, res, strlen(res) + 1);
    if (ret == -1) {
        pthread_mutex_unlock(&mutex_mensaje);
        return;
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