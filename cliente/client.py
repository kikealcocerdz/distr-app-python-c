from enum import Enum
import argparse
import socket
import threading
import sys
import re

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
    _connected_user = None
    _serverSock = None
    _isConnected = False
    _serverThread = None
    _list_users = { }


    # ******************** METHODS *******************


    @staticmethod
    def register(user):
        print("Registering user: " + user)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "REGISTER\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            
            print('Sending user: ' + user)
            sock.sendall(user.encode() + "\0".encode())
            respuesta = sock.recv(1024).decode()

            if respuesta[0] == "0":
                print('REGISTER OK')
            elif respuesta[0] == "1":
                print('REGISTER IN USE')
            elif respuesta[0] == "2":
                print('REGISTER FAIL')
            else:
                print('REGISTER FAIL')

            
        except Exception as e:
            print("Exception during registration:", str(e))
            return client.RC.ERROR
        

   
    @staticmethod
    def  unregister(user) :
        print("Unregistering user: " + user)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "UNREGISTER\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            
            print('Sending user: ' + user)
            sock.sendall(user.encode() + "\0".encode())
            respuesta = sock.recv(1024).decode("utf-8")
            if respuesta[0] == "0":
                print('UNREGISTER OK')
            elif respuesta[0] == "1":
                print('USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('REGISTER FAIL')
            else:
                print('REGISTER FAIL')
            
        except Exception as e:
            print("Exception during unregistration:", str(e))

    @staticmethod
    def connect(user):
        print("Connecting user: " + user)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)

            server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_sock.bind(('localhost', 0))
            server_sock.listen(1)
            free_server, free_port = server_sock.getsockname()
            print('Free server: ' + str(free_server))
            print('Free port: ' + str(free_port))
            client._serverSock = server_sock
            # Start a new thread to handle server connection
            client._serverThread = threading.Thread(target=client.handle_server_connection, args=(user, client._serverSock, free_server, free_port))
            client._serverThread.start()

            message = "CONNECT\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())

            print('Sending user: ' + user) 
            sock.sendall(user.encode() + b"\0")

            print('Sending port: ' + str(free_port))
            sock.sendall(str(free_port).encode() + b"\0")

            print('Sending server: ' + str(free_server))
            sock.sendall(str(free_server).encode() + b"\0")

            respuesta = sock.recv(1024).decode("utf-8")
            print('Received message: ' + respuesta)
            
            if respuesta[0] == "0":
                sock_thread = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                thread_address = (free_server, free_port)
                sock_thread.connect(thread_address)  
                client._connected_user = user
                client._isConnected = True
                print('CONNECT OK')

            elif respuesta[0] == "1":
                print('CONNECT FAIL, USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('USER ALREADY CONNECTED')
            else:
                print('CONNECT FAIL')

            # Close the connection after processing
            sock.close()

        except Exception as e:
            print("Exception during connection:", str(e))
            return client.RC.ERROR
    
    @staticmethod
    def  disconnect(user) :
        print("Disconnecting user: " + user)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "DISCONNECT\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            
            print('Sending user: ' + user)
            sock.sendall(user.encode() + "\0".encode())
            respuesta = sock.recv(1024).decode("utf-8")
            if respuesta[0] == "0":
                client._serverSock.shutdown(socket.SHUT_RDWR)
                client._serverSock.close()
                client._isConnected = False
                client._serverThread.join()
                client._connected_user = None
                print('DISCONNECT OK')

            elif respuesta[0] == "1":
                print('DISCONNECT FAIL / USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('DISCONNECT FAIL / USER NOT CONNECTED')
            else:
                print('DISCONNECT FAIL') 
            
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
            print('Sending message: ' + message)
            sock.sendall(message.encode())

            print('Sending user: ' + str(client._connected_user)) 
            sock.sendall(client._connected_user.encode() + "\0".encode())

            print('Sending file name: ' + fileName)
            sock.sendall(fileName.encode() + "\0".encode())

            print('Sending description: ' + description)
            sock.sendall(description.encode() + "\0".encode())

            respuesta = sock.recv(1024).decode("utf-8")
            
            print('Received message: ' + respuesta[0])
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

        except Exception as e:
            print("Exception during publishing:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  delete(fileName) :
        print("Deleting file: " + fileName)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "DELETE\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            print('Sending user: ' + client._connected_user)
            sock.sendall(client._connected_user.encode() + "\0".encode())
            
            print('Sending user: ' + fileName)
            sock.sendall(fileName.encode() + "\0".encode())
            
            respuesta = sock.recv(1024).decode("utf-8")
            print('Received message: ' + respuesta  )

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
            return client.RC.OK  # Return appropriate value based on 
            
        except Exception as e:
            print("Exception deleting the file:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  listusers() :
        print("Listing users")
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "LIST_USERS\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())

            print('Sending user: ' + client._connected_user)
            sock.sendall(client._connected_user.encode() + "\0".encode())
            
            respuesta = sock.recv(1024).decode("utf-8")
            numero_de_conectados = sock.recv(1024).decode("utf-8")
            numero_de_conectados = numero_de_conectados.strip('\x00')

            print('Received message  111: ' + respuesta)

            if respuesta[0] == "0":
                print('LIST_USERS OK')
                for i in range(1):
                    usuario_conectado = sock.recv(1024).decode("utf-8")
                    print("\t" + usuario_conectado + "\n")
                    usuario_conectado_cleaned = re.sub(r'[^\w.\s]', '', usuario_conectado)
                    # Here, [^\w.\s] means any character that is not alphanumeric, a dot, or whitespace
                    print(usuario_conectado_cleaned.split(" "))
                    #client._list_users.update({ : usuario_conectado[:-1] })
                print(client._list_users)

            elif respuesta[0] == "1":
                print('LIST_USERS FAIL')
            else:
                print('LIST_USERS FAIL')

        except Exception as e:
            print("Exception listing users:", str(e))
            return client.RC.ERROR
        

    @staticmethod
    def  listcontent(user) :
        print("Listing content of user: " + user)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (client._server, client._port)
            sock.connect(server_address)
            
            message = "LIST_CONTENT\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            print('Sending connected user: ' + client._connected_user)
            sock.sendall(client._connected_user.encode() + "\0".encode())
            
            print('Sending user to get files: ' + user)
            sock.sendall(user.encode() + "\0".encode())

            
            respuesta = sock.recv(1024).decode("utf-8")
            numero_de_archivos = sock.recv(1024).decode("utf-8")
            numero_de_archivos = numero_de_archivos.strip('\x00')

            print('Received message: ' + respuesta)
            print('Received number of files: ' + numero_de_archivos)

            if respuesta[0] == "0":
                print('LIST_CONTENT OK')
                for i in range(int(numero_de_archivos)):
                    archivo = sock.recv(1024).decode("utf-8")
                    print("\t" + archivo, end='')  # Imprimir un tabulador seguido del nombre del archivo                    sys.stdout.flush()  # Forzar la salida inmediata
                    print()  # Agregar un salto de línea después de cada archivo

            elif respuesta[0] == "1":
                print('LIST_CONTENT FAIL, USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('LIST_CONTENT FAIL, USER NOT CONNECTED')
            else:
                print('LIST_CONTENT FAIL')
            
        except Exception as e:
            print("Exception listing content:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        print("User " + user + " wants to download file " + remote_FileName + " to " + local_FileName)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect(client._serverSock)
            
            message = "GET_FILE\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())
            print('Sending user: ' + user)
            sock.sendall(user.encode() + "\0".encode())
            print('Sending remote file name: ' + remote_FileName)
            sock.sendall(remote_FileName.encode() + "\0".encode())
            print('Sending local file name: ' + local_FileName)
            sock.sendall(local_FileName.encode() + "\0".encode())
            
            respuesta = sock.recv(1024).decode("utf-8")
            print('Received message: ' + respuesta)
            sock.close()  # Cierra el socket después de usarlo

            return client.RC.OK
        
        except Exception as e:
            print("Exception getting the file:", str(e))
            return client.RC.ERROR
        
        finally:
            sock.close()
    
    
    @staticmethod
    def handle_server_connection(user, server_sock, free_server, free_port):
        try:
            print('Server connection thread started on port:', free_port, user, free_server)
            while True:
                connection, client_address = server_sock.accept()
                print('Connection accepted from:', client_address)
                message = connection.recv(1024).decode("utf-8")
                if message == "GET_FILE\0":
                    print("Received GET FILE request.")
                    # Aquí puedes proceder con la funcionalidad relacionada con la solicitud de GET FILE
                else:
                    print("Received unexpected message:", message)
                
        except Exception as e:
            print("Exception in server connection thread:", str(e))
                

                
                 
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
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False
        
        client._server = args.s
        client._port = args.p

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