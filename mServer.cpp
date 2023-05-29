#include <netinet/in.h> //incluye funciones para trabajar con direcciones ip
#include <sys/socket.h> //incluye funciones para trabajar con sockets
#include <arpa/inet.h> //incluye funciones para trabajar con direcciones ip
#include <vector> //incluye funciones para trabajar con vectores
#include <cstdlib>  //incluye funciones para la gestion de memoria
#include <ctime> //incluye funciones para la manipulacion y representacion de tiempo
#include <iostream> //incluye funciones de entrada y salida en la consola
#include <cstring> //incluye funciones para la manipulacion de cadena de caracteres
#include <thread> //incluye funciones para la creacion y manipulacion de hilos
#include <unistd.h> //incluye funciones relacionadas con la interfaz del S.I.

/* Javier Villalobos - Miguel Castillo
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
*/

using namespace std;

class Barco { 

    private:

        int b_tamaño; //guarda el tamaño del barco.

    public:

        int b_orientacion; //Guarda la orientación del barco.
        vector<int> b_x; //Vector del tamaño del barco que guarda la posición de cada parte del barco (fila).
        vector<int> b_y; //Vector del tamaño del barco que guarda la posición de cada parte del barco (columna).
        vector<bool> b_impactos; //Vector del tamaño del barco que guarda si ha sido impactado. Se define todas las partes en false y una vez que es impactado cambia a true. 
                                 //Se usará para saber si el barco ha sido destruido (cuando todas las partes sean true). 
        
        Barco(int tamaño, int orientacion, int x, int y) :  //Constructor 
            b_tamaño(tamaño),
            b_x(tamaño),
            b_y(tamaño),
            b_impactos(tamaño, false),
            b_orientacion(orientacion)
        {  
            for (int i = 0; i < b_tamaño; i++) { //for para llenar los vectores, recibiendo solo la primera posición del barco y la orientación para saber qué variable aumentar.
                b_x[i] = x;
                b_y[i] = y;
                if(orientacion == 0) x++;
                else y++;          
            }       
        }

        int tamaño() const { return b_tamaño; } //Devuelve el tamaño del barco.

        bool destruido() const { //Función que revisa si un barco ha sido destruido, si todas las partes han cambiado a true.
            for (bool impacto : b_impactos) {
                if (!impacto) return false;
            }
            return true;   
        }
};

class Tablero {

    private:

        char matriz[15][15]; //Declara una matriz de 15 por lado.
        vector<Barco> t_barcos; //Vector que almacena cada barco que se inicie en el tablero.

    public: 

        Tablero() { 
            for (int i = 0; i < 15; i++) { //Se inicia la matriz con el caracter ' '.
                for (int j = 0; j < 15; j++) {
                        matriz[i][j] = ' ';
                }
            }
        }

        bool revisar_colocar(int tamaño, int orientacion, int fila, int columna){  //Función para revisar si en las posiciones que se quiere ubicar un nuevo barco, no haya sido uno ya ubicado.
            for (int i = 0 ; i < tamaño ; i++) {                                   //Devuelve false si encuentra una posición ocupada y true si es posible ubicar el nuevo barco.
                if(matriz[fila][columna] != ' ') return false;
                if(orientacion == 0) fila++; 
                else columna++;
            }
            return true;
        }

        void colocar(int tamaño, int orientacion, int fila, int columna) { //Función que crea un nuevo objeto barco, lo agrega al vector y lo agrega a la matriz.
            Barco barco(tamaño, orientacion, fila, columna); //Crea nuevo objeto barco.
            t_barcos.push_back(barco); //Agrega el barco al vector.
            int x = barco.b_x[0]; //Se toma la primera posición del barco.
            int y = barco.b_y[0];
            for (int i = 0 ; i < barco.tamaño(); i++) { //for para cambiar la posición de la matriz al nuevo barco.
                if (barco.tamaño() == 5) matriz[x][y] = 'P';
                if (barco.tamaño() == 4) matriz[x][y] = 'B';
                if (barco.tamaño() == 3) matriz[x][y] = 'S';
                if (barco.tamaño() == 1) matriz[x][y] = 'L';
                if (barco.b_orientacion == 1) y++;
                else x++;
            } 
        }

        void mostrar() { //Función para mostrar por pantalla la matriz, se muestra para las filas, numeros del 1-15 y para las columnas letras de la A-0.
            char letra = 'A';
            for (int i = 0; i < 16; i++) {
                if (i == 0) {
                    cout << "    ";
                } else if (i > 0 && i < 10) {
                    cout << i << " | ";
                } else {
                    cout << i << "| ";
                }
                for (int j = 0; j < 15; j++) {
                    if(i == 0) {
                        cout << letra << "   ";
                        letra++;
                    } else {
                        cout << matriz[i-1][j] << " | ";
                    }
                }
                cout << endl;
            }
        }

