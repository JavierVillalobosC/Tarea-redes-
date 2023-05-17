#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>

/* JUEGO BATALLA NAVAL UN SOLO JUGADOR
En esta versión existe un único tablero con barcos ubicados aleatoriamente,
se pide por consola que el jugador vaya ingresando posiciones para disparar,
se muestra el tablero cada vez que el usuario dispare.
El juego termina cuando el usuario destruya todos los barcos. Se mostrará el 
número de intentos.

Errores a corregir:
- Al pedir un numero (fila) para disparar, si se ingresa un caracter distinto a un numero,
el programa se cae.

Clases:
-Barco
-Tablero
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
        
        Barco(int tamaño, int orientacion, int x, int y) : //Constructor
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

        Tablero() { //Constructor
            for (int i = 0; i < 15; i++) { //Se inicia la matriz con el caracter ' '.
                for (int j = 0; j < 15; j++) {
                        matriz[i][j] = ' ';
                }
            }
        }

        bool revisar(int tamaño, int orientacion, int fila, int columna){ //Función para revisar si en las posiciones que se quiere ubicar un nuevo barco, no haya sido uno ya ubicado.
            for (int i = 0 ; i < tamaño ; i++) {                          //Devuelve false si encuentra una posición ocupada y true si es posible ubicar el nuevo barco.
                if(matriz[fila][columna] != ' ') return false;
                if(orientacion == 0) fila++; 
                else columna++;
            }
            return true;
        }

        void colocar(int tamaño, int orientacion, int fila, int columna) { //Función que crea un nuevo objeto barco, lo agrega al vector y lo agrega a la matriz.
            Barco barco(tamaño, orientacion, fila, columna); //Crea nuevo objeto barco.
            t_barcos.push_back(barco); //Agrega a vector.
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
                        if(matriz[i-1][j] == ' ' || matriz[i-1][j] == 'O' || matriz[i-1][j] == 'X') { //Se oculta la posición si esta contiene un barco, mostrando el caracter ' '.
                            cout << matriz[i-1][j] << " | ";
                        } else {
                            cout << " " << " | ";
                        }
                    }
                }
                cout << endl;
            }
        }

        void disparar(int fila, int columna) { //Función para revisar y/o cambiar una posición en la matriz cuando el usuario ingrese un disparo.
            if (matriz[fila][columna] == 'X' || matriz[fila][columna] == 'O') { //'X' significa que se disparó en esa posición pero no impactó en un barco. 
                cout << "Ya has disparado en esta posicion" << endl;            //'O' significa que se disparó en esa posición e impactó en un barco. 
                cout << "***************************************************************" << endl;
            } else if (matriz[fila][columna] == ' ') { 
                cout << "No has acertado" << endl << "***************************************************************" << endl;
                matriz[fila][columna] = 'X';
            } else {
                cout << "HAS ACERTADO EN UN BARCO ENEMIGO!!" << endl << "***************************************************************" << endl;
                matriz[fila][columna] = 'O';
                for (Barco& barco : t_barcos) { //for para buscar en que barco se impactó.
                    for (int i = 0; i < barco.tamaño() ; i++) {          
                        if (fila == barco.b_x[i] && columna == barco.b_y[i]) barco.b_impactos[i] = true; //Se cambia la posición del barco impactada a true.
                    }
                }
            }    
        }

        bool completado() const { //Función que revisa si los barcos del tablero han sido todos destruidos.
            for (const Barco barco : t_barcos) {      //Llama a la función destruido() de cada barco, si todos devuelven true, significa que todos han sido destruidos 
                if (!barco.destruido()) return false; //y está completo el tablero.                 
            }
            return true;
        }
};

int main() {

    srand(time(NULL));   
    int barcos[8] = {5,4,4,3,3,1,1,1}; //Vector con todos los tamaños de los barcos solicitados a ingresar.
    int t_fila = 0;
    int t_columna = 0;
    int intentos = 0; //Variable que aumenta cada vez que se ingrese un intento de disparo.
    Tablero tablero1;
    for(int i = 0; i < 8 ; i++) {
        while (true) { //Se crearán posiciones hasta que una sea correcta.
            int ori = rand() % 2; //orientación: 0 vertical - 1 horizontal.
            if (ori == 0) {
                t_fila = rand() % (15-(barcos[i]-1)); //posición random entre 0 y 15-tamañoBarco para no sobrepasar el limite de la matriz.
                t_columna = rand() % 15;
            } else {
                t_fila = rand() % (15);
                t_columna = rand() % (15-(barcos[i]-1));
            }
            if (tablero1.revisar(barcos[i], ori, t_fila, t_columna)) { //Revisa si es posible ingresar el nuevo barco, de ser asi, lo ingresa y corta el while.
                tablero1.colocar(barcos[i], ori, t_fila, t_columna);              
                break;
            } 
        }
    }
    tablero1.mostrar();
    while (!tablero1.completado()) { //Ciclo que no termina hasta que el tablero este completo.
        intentos++;
        int fila = 0; //Variables para pedir valores.
        char columna = ' ';
        int columna_int = 0; 
        cout << endl << "***************************************************************" << endl;
        while(fila < 1 || fila > 15){
            cout << "Ingrese la posicion a disparar (fila 1-15): ";
            cin >> fila;
            if(fila < 1 || fila > 15) cout << "El numero ingresado es invalido, intente nuevamente." << endl;
        }
        while(columna < 'a' || columna > 'o'){
            cout << "Ingrese la posicion a disparar (columna a-o): ";
            cin >> columna;
            if(columna < 'a' || columna > 'o') cout << "El numero ingresado es invalido, intente nuevamente." << endl;          
        }
        cout << "***************************************************************" << endl;
        columna_int = static_cast<int>(columna); //Toma el valor ascii del caracter.
        cout << "Disparando en la posicion (" << fila << "," << columna << ")" << endl;
        tablero1.disparar(fila-1, columna_int-97);// se envian valores entre 0-14 para buscar en la matriz del tablero.
        tablero1.mostrar();
    }

    cout << endl << "***************************************************************" << endl << "JUEGO TERMINADO!!, usted hizo " << intentos << " intentos." << endl << "***************************************************************";

    return 0;
}