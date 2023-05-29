# Tarea-redes-castillo-villalobos

Juego Batalla Naval
Programa Servidor

El programa servidor recibe por consola al ejecutarse el puerto al cual se conectara el socket que se inicia en el main().
Al crear el socket, espera que se conecten clientes. Al conectarse un cliente se genera un hilo para llevar el juego con ese cliente,
se crea un tablero para el servidor (con barcos) y uno para el cliente (sin barcos, para guardar disparo realizados).
El cliente realizará el primer disparo y servidor devolvera un disparo, por cada disparo se muestra por patalla el servidor.
    
Si se completo el tablero del Servidor (significa que Cliente gana), se envia un mensaje al cliente. Si un no esta completo se 
envía constantemente despues de cada disparo un mensaje para que continue el juego.

Desde el cliente constantemente se recibe un mensaje tambien para decir que no esta completo su tablero. Cuando si está completo este mensaje cambiará
Si se recibe que el tablero del Cliente esta completo (significa que el Servidor ganó), se termina el juego y ese hilo.

1- Compilar programa

        g++ Server.cpp -o Server

2- Iniciar programa 

        ./Server <Puerto>

3- Finalizar programa

        Se debe hacer de manero default: ctrl + C 