from enum import Enum
import argparse
import socket
import threading
import sys
import re
import time
import os
from zeep import Client


class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _fileDirectory = None
    _connected_user = None
    _serverSock = None
    _isConnected = False
    _serverThread = None
    _threadAddress = None
    _list_users = { }

    wsdl_url = 'http://localhost:5000/?wsdl'
    # Crear un cliente Zeep
    clientweb = Client(wsdl=wsdl_url)


    # ******************** METHODS *******************


    @staticmethod
    def register(user):
        try:
            # Creamos el socket para conectarnos al servidor
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            if len(user) > 255:
                print("REGISTER FAIL")
                return client.RC.USER_ERROR
            else:                
                # Envíamos el mensaje de registro
                message = "REGISTER\0"
                sock.sendall(message.encode())
                sock.sendall(user.encode() + "\0".encode())

                # Obtenemos el timestamp del servidor web
                timestamp = client.clientweb.service.get_timestamp()
                sock.sendall(timestamp.encode() + "\0".encode())

                # Recibimos la respuesta del servidor
                respuesta = sock.recv(16).decode()

                if respuesta[0] == "0":
                    print('REGISTER OK')
                elif respuesta[0] == "1":
                    print('USERNAME IN USE')
                elif respuesta[0] == "2":
                    print('REGISTER FAIL')
                else:
                    print('REGISTER FAIL')
            
            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK

            
        except Exception as e:
            print("Exception during registration:", str(e))
            return client.RC.ERROR
        

   
    @staticmethod
    def  unregister(user) :
        try:
            # Es necesario conectarse al servidor para poder darse de baja en el sistema
            # para evitar que un usuario externo pueda dar de baja a otro usuario
            if client._connected_user == user:
                client.disconnect(user)
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_address = (client._server, client._port)
                sock.connect(server_address)
                
                message = "UNREGISTER\0"
                # Enviamos el mensaje de baja
                sock.sendall(message.encode())
                
                sock.sendall(user.encode() + "\0".encode())

                # Obtenemos el timestamp del servidor web
                timestamp = client.clientweb.service.get_timestamp()
                sock.sendall(timestamp.encode() + "\0".encode())

                # Recibimos la respuesta del servidor
                respuesta = sock.recv(16).decode("utf-8")

                if respuesta[0] == "0":
                    print('UNREGISTER OK')
                elif respuesta[0] == "1":
                    print('USER DOES NOT EXIST')
                elif respuesta[0] == "2":
                    print('UNREGISTER FAIL')
                else:
                    print('UNREGISTER FAIL')

                sock.close()  # Cierra el socket después de usarlo

            else:
                print('UNREGISTER FAIL / NO ERES TÚ')
            
            return client.RC.OK
            
        except Exception as e:
            print("Exception during unregistration:", str(e))

    @staticmethod
    def connect(user):
        try:
            # Creamos el socket para conectarnos al servidor
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)

            # Creamos un socket para el servidor del cliente y encontramos un puerto libre
            server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_sock.bind(('localhost', 0))
            server_sock.listen(1)
            free_server, free_port = server_sock.getsockname()
            client._serverSock = server_sock
        
            # Enviamos el mensaje de conexión
            message = "CONNECT\0"
            sock.sendall(message.encode())

            sock.sendall(user.encode() + b"\0")

            # Enviamos el puerto y la dirección del servidor del cliente
            sock.sendall(str(free_port).encode() + b"\0")

            sock.sendall(str(free_server).encode() + b"\0")

            
            timestamp = client.clientweb.service.get_timestamp()
            sock.sendall(timestamp.encode() + "\0".encode())

            respuesta = sock.recv(1).decode("utf-8")
            
            if respuesta[0] == "0":
                # Creamos un hilo para manejar la conexión con el servidor del cliente
                client._isConnected = True
                client._serverThread = threading.Thread(target=client.handle_server_connection, args=(user, client._serverSock, free_server, free_port, client._fileDirectory))
                client._serverThread.start()
                
                # Establecemos los atributos del cliente
                client._connected_user = user
                client._threadAddress = (free_server, free_port)
                print('CONNECT OK')


            elif respuesta[0] == "1":
                print('CONNECT FAIL, USER DOES NOT EXIST')
                return client.RC.ERROR

            elif respuesta[0] == "2":
                print('USER ALREADY CONNECTED')
                return client.RC.ERROR

            else:
                print('CONNECT FAIL')
                return client.RC.ERROR

            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK

        except Exception as e:
            print("Exception during connection:", str(e))
            return client.RC.ERROR
    
    @staticmethod
    def  disconnect(user) :
        try:
            # Comprobamos que el usuario que se quiere desconectar es el mismo que el usuario conectado
            if user != client._connected_user:
                print('DISCONNECT FAIL / NO ERES TÚ')
                return client.RC.ERROR
            
            else:
                # Creamos un socket para conectarnos al servidor
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_address = (client._server, client._port)
                sock.connect(server_address)
                
                # Enviamos el mensaje de desconexión
                message = "DISCONNECT\0"
                sock.sendall(message.encode())
        
                sock.sendall(client._connected_user.encode() + "\0".encode())
                timestamp = client.clientweb.service.get_timestamp()
                sock.sendall(timestamp.encode() + "\0".encode())

                respuesta = sock.recv(1).decode("utf-8")

                if respuesta[0] == "0":
                    # Cerramos el socket del servidor del cliente y esperamos a que el hilo termine
                    sock_thread = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sock_thread.connect(client._threadAddress)

                    client._isConnected = False
                    client._connected_user = None
                    
                    sock_thread.shutdown(socket.SHUT_RDWR)
                    sock_thread.close()
                    client._serverSock.close()
                    client._serverThread.join()

                    print('DISCONNECT OK')

                elif respuesta[0] == "1":
                    print('DISCONNECT FAIL / USER DOES NOT EXIST')
                elif respuesta[0] == "2":
                    print('DISCONNECT FAIL / USER NOT CONNECTED')
                else:
                    print('DISCONNECT FAIL') 
                
                sock.close()  # Cierra el socket después de usarlo
                return client.RC.OK
            
        except Exception as e:
            print("Exception during disconnection:", str(e))
            return client.RC.ERROR

    @staticmethod
    def publish(fileName, description):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)

            message = "PUBLISH\0"
            sock.sendall(message.encode())

            if client._isConnected:
                sock.sendall(client._connected_user.encode() + "\0".encode())
                #Comprobamos si el archivo existe en el directorio pasado por el cliente
                if not os.path.exists(client._fileDirectory + "/" + fileName + '.txt'):
                    print('PUBLISH FAIL, FILE DOES NOT EXIST')
                    # Si no existe enviamos notfound, que el servidor interpretará como que el archivo no existe
                    sock.sendall("notfound".encode() + "\0".encode())
                    sock.sendall(description.encode() + "\0".encode())
                    timestamp = client.clientweb.service.get_timestamp()
                    sock.sendall(timestamp.encode() + "\0".encode())
                    return client.RC.ERROR

                sock.sendall(fileName.encode() + "\0".encode())

                sock.sendall(description.encode() + "\0".encode())

                timestamp = client.clientweb.service.get_timestamp()
                sock.sendall(timestamp.encode() + "\0".encode())

                respuesta = sock.recv(1).decode("utf-8")
                
                if respuesta[0] == "0":
                    print('PUBLISH OK')

                elif respuesta[0] == "1":
                    print('PUBLISH FAIL, USER DOES NOT EXIST')

                elif respuesta[0] == "2":
                    print('PUBLISH FAIL, USER NOT CONNECTED')
                elif respuesta[0] == "3":
                    print('PUBLISH FAIL, FILE ALREADY EXISTS')

                else:
                    print('PUBLISH FAIL')

            else:
                print('PUBLISH FAIL, USER NOT CONNECTED')

            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK

        except Exception as e:
            print("Exception during publishing:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  delete(fileName) :
        try:
            # Creamos el socket para conectarnos al servidor
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "DELETE\0"
            # Enviamos el mensaje de borrado
            sock.sendall(message.encode())
            sock.sendall(client._connected_user.encode() + "\0".encode())
            
            sock.sendall(fileName.encode() + "\0".encode())
            
            timestamp = client.clientweb.service.get_timestamp()
            sock.sendall(timestamp.encode() + "\0".encode())

            respuesta = sock.recv(1).decode("utf-8")

            if respuesta[0] == "0":
                print('DELETE OK')
            elif respuesta[0] == "1":
                print('DELETE FAIL, USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('DELETE FAIL, USER NOT CONNECTED')
            elif respuesta[0] == "3":
                print('DELETE FAIL, CONTENT NOT PUBLISHED')
            else:
                print('DELETE FAIL')

            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK  
            
        except Exception as e:
            print("Exception deleting the file:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  listusers() :
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "LIST_USERS\0"
            sock.sendall(message.encode())

            # Si el usuario no está conectado, enviamos un mensaje vacío que el servidor interpretará como que no hay usuario conectado
            if client._connected_user is None:
                sock.sendall("".encode() + "\0".encode())
            else:
                sock.sendall(client._connected_user.encode() + "\0".encode())
            
            timestamp = client.clientweb.service.get_timestamp()
            sock.sendall(timestamp.encode() + "\0".encode())

            respuesta = sock.recv(16)
            respuesta = respuesta.decode("utf-8")
            
            sock.sendall("OK\0".encode())

            numero_de_conectados = sock.recv(16)
            numero_de_conectados = numero_de_conectados.decode("utf-8")
            numero_de_conectados = numero_de_conectados.strip('\x00')

            if respuesta[0] == "0":
                print('LIST_USERS OK')
                client._list_users.clear()
                # Iteramos sobre el número de usuarios conectados
                for i in range(int(numero_de_conectados)):
                    usuario_conectado = sock.recv(256).decode("utf-8")
                    print("\t" + usuario_conectado)

                    # Enviamos una confirmación para no solapar los mensajes
                    sock.sendall("OK\0".encode())

                    user_info = usuario_conectado.split(" ")
                    for i in range(len(user_info)):
                        # Eliminamos los caracteres nulos y saltos de línea
                        user_info[i] = user_info[i].strip('\x00')
                        user_info[i] = user_info[i].strip('\n')
                        client._list_users[user_info[0]] = (user_info[1], int(user_info[2]))
                    

            elif respuesta[0] == "1":
                print('LIST_USERS FAIL, USER DOES NOT EXIST')
            
            elif respuesta[0] == "2":
                print('LIST_USERS FAIL, USER NOT CONNECTED')
            
            else:
                print('LIST_USERS FAIL')

            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK

        except Exception as e:
            print("Exception listing users:", str(e))
            return client.RC.ERROR
        

    @staticmethod
    def  listcontent(user):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "LIST_CONTENT\0"
            sock.sendall(message.encode())
            sock.sendall(client._connected_user.encode() + "\0".encode())
            
            sock.sendall(user.encode() + "\0".encode())

            timestamp = client.clientweb.service.get_timestamp()
            sock.sendall(timestamp.encode() + "\0".encode())

            respuesta = sock.recv(1024).decode("utf-8")
            
            sock.sendall("OK\0".encode())

            numero_de_archivos = sock.recv(1024).decode("utf-8")
            numero_de_archivos = numero_de_archivos.strip('\x00')


            if respuesta[0] == "0":
                print('LIST_CONTENT OK')
                for i in range(int(numero_de_archivos)):
                    archivo = sock.recv(1024).decode("utf-8")
                    # Imprimir un tabulador seguido del nombre del archivo
                    print("\t" + archivo)

            elif respuesta[0] == "1":
                print('LIST_CONTENT FAIL, USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('LIST_CONTENT FAIL, USER NOT CONNECTED')
            else:
                print('LIST_CONTENT FAIL')
            
            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK
            
        except Exception as e:
            print("Exception listing content:", str(e))
            return client.RC.ERROR

    
    @staticmethod
    def getfile(user, remote_FileName, local_FileName):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            if user not in client._list_users:
                print('GET_FILE FAIL, USER DOES NOT EXIST')
                return client.RC.ERROR
            thread = client._list_users.get(user)
            if thread is None:
                print('GET_FILE FAIL, USER NOT CONNECTED')
                return client.RC.ERROR

            sock.connect(thread)
            message = "GET_FILE\0" 
            sock.sendall(message.encode())

            sock.sendall(str(len(user)).encode() + "\0".encode())
            sock.sendall(user.encode() + "\0".encode())

            sock.sendall(remote_FileName.encode() + "\0".encode())
            
            respuesta = sock.recv(256).decode("utf-8")

            if respuesta[0] == "0":
                # Recibimos el archivo por chunks de 256 bytes y lo escribimos en el archivo local
                with open(client._fileDirectory + "/" + local_FileName + '.txt', 'wb') as local_file:
                    while True:
                        file_chunk = sock.recv(256)
                        if file_chunk == b'FINARCHIVO\0':
                            break
                        local_file.write(file_chunk)
                        sock.sendall('OK\0'.encode())
                print('GET_FILE OK')
            elif respuesta[0] == "2":
                print('GET_FILE FAIL, FILE NOT FOUND')
            else:
                print('GET_FILE FAIL')
            
            sock.close()  # Cierra el socket después de usarlo
            return client.RC.OK
            
        except Exception as e:
            print("Exception getting the file:", str(e))
            return client.RC.ERROR
        
        finally:
            sock.close()


    
    @staticmethod
    def handle_server_connection(user, server_sock, free_server, free_port, file_directory):
        try:
            while client._isConnected:
                connection, client_address = server_sock.accept()
                message = connection.recv(256).decode("utf-8")
                if message.startswith("GET_FILE"):
                    parts = message.split("\0")

                    user = parts[2]
                    remote_file_name = parts[3]
                    
                    pathFile = file_directory + "/" + remote_file_name + '.txt'
                    # Check if the remote file exists and is accessible
                    if os.path.exists(pathFile):
                        # Read the content of the remote file
                        connection.sendall(('0\0').encode())
                        with open(pathFile, 'rb') as remote_file:
                            while True:
                                file_chunk = remote_file.read(256)
                                if not file_chunk:
                                    connection.sendall(b'FINARCHIVO\0')
                                    break
                                connection.sendall(file_chunk)
                                comprobacion = connection.recv(256)
                    else:
                        # Send code 2 to indicate that the remote file does not exist
                        connection.sendall(('2\0').encode())
        

        except Exception as e:
            print("Exception in server connection thread:", str(e))




    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
                    
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port> -f <filedirectory>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        parser.add_argument('-f', type=str, required=True, help='File directory')

        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port> -f <filedirectory>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False
        
        client._server = args.s
        client._port = args.p
        client._fileDirectory = args.f

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        #  Write code here
        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])