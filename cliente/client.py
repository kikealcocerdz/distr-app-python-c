from enum import Enum
import argparse
import socket
import threading
import sys

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
            return client.RC.ERROR


    
    @staticmethod
    def connect(user):
        print("Unregistering user: " + user)
        try:
            server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_sock.bind(('localhost', 0))
            server_sock.listen(1)
            free_server, free_port = server_sock.getsockname()
            print('Listening on port: ' + str(free_port))
            print('Server address: ' + str(free_server))

            def handle_client_requests():
                while True:
                    connection, client_address = server_sock.accept()
                    # Your logic to handle client requests goes here
                    data = connection.recv(1024).decode()
                    if data == "GET_PORT":
                        print('Sending free port: ' + str(free_port))
                        connection.sendall(str(free_port).encode())
                    else:
                        # Handle other client requests
                        pass

            thread = threading.Thread(target=handle_client_requests)
            thread.start()

            message = "CONNECT\0"
            print('Sending message: ' + message)
            sock.sendall(message.encode())

            print('Sending user: ' + user)
            sock.sendall(user.encode() + b"\0")  
            sock.sendall(str(free_port).encode())  
            respuesta = sock.recv(1024).decode("utf-8")
            if respuesta[0] == "0":
                print('CONNECT OK')
            elif respuesta[0] == "1":
                print('USER DOES NOT EXIST')
            elif respuesta[0] == "2":
                print('USER ALREADY CONNECTED')
            else:
                print('CONNECT FAIL')

        except Exception as e:
            print("Exception during connection:", str(e))
            return client.RC.ERROR
        finally:
            sock.close()



    
    @staticmethod
    def  disconnect(user) :
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  publish(fileName,  description) :
        #  Write your code here
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
            print('Sending user: ' + fileName)
            sock.sendall(fileName.encode() + "\0".encode())
            
            msg = sock.recv(1)  # Wait for server response
            print('Received message: ' + msg.decode()   )
            sock.close()  # Cierra el socket después de usarlo

            return client.RC.OK  # Return appropriate value based on 
            
        except Exception as e:
            print("Exception deleting the file:", str(e))
            return client.RC.ERROR

    @staticmethod
    def  listusers() :
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  listcontent(user) :
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        #  Write your code here
        return client.RC.ERROR

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