        char disparar(int fila, int columna){ //Función para revisar y/o cambiar una posición en la matriz cuando el usuario ingrese un disparo.
            if (matriz[fila][columna] == ' ') { //'X' significa que se disparó en esa posición pero no impactó en un barco. 
                matriz[fila][columna] = 'X'; //'O' significa que se disparó en esa posición e impactó en un barco. 
                return 'X';
            } else {
                matriz[fila][columna] = 'O';
                for (Barco& barco : t_barcos) { 
                    for (int i = 0; i < barco.tamaño() ; i++) {      //for para buscar en que barco se impactó.    
                        if (fila == barco.b_x[i] && columna == barco.b_y[i]) barco.b_impactos[i] = true; //Se cambia la posición del barco impactada a true.
                    }
                }
                return 'O';
            }
        }

        bool revisar_disparo(int fila, int columna){ //Función que revisa si ya se disparo anteriormente en la posicion que se desea hacer un nuevo disparo.
            if(matriz[fila][columna] == ' ') return true;
            return false;
        }

        void guardar(int fila, int columna, char res){ //Función que guarda en el tablero enemigo el resultado del disparo que se realizo.
            matriz[fila][columna] = res;
        }

        bool completado() const { //Función que revisa si los barcos del tablero han sido todos destruidos.
            for (const Barco barco : t_barcos) {      //Llama a la función destruido() de cada barco, si todos devuelven true, significa que todos han sido destruidos 
                if (!barco.destruido()) return false; //y está completo el tablero.           
            }
            return true;
        }
};

class Server { //Clase para Servidor

    public:

        int server_fd, new_socket; 
        struct sockaddr_in address; //Variable que se usará para almacenar la información de dirección de socket.
        int addrlen = sizeof(address); //Almacena el tamaño de la dirección.
        char buffer[1024] = { 0 }; //buffer para almacenar los mensajes enviados y recibidos
        int conexion = 0; //cada vez que se cree una conexion aumentara en 1, para asignar al cliente un numero

