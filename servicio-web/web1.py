from zeep import Client

# URL del servicio web WSDL
wsdl_url = 'http://localhost:5000/?wsdl'

# Crear un cliente Zeep
client = Client(wsdl=wsdl_url)

# Llamar al método get_timestamp del servicio web
timestamp = client.service.get_timestamp()

# Imprimir el timestamp devuelto por el servicio web
print(timestamp)
