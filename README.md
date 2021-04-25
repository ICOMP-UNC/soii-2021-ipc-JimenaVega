
TP1 Sistemas Operativos 2 
## Ingeniería en Computación - FCEFyN - UNC 2021
---

</br>
<p align="center">


  <h3 align="center">Interprocess Communication</h3>

  <p align="center">
    Communication system using IPv4 sockets and System V queue.
    <br>
    <a href="https://reponame/issues/new?template=bug.md">Report bug</a>
    ·
    <a href="https://reponame/issues/new?template=feature.md&labels=feature">Request feature</a>
  </p>
</p>

</br>

---



## Tabla de contenidos
<!-- - [## Ingeniería en Computación - FCEFyN - UNC 2021](#-ingeniería-en-computación---fcefyn---unc-2021)
- [Tabla de contenidos](#tabla-de-contenidos) -->
- [Introduccion](#introduccion)
- [Processes](#processes)
- [Secuencias](#secuencias)
- [Prerequisitos](#prerequisitos)
- [Set up](#set-up)
- [Que está incluido?](#Qué-esta-incluido?)
- [Turn off](#turn-off)
- [Referencias](#referencias)

## Introduccion

En este trabajo se verá la comunicación entre 3 productores que mandan cada una tasa determinada de segundos un mensaje hacia clientes suscriptos en una lista dentro del servidor.


## Processes

-  Server o delivery manager

El server utiliza socket INET con conexión para comunicarse con los clientes. Para poder aceptar a multiples clientes se utilizó la libreria epoll que nos permite detectar cambios de eventos en los clientes.

Mientras que para el otro lado de la comunicación se utiliza una System V queue de la cual el server solo lee los mensajes que los productores le envian.

- CLI

Se planteo la terminal como si fuese un cliente más, esto es, se conecta con el servidor via socket. Utiliza una trama de comunicación con un checksum para validar.


- Productores

Los productores utilizan una cola generada por un archivo comun con el servidor que se pide como requisito mas adelante en este informe.

Su funcion es simplemente generar mensajes cada una tasa determinada de segundos.

- Clientes

Como se dijo anteriormente, estos se comunican con el server via socket. Para poder hacerlo se utiliza la trama de comunicacion definida en la siguiente imagen:



![Frames](/img/frames-SO2-TP1.png)


## Secuencias

1. Primer interaccion entre client y server

![Frames](/img/client_serv_1.png)
1. Interacciones entre CLI y server

![Frames](/img/CLI_serv.png)

1.  Comunicacion regular entre productores, server y clients.

![Frames](/img/cli-serv-pro.png)



## Prerequisitos


- Linux operating system
- Python 3
- Openssl should be installed 
  
  ```sudo apt-get install libssl-dev```
- Makefile
- Libzip. Para instalar referirse [here](#https://libzip.org/documentation/)

## Set up 

Primero crear las carpetas  *bin* con ```mkdir bin``` y *obj* con ```mkdir obj```

Este programa se compila usando```make``` dentro del repositorio.

Para que la cola  System V se genere correctamente : </br>
 ```
 touch /tmp/mqueue_server_key
 ```

Luego para correr el servidor y la CLI vaya dentro de  */bin* y ejecute : 
```
./serv <port> <server-ip>
./CLI <server-ip> <port>
```

Para los clients y producers vaya dentro de */src* :
```
python3 clients.py
python3 producers.py
```


## Qué esta incluido?



```text

├── bin
├── img
├── inc
│    ├── libserv.h
│    ├── lib_glob_struct.h
│    ├── global_var.h
│    └── cient_list.h
├── obj
├── src
│    ├── clean.py
│    ├── CLI.c
│    ├── client.c
│    ├── client_list.c
│    ├── clients.py
│    ├── libserv.c
│    ├── producer_1.c
│    ├── producer_2.c
│    ├── producer_3.c
│    ├── producers.py
│    └── server.c
├── Makefile
└── README.md
```

## Turn off

Terminar todos los procesos

```
python3 clean.py
```


## Referencias

- man epoll(7) : codigo de ejemplo
- msg queue : [https://www.softprayog.in/programming/interprocess-communication-using-system-v-message-queues-in-linux]
- lists : [https://www.geeksforgeeks.org/linked-list-set-3-deleting-node]
