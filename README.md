# Kyma

Kyma es un multímetro digital que registra, almacena y analiza datos en tiempo real, ofreciendo una visualización interactiva y accesible desde un panel web.

### Descripción del proyecto

Un multímetro permite medir valores como el voltaje o el consumo de un circuito en un momento puntual. Sin embargo, en muchos casos resulta útil disponer de una traza continua de esos valores, como en la imagen de ejemplo. Esto facilita analizar el comportamiento del sistema con más detalle y detectar picos de consumo asociados a determinadas operaciones.

![](/docs/signal.png)


Kyma es un módulo que se conecta en serie a un circuito y permite registrar en tiempo real y conservar la traza del voltaje, la corriente y la potencia usada por el mismo.

### Arquitectura

![](/docs/arch1.jpg)

El microcontrolador envía sus lecturas por MQTT a Node-RED, que las reexpone mediante un websocket. Esto permite que el panel web —que no puede usar MQTT desde el navegador— reciba los datos en tiempo real.

Además, Node-RED sirve la propia página del panel, que no es más que una página estática que abre un websocket y dibuja las lecturas en una gráfica.

Aunque Mosquitto también ofrece MQTT por websocket y permitiría conectar el panel directamente, pasar por Node-RED resulta útil porque permite almacenar, transformar o procesar los datos antes de enviarlos al frontend.

Simplemente a modo de detalle, en realidad Mosquitto y NodeRed están contenerizados para facilitar su despliegue y manejo:

![](/docs/arch2.jpg)

### Node-RED

La configuración de Node-RED es muy sencilla y se basa en dos flujos independientes: uno para servir la página web y otro para reenviar, vía WebSocket, los datos que llegan por MQTT. El microcontrolador envía las lecturas en un JSON con la estructura necesaria, por lo que no hace falta aplicar transformaciones intermedias.

![](/docs/node-red.png)


### Hardware

El módulo físico de Kyma se compone de:

* Sensor de tensión INA219
* Wemos D1 Mini como microcontrolador. Encargado de recoger los datos del sensor y transmitirlos vía MQTT

### Demo
