

<center>
<div style="align: right"><img style="position:absolute" src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0d/FIUBA_gris_transparente.png/275px-FIUBA_gris_transparente.png"></div>

<br></br>
<br></br>


# <center>Trabajo práctico 1 - Protocolo DBus</center>
### <center>Taller de programación 1 - Veiga - 1er Cuatrimestre 2020

<br></br>

| | |
|---|---|
|  Alumno | Federico Jure  |
| Padrón  |  97598 |
| Email   | fedejure@gmail.com |
## [<center>Repositorio fuente](https://github.com/FedeJure/DBusProtocol)


<br></br>
</center>
----------------------------------

### descripción del trabajo realizado

> El programa modela una resolucion acotada del protocolo DBus utilizado para la comunicacion entre distintos programas. Como el nombre lo indica, bus, es una vía en donde se envia información de un emisor hasta un receptor, en forma de un stream de bytes. 
<br/>Se utilizo el lenguaje C, y se usaron librerias estandar y la libreria de socket de linux.
<br/> Para que funcióne, se requiere que ambas partes sepan armar y entender el mensaje y para ésto se utiliza un protocolo en este caso DBus.
<br/>
<br/>
<br/>
El programa consiste en dos ejecutables, un cliente y un servidor.
<br/><br/> **Cliente**: Es el encargado de interactuar con el usuario mediante la entrada de un archivo o mediante la entrada estandar, procesar los datos ingresados, armar un mensaje que respete el protocolo, y finalmente enviarlo al servidor. 
<br/><br/> **Servidor:** El servidor estára funcionando en un determinado puerto esperando recibir un mensaje de un cliente mediante una conexion por socket. Una vez establecida una conexión exitosa y recibido un mensaje, se decodificara el mismo respetando el protocolo a seguir y ejecutando la accion requerida. [En este caso no se ejecutara ninguna accion mas que imprimir por pantalla la acción requerida].