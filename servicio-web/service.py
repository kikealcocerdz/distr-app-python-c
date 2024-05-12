from spyne import Application, ServiceBase, rpc
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication
from wsgiref.simple_server import make_server

class TimestampService(ServiceBase):
    @rpc(_returns=str)
    def get_timestamp(self):
        from datetime import datetime
        now = datetime.now()
        timestamp_str = now.strftime('%d-%m-%Y %H:%M:%S')
        return timestamp_str

# Crear la aplicación Spyne
application = Application([TimestampService], 'http://localhost:5000/',
                           in_protocol=Soap11(validator='lxml'),
                           out_protocol=Soap11())

# Crear el servidor WSGI
wsgi_application = WsgiApplication(application)

# Iniciar el servidor
if __name__ == '__main__':
    server = make_server('localhost', 5000, wsgi_application)
    print("Listening on http://localhost:5000...")
    server.serve_forever()
