#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

/*clase donde se creo el socket del cliente y se conecta al 
socket del servidor*/
class Client {

    public:

        int client_fd;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        char buffer[1024] = { 0 };

        Client(char* server_ip, int port) {
            client_fd = socket(AF_INET, SOCK_STREAM, 0);

            address.sin_addr.s_addr = inet_addr(server_ip);
            address.sin_family = AF_INET;
            address.sin_port = htons(port);

            connect(client_fd,(struct sockaddr*)&address,addrlen);
            cout << "conectado al Servidor!" << endl;
        }

        void enviar(char mensaje)
        {
            this->buffer[0]=mensaje;
            send(client_fd, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
        }
    
        string recibir()
        {
            recv(client_fd, buffer, sizeof(buffer), 0);
            string buf=buffer;
            memset(buffer, 0, sizeof(buffer));
            return buf;
        }
    
        void cerrarSocket()
        {
            close(client_fd);
            cout << "Socket cerrado." << endl << endl;
        }
};
/* Clase donde se generan los diferentes barcos de cliente*/
class Barco { 

    private:

        int b_tamaño;

    public:

        int b_orientacion; 
        vector<int> b_x; 
        vector<int> b_y; 
        vector<bool> b_impactos; 
        
        Barco(int tamaño, int orientacion, int x, int y) : 
            b_tamaño(tamaño),
            b_x(tamaño),
            b_y(tamaño),
            b_impactos(tamaño, false),
            b_orientacion(orientacion)
        {  
            for (int i = 0; i < b_tamaño; i++) { 
                b_x[i] = x;
                b_y[i] = y;
                if(orientacion == 0) x++;
                else y++;          
            }       
        }

        int tamaño() const { return b_tamaño; } 

        bool destruido() const { 
            for (bool impacto : b_impactos) {
                if (!impacto) return false;
            }
            return true;   
        }
};
/* clase donde se genera el tablero del cliente y los disparos que se ejecutan*/
class Tablero {

    private:

        char matriz[15][15]; 
        vector<Barco> t_barcos; 

    public: 

        Tablero() { 
            for (int i = 0; i < 15; i++) { 
                for (int j = 0; j < 15; j++) {
                        matriz[i][j] = ' ';
                }
            }
        }

        bool revisar_colocar(int tamaño, int orientacion, int fila, int columna){ 
            for (int i = 0 ; i < tamaño ; i++) {                          
                if(matriz[fila][columna] != ' ') return false;
                if(orientacion == 0) fila++; 
                else columna++;
            }
            return true;
        }

        void colocar(int tamaño, int orientacion, int fila, int columna) { 
            Barco barco(tamaño, orientacion, fila, columna); 
            t_barcos.push_back(barco); 
            int x = barco.b_x[0]; 
            int y = barco.b_y[0];
            for (int i = 0 ; i < barco.tamaño(); i++) { 
                if (barco.tamaño() == 5) matriz[x][y] = 'P';
                if (barco.tamaño() == 4) matriz[x][y] = 'B';
                if (barco.tamaño() == 3) matriz[x][y] = 'S';
                if (barco.tamaño() == 1) matriz[x][y] = 'L';
                if (barco.b_orientacion == 1) y++;
                else x++;
            } 
        }

        void mostrar() {
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

        char disparar(int fila, int columna){
            if (matriz[fila][columna] == ' ') { 
                matriz[fila][columna] = 'X';
                return 'X';
            } else {
                matriz[fila][columna] = 'O';
                for (Barco& barco : t_barcos) { 
                    for (int i = 0; i < barco.tamaño() ; i++) {          
                        if (fila == barco.b_x[i] && columna == barco.b_y[i]) barco.b_impactos[i] = true; 
                    }
                }
                return 'O';
            }
        }

        bool revisar_disparo(int fila, int columna){
            if(matriz[fila][columna] == ' ') return true;
            cout << "Ya has disparado en esta posicion. Intenta nuevamente" << endl;
            return false;
        }

        void guardar(int fila, int columna, char res){
            matriz[fila][columna] = res;
        }

        bool completado() const { 
            for (const Barco barco : t_barcos) {      
                if (!barco.destruido()) return false;             
            }
            return true;
        }
};

int main(int argc, char *argv[]) {

    srand(time(NULL));
    int port = stoi(argv[2]);   
    int barcos[8] = {5,4,4,3,3,1,1,1}; 
    int mi_fila = 0;
    int mi_columna = 0;
    char charmi_fila;
    char charmi_columna;  
    int vs_fila;
    int vs_columna;
    char charvs_fila;
    char charvs_columna;
    char res;
    Client *Cliente = new Client(argv[1],port);
    Tablero Mitablero;
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
            if (Mitablero.revisar_colocar(barcos[i], ori, mi_fila, mi_columna)) { 
                Mitablero.colocar(barcos[i], ori, mi_fila, mi_columna);              
                break;
            } 
        }
    }
    /*se muestra el tablero propio y del enemigo y se realizan 
    los disparos*/
    Tablero Vstablero;
    while(!Mitablero.completado()){
        cout << "Tu tablero:" << endl; 
        Mitablero.mostrar();
        cout << "***************************************************************" << endl;
        cout << "Tablero Enemigo:" << endl; 
        Vstablero.mostrar();     
        cout << endl << "***************************************************************" << endl;
        do{
            mi_fila = 0; 
            mi_columna = 0; 
            charmi_fila = ' ';
            charmi_columna = ' ';  
            while(charmi_fila < '0' || charmi_fila > '?' ){
                cout << "Ingrese la posicion a disparar (fila 1-15): ";
                cin >> mi_fila;
                charmi_fila = (char)mi_fila+47;
                if(charmi_fila < '0' || charmi_fila > '?' ) cout << "El numero ingresado es invalido, intente nuevamente." << endl;
            }
            while(charmi_columna < 'a' || charmi_columna > 'o'){
                cout << "Ingrese la posicion a disparar (columna a-o): ";
                cin >> charmi_columna;
                mi_columna = (int)charmi_columna-97;
                if(charmi_columna < 'a' || charmi_columna > 'o') cout << "El valor ingresado es invalido, intente nuevamente." << endl;          
            }
        }while(!Vstablero.revisar_disparo(mi_fila-1,mi_columna));
        cout << "***************************************************************" << endl;
        Cliente->enviar(charmi_fila);
        Cliente->enviar(charmi_columna);
        cout << "Disparaste en la posicion (" << mi_fila << "," << charmi_columna << ")" << endl;
        res = Cliente->recibir()[0];
        if(res == 'O') cout << "Has acertado en un barco!!" << endl;
        else cout << "No has acertado en un barco!!" << endl;
        cout << "***************************************************************" << endl;
        Vstablero.guardar(mi_fila-1, mi_columna, res);

        charvs_fila = Cliente->recibir()[0];
        vs_fila = (int)charvs_fila - 48;
        charvs_columna = Cliente->recibir()[0];
        vs_columna = (int)charvs_columna - 97;
        res = Mitablero.disparar(vs_fila, vs_columna);
        Cliente->enviar(res);
        cout << "Te han acatado en la posicion (" << vs_fila+1 << "," << charvs_columna << ")" << endl;
        cout << "***************************************************************" << endl;
        if(Cliente->recibir()[0] == 'T'){
            cout << "Has Ganado el juego!!" << endl;
            break;
        }
        if(Mitablero.completado()) {
            cout << "Has perdido el juego. Suerte la proxima" << endl;
            Cliente->enviar('T');
        } else Cliente->enviar('Z');
    }

    Cliente->cerrarSocket();
    return 0;
}
