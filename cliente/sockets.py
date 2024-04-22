import socket
import sys

sock = _server
port = _port

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
server_address = (sock, port)

print('conectando a {} en el puerto {}'.format(*server_address))
sock.connect(server_address)

try:
    message = b'Esto es una cadena de prueba\0'
    sock.sendall(message)

    message = ''
    while True:
        msg = sock.recv(1)
        if (msg == b'\0'):
            break
        message += msg.decode()
    message = message + "\0"

    print('mensaje: ' + message)
finally:
    sock.close()