        Server(int PORT) {
            server_fd = socket(AF_INET, SOCK_STREAM, 0); //Se crea un nuevo socket
            
            address.sin_family = AF_INET; //Configura address para que sea compatible con IPv4 y pueda usar funciones de socket.
            address.sin_addr.s_addr = INADDR_ANY; //configura el socket para enlazarlo a las interfaces de red disponibles en el sistema.
            address.sin_port = htons(PORT); //Se especifica el numero de puerto

            cout << "Iniciando Servidor en puerto: " << PORT << endl; 

            bind(server_fd, (struct sockaddr*)&address, sizeof(address)); //Enlaza socket con dirección IP y puerto      
            listen(server_fd, 0); //Escucha por conexiones
            
            cout << "Escuchando para conexiones entrantes..." << endl;         

            while(true){ 
                sockaddr_in address;
                addrlen = sizeof(address);
                if((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen)) != -1) { //Acepta una nuevo conexión.
                    this->conexion++;
                    thread clientThread(&Server::handleClient,this,new_socket,conexion); //crea un nuevo hilo y lo inicializa 
                    clientThread.detach();
                } else {
                    cout << "No se ha podido establecer conexion." << endl;
                }
                
            }
        }

        string recibir(int socket) { //Función para recibir mensajes del cliente.
            recv(socket, buffer, sizeof(buffer), 0);
            string buf=buffer;
            memset(buffer, 0, sizeof(buffer));
            return buf;
        }

        void enviar(int socket,char mensaje) { //Función para enviar mensajes del cliente.
            this->buffer[0]=mensaje;
            send(socket, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
        }

        void cerrarSocket() { //Función para cerrar el socket
            close(server_fd);
            cout << "Socket cerrado, cliente desconectado." << endl;
        }

    private:

        void handleClient(int new_socket,int conexion){ //Función que maneja la conexion del servidor con cliente (recibe el socket creado y el nuevo de conexion del cliente)

            cout << "Nuevo cliente ha establecido conexion. Numero cliente: " << conexion << endl;

            int barcos[8] = {5,4,4,3,3,1,1,1}; //Vector con todos los tamaños de los barcos solicitados a ingresar.
            int mi_fila = 0; //variables para almacenar las posiciones que tanto el servidor y el cliente desean disparar
            int mi_columna = 0; //para cada posisión (fila y columna) se crea una variable "int" la cual se usa para trabajar con la matriz tablero y las variables "char" 
            int vs_fila;        //son para enviar las posiciones como mensaje al cliente o las que este envió.
            int vs_columna;
            char charvs_fila;
            char charvs_columna;
            char charmi_fila;
            char charmi_columna;
            char res; //variable para saber si el tablero del cliente esta completo (se recibe desde el cliente).
            
            Tablero Mitablero; //crea un tablero para el servidor e inicia las posiciones de los barcos aleatoriamente.
            for(int i = 0; i < 8 ; i++) {
                while (true) { 
                    int ori = rand() % 2; 
                    if (ori == 0) {
                        mi_fila = rand() % (15-(barcos[i]-1)); 
                        mi_columna = rand() % 15;
                    } else {
                        mi_fila = rand() % (15);
                        mi_columna = rand() % (15-(barcos[i]-1));
                    }
                    if (Mitablero.revisar_colocar(barcos[i], ori, mi_fila, mi_columna)) { //Revisa si es posible ingresar el nuevo barco, de ser asi, lo ingresa y corta el while. 
                        Mitablero.colocar(barcos[i], ori, mi_fila, mi_columna);              
                        break;
                    } 
                }
            }
            Tablero Vstablero; //Crea un tablero para que el servidor almacene donde disparó y asi no dispara en la misma posicion nuevamente
            cout << "Tablero Servidor (Cliente " << conexion << "):" << endl <<"***************************************************************" << endl;
            Mitablero.mostrar();
            cout << "Tablero Cliente n°: " << conexion << endl << "***************************************************************" << endl;
            Vstablero.mostrar();
            cout << "***************************************************************" << endl;
            while(!Mitablero.completado()){ //Ciclo que no termina hasta que el tablero este completo.
                charvs_fila = this->recibir(new_socket)[0]; //recibe posición
                vs_fila = (int)charvs_fila - 48; //transforma posición en "int".
                charvs_columna = this->recibir(new_socket)[0];
                vs_columna = (int)charvs_columna - 97;
                res = Mitablero.disparar(vs_fila, vs_columna); //genera disparo y recibe si este acertó o no.
                this->enviar(new_socket,res); //envía resultado
                cout << "El cliente n°: " << conexion <<", disparo en la posicion (" << vs_fila+1 << "," << charvs_columna << ")" << endl;

                while(true){
                    mi_fila = rand() % 15;  //genere posiciónm al azar para disparar
                    mi_columna = rand() % 15;         
                    if(Vstablero.revisar_disparo(mi_fila,mi_columna)){ //revisa si la posición esta libre
                        charmi_fila = (char)mi_fila + 48; //tranforma posición en "char"
                        this->enviar(new_socket,charmi_fila); //envía posición
                        charmi_columna = (char)mi_columna + 97;
                        this->enviar(new_socket, charmi_columna);
                        cout << "El Servidor disparo en la posicion (" << mi_fila-1 << "," << charmi_columna << ") al cliente n°: " << conexion << endl;
                        break;
                    }
                } 
                res = this->recibir(new_socket)[0]; //recibe resultado del disparo
                Vstablero.guardar(mi_fila, mi_columna, res); //guarda el resultado
                cout << "Tablero Servidor:" << endl <<"***************************************************************" << endl;
                Mitablero.mostrar();
                cout << "Tablero Cliente n°: " << conexion << endl << "***************************************************************" << endl;
                Vstablero.mostrar();
                cout << "***************************************************************" << endl;
                if(Mitablero.completado()) { //Si el tablero del Servidor esta completo, significa que el cliente ha ganado
                    cout << "El Cliente n°: " << conexion << ", ha ganado" << endl;
                    this->enviar(new_socket,'T'); //envia "T" como aviso que el tablero esta completo
                }
                else this->enviar(new_socket,'Z'); //sino esta completo envía una "Z" y continua el juego.
                if(this->recibir(new_socket)[0] == 'T') { //Si se recibe una "T" significa que se completo el tablero del cliente, por lo tanto el servidor gana
                    cout << "El Cliente n°: " << conexion << ", ha perdido" << endl;
                    break;
                } //Si no, se recibe "Z" lo que significa que sigue el juego
            }
        }

};

int main(int argc, char* argv[]) {
    
    int PORT = stoi(argv[1]); //toma el argumento por consola para designar el puerto ingresado
    srand(time(NULL));  //inicia el generador de números aleatorios en C++.
    Server *Servidor = new Server(PORT); //Genera un nuevo Objeto servidor
    Servidor->cerrarSocket(); //Se cierra el servidor

    return 0;
}