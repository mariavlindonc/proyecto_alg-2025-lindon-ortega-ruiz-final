#ifndef PROYECT_LIBRARY_
#define PROYECT_LIBRARY_

#include <iostream>
#include <sstream> // Permite trabajar con las lineas de csv como strings (stringstream, getline)
#include <fstream> // Permite leer y escribir archivo externos (ifstream)
#include <iomanip>
#include <math.h>
#include <ctime>
#include "Lista\Lista.h"
#include "Pila\Pila.h"
#include "HashMap\HashMap.h" // CAMBIAR POR HASHMAP LIST PARA USAR MEJOR MANEJO DE COLISIONES
#define NOMBRE_ARCHIVO ("C:/Users/mairi/source/proyecto-2025-comision-tosetti-lindon-ortega-ruiz/ventas_sudamerica.csv")
using namespace std;

struct Venta {
    string pais, ciudad, cliente, producto, categoria, medio_envio, estado_envio;
    tm fecha;
    int id, cantidad;
    float precio_unitario, monto_total;
};

// Estructura que relaciona cada ciudad con su monto total
struct ciudad_monto {
    string ciudad;
    float total;
};

struct producto_monto {
    string nombre;
    float total;
};

struct categoria {
    string nombre;
    float promedio;
    float total;
    int ventas;
};

struct medioenvio_cantidad {
    string nombre;
    int ventas;
};

struct estadoenvio_cantidad {
    string nombre;
    int ventas;
};

struct producto_cantidad {
    string producto;
    int cantidad;
};

struct producto_promedio {
    string producto;
    float total;
    int ventas;
    float promedio;
};

struct estadisticas_pais {
    // Lista<int> Ventas; --> Podría haber desarrollado las siguientes estructuras en base a esto
    Lista<ciudad_monto> ciudadesOrdenadasMonto;
    Lista<producto_monto> productosMontoTotal;
    Lista<categoria> categoriasPromedio;
    Lista<medioenvio_cantidad> mediosDeEnvio;
    Lista<estadoenvio_cantidad> estadosDeEnvio;
};

struct dia_montos {
    tm fecha;
    float total;
};

unsigned int hashFunc(unsigned int clave) {
    return clave;  // Retorna el valor del ID como el índice hash
}

unsigned int hashString(string clave) {
    unsigned int hash = 0;
    for (char c : clave) {
        hash = 131 * hash + c;
        hash ^= (hash >> 5); // 'bitwise shift' --> mueve los bits para mejorar la distribucion
    }
    return hash;
}

auto trim = [](string s) { //FUNCION BUSCADA EN INTERNET PARA ASEGURAR QUE LOS DATOS NO TENGAN ESPACIOS EXTRAS
    while (!s.empty() && isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && isspace(s.back())) s.pop_back();
    return s;
};

tm stringToDateTime(const string& fechaString) { //CONVIERTO A FECHA
    tm fecha = {};
    istringstream ss(fechaString);
    
    ss >> get_time(&fecha, "%d/%m/%Y %H:%M"); // Formato: DD/MM/YYYY HH:MM

    if (ss.fail()) {
        throw runtime_error("Error al convertir la cadena a fecha.");
    }

    return fecha;
}

bool validarFecha(const string& date) {
    tm fecha = {};
    istringstream ss(date);
    ss >> get_time(&fecha, "%d/%m/%Y %H:%M");
    if (ss.fail()) {
        return false;
    } else {
        return true;
    }
}

//Función carga de archivo csv
void loadFile (HashMap<unsigned int, Venta> &mapa, int &size) {
    
    ifstream archivo(NOMBRE_ARCHIVO); // Abrir el archivo

    string linea;
    char delimitador = ',';

    getline(archivo, linea); // Descartar primera linea

    while (getline(archivo, linea)) // Leer todas las líneas
    { 
        Venta v;

        stringstream stream(linea); // Convertir la cadena a un stream
        string id_venta, fecha, cantidad, precio_unitario, monto_total;
        // Extraer todos los valores de esa fila
        getline(stream, id_venta, delimitador);
        getline(stream, fecha, delimitador);
        getline(stream, v.pais, delimitador);
        getline(stream, v.ciudad, delimitador);
        getline(stream, v.cliente, delimitador);
        getline(stream, v.producto, delimitador);
        getline(stream, v.categoria, delimitador);
        getline(stream, cantidad, delimitador);
        getline(stream, precio_unitario, delimitador);
        getline(stream, monto_total, delimitador);
        getline(stream, v.medio_envio, delimitador);
        getline(stream, v.estado_envio, delimitador);

        // Ingresar los datos numericos a la struct
        v.id = stoi(id_venta);
        v.cantidad = stoi(cantidad);
        v.precio_unitario = stof(precio_unitario);
        v.monto_total = stof(monto_total);
        v.pais = trim(v.pais);
        v.ciudad = trim(v.ciudad);
        v.fecha = stringToDateTime(fecha);
        mapa.put(v.id, v);
        size++;
    }

    archivo.close();
}

Nodo<producto_promedio>* obtenerNodoPP(Lista<producto_promedio>& lista, int pos) { // Para trabajar con punteros, obtenemos el nodo
    if (pos < 0 || pos >= lista.getTamanio()) return nullptr;
    Nodo<producto_promedio>* aux = lista.getInicio(); // el auxiliar es el primer nodo de la lista
    int i = 0;
    while (aux != nullptr && i < pos) { // busco la posicion
        aux = aux->getSiguiente();
        i++;
    }
    return aux; // devuelve el nodo correspondiente a la posicion
}

int posListAvgPP(Lista<producto_promedio>& lista, int bot, int top) { //Obtengo la posición del promedio de la lista
    if (top - bot <= 0 || bot < 0 || top > lista.getTamanio()) return -1;

    Nodo<producto_promedio>* nodo = obtenerNodoPP(lista, bot);
    if (!nodo) return -1;

    float suma = 0;
    for (int i = bot; i <= top; i++) {
        if (!nodo) return -1;
        suma += nodo->getDato().promedio;
        nodo = nodo->getSiguiente();
    }
    float promedio_lista = suma/(top - bot);

    nodo = obtenerNodoPP(lista,bot);
    int pos = bot;
    float dif_min = abs(nodo->getDato().promedio - promedio_lista), dif_actual;
    for (int i = bot; i <= top && nodo; ++i) {
        float dif = fabs(nodo->getDato().promedio - promedio_lista);
        if (dif < dif_min) {
            dif_min = dif;
            pos = i;
        }
        nodo = nodo->getSiguiente();
    }
    return pos;
}

// funcion de particion para simplificar el quicksort, divide en mayores y menores que el pivote 
int partitionPP(Lista<producto_promedio>& lista, int bot, int top) { //
    if (bot > top) return bot;

    int pivotIndex = posListAvgPP(lista, bot, top);
    if (pivotIndex == -1) return bot;

    Nodo<producto_promedio>* pivotNodo = obtenerNodoPP(lista, pivotIndex); // el pivot es el promedio de la seccion
    Nodo<producto_promedio>* pivotMedio = obtenerNodoPP(lista, (bot + top)/2); //Obtengo el nodo del medio
    if (!pivotNodo || !pivotMedio) return bot;
    
    float pivot = pivotNodo->getDato().promedio; //obtengo el promedio del pivot
    
    //Intercambio la posicion de esos dos pivotes
    producto_promedio temp = pivotNodo->getDato();
    pivotNodo->setDato(pivotMedio->getDato());
    pivotMedio->setDato(temp);
    
    int i = bot, j = top;

    while (i <= j) {
        Nodo<producto_promedio>* nodoI = obtenerNodoPP(lista, i);
        while (nodoI && nodoI->getDato().promedio < pivot) {
            i++;
            nodoI = obtenerNodoPP(lista, i);
        }

        Nodo<producto_promedio>* nodoJ = obtenerNodoPP(lista, j);
        while (nodoJ && nodoJ->getDato().promedio > pivot) {
            j--;
            nodoJ = obtenerNodoPP(lista, j);
        }

        if (i <= j && nodoI && nodoJ) {
            producto_promedio temp = nodoI->getDato();
            nodoI->setDato(nodoJ->getDato());
            nodoJ->setDato(temp);
            i++;
            j--;
        }
    }
    return j;
}

// Implementación de Quicksort para Lista Enlazada
void quicksortListaPP(Lista<producto_promedio>& lista, int bot, int top) {
    if (bot < top) {
        int pi = partitionPP(lista, bot, top);
        quicksortListaPP(lista, bot, pi);
        quicksortListaPP(lista, pi + 1, top);
    }
}

Nodo<ciudad_monto>* obtenerNodoCM(Lista<ciudad_monto>& lista, int pos) { // Para trabajar con punteros, obtenemos el nodo
    if (pos < 0 || pos >= lista.getTamanio()) return nullptr;
    Nodo<ciudad_monto>* aux = lista.getInicio(); // el auxiliar es el primer nodo de la lista
    int i = 0;
    while (aux != nullptr && i < pos) { // busco la posicion
        aux = aux->getSiguiente();
        i++;
    }
    return aux; // devuelve el nodo correspondiente a la posicion
}

int posListAvgCM(Lista<ciudad_monto>& lista, int bot, int top) { //Obtengo la posición del promedio de la lista
    if (top - bot <= 0 || bot < 0 || top > lista.getTamanio()) return -1;

    Nodo<ciudad_monto>* nodo = obtenerNodoCM(lista, bot);
    if (!nodo) return -1;

    float suma = 0;
    for (int i = bot; i <= top; i++) {
        if (!nodo) return -1;
        suma += nodo->getDato().total;
        nodo = nodo->getSiguiente();
    }
    float promedio = suma/(top - bot);

    nodo = obtenerNodoCM(lista,bot);
    int pos = bot;
    float dif_min = abs(nodo->getDato().total - promedio), dif_actual;
    for (int i = bot; i <= top && nodo; ++i) {
        float dif = fabs(nodo->getDato().total - promedio);
        if (dif < dif_min) {
            dif_min = dif;
            pos = i;
        }
        nodo = nodo->getSiguiente();
    }
    return pos;
}

// funcion de particion para simplificar el quicksort, divide en mayores y menores que el pivote 
int partitionCM(Lista<ciudad_monto>& lista, int bot, int top) { //
    if (bot > top) return bot;

    int pivotIndex = posListAvgCM(lista, bot, top);
    if (pivotIndex == -1) return bot;

    Nodo<ciudad_monto>* pivotNodo = obtenerNodoCM(lista, pivotIndex); // el pivot es el promedio de la seccion
    Nodo<ciudad_monto>* pivotMedio = obtenerNodoCM(lista, (bot + top)/2); //Obtengo el nodo del medio
    if (!pivotNodo || !pivotMedio) return bot;
    
    float pivot = pivotNodo->getDato().total; //obtengo el total del pivot
    
    //Intercambio la posicion de esos dos pivotes
    ciudad_monto temp = pivotNodo->getDato();
    pivotNodo->setDato(pivotMedio->getDato());
    pivotMedio->setDato(temp);
    
    int i = bot, j = top;

    while (i <= j) {
        Nodo<ciudad_monto>* nodoI = obtenerNodoCM(lista, i);
        while (nodoI && nodoI->getDato().total < pivot) {
            i++;
            nodoI = obtenerNodoCM(lista, i);
        }

        Nodo<ciudad_monto>* nodoJ = obtenerNodoCM(lista, j);
        while (nodoJ && nodoJ->getDato().total > pivot) {
            j--;
            nodoJ = obtenerNodoCM(lista, j);
        }

        if (i <= j && nodoI && nodoJ) {
            ciudad_monto temp = nodoI->getDato();
            nodoI->setDato(nodoJ->getDato());
            nodoJ->setDato(temp);
            i++;
            j--;
        }
    }
    return j;
}

// Implementación de Quicksort para Lista Enlazada
void quicksortListaCM(Lista<ciudad_monto>& lista, int bot, int top) {
    if (bot < top) {
        int pi = partitionCM(lista, bot, top);
        quicksortListaCM(lista, bot, pi);
        quicksortListaCM(lista, pi + 1, top);
    }
}

Nodo<producto_cantidad>* obtenerNodoPC(Lista<producto_cantidad>& lista, int pos) { // Para trabajar con punteros, obtenemos el nodo
    if (pos < 0 || pos >= lista.getTamanio()) return nullptr;
    Nodo<producto_cantidad>* aux = lista.getInicio(); // el auxiliar es el primer nodo de la lista
    int i = 0;
    while (aux != nullptr && i < pos) { // busco la posicion
        aux = aux->getSiguiente();
        i++;
    }
    return aux; // devuelve el nodo correspondiente a la posicion
}

int posListAvgPC(Lista<producto_cantidad>& lista, int bot, int top) { //Obtengo la posición del promedio de la lista
    if (top - bot <= 0 || bot < 0 || top > lista.getTamanio()) return -1;

    Nodo<producto_cantidad>* nodo = obtenerNodoPC(lista, bot);
    if (!nodo) return -1;

    float suma = 0;
    for (int i = bot; i <= top; i++) {
        if (!nodo) return -1;
        suma += nodo->getDato().cantidad;
        nodo = nodo->getSiguiente();
    }
    float promedio = suma/(top - bot);

    nodo = obtenerNodoPC(lista,bot);
    int pos = bot;
    float dif_min = abs(nodo->getDato().cantidad - promedio), dif_actual;
    for (int i = bot; i <= top && nodo; ++i) {
        float dif = fabs(nodo->getDato().cantidad - promedio);
        if (dif < dif_min) {
            dif_min = dif;
            pos = i;
        }
        nodo = nodo->getSiguiente();
    }
    return pos;
}

// funcion de particion para simplificar el quicksort, divide en mayores y menores que el pivote 
int partitionPC(Lista<producto_cantidad>& lista, int bot, int top) { //
    if (bot > top) return bot;

    int pivotIndex = posListAvgPC(lista, bot, top);
    if (pivotIndex == -1) return bot;

    Nodo<producto_cantidad>* pivotNodo = obtenerNodoPC(lista, pivotIndex); // el pivot es el promedio de la seccion
    Nodo<producto_cantidad>* pivotMedio = obtenerNodoPC(lista, (bot + top)/2); //Obtengo el nodo del medio
    if (!pivotNodo || !pivotMedio) return bot;
    
    int pivot = pivotNodo->getDato().cantidad; //obtengo la cantidad del pivot
    
    //Intercambio la posicion de esos dos pivotes
    producto_cantidad temp = pivotNodo->getDato();
    pivotNodo->setDato(pivotMedio->getDato());
    pivotMedio->setDato(temp);
    
    int i = bot, j = top;

    while (i <= j) {
        Nodo<producto_cantidad>* nodoI = obtenerNodoPC(lista, i);
        while (nodoI && nodoI->getDato().cantidad < pivot) {
            i++;
            nodoI = obtenerNodoPC(lista, i);
        }

        Nodo<producto_cantidad>* nodoJ = obtenerNodoPC(lista, j);
        while (nodoJ && nodoJ->getDato().cantidad > pivot) {
            j--;
            nodoJ = obtenerNodoPC(lista, j);
        }

        if (i <= j && nodoI && nodoJ) {
            producto_cantidad temp = nodoI->getDato();
            nodoI->setDato(nodoJ->getDato());
            nodoJ->setDato(temp);
            i++;
            j--;
        }
    }
    return j;
}

void quicksortListaPC(Lista<producto_cantidad>& lista, int bot, int top) {
    if (bot < top) {
        int pi = partitionPC(lista, bot, top);
        quicksortListaPC(lista, bot, pi);
        quicksortListaPC(lista, pi + 1, top);
    }
}

void bubbleSortMediosDeEnvio (Lista<medioenvio_cantidad>& lista) {
    Nodo<medioenvio_cantidad>* inicio_m = lista.getInicio();

    if (inicio_m != nullptr) {
        bool swapped;
        do {
            swapped = false;
            Nodo<medioenvio_cantidad>* actual = inicio_m;
            Nodo<medioenvio_cantidad>* siguiente = inicio_m->getSiguiente();

            while (siguiente != nullptr) {
                if (actual->getDato().ventas < siguiente->getDato().ventas) {
                    // Intercambiamos los datos
                    medioenvio_cantidad temp = actual->getDato();
                    actual->setDato(siguiente->getDato());
                    siguiente->setDato(temp);
                    swapped = true;
                }
                actual = siguiente;
                siguiente = siguiente->getSiguiente();
            }
        } while (swapped);
    }
}

void bubbleSortEstadosDeEnvio (Lista<estadoenvio_cantidad>& lista) {
    Nodo<estadoenvio_cantidad>* inicio_e = lista.getInicio();

    if (inicio_e != nullptr) {
        bool swapped;
        do {
            swapped = false;
            Nodo<estadoenvio_cantidad>* actual = inicio_e;
            Nodo<estadoenvio_cantidad>* siguiente = inicio_e->getSiguiente();

            while (siguiente != nullptr) {
                if (actual->getDato().ventas < siguiente->getDato().ventas) {
                    // Intercambiamos los datos
                    estadoenvio_cantidad temp = actual->getDato();
                    actual->setDato(siguiente->getDato());
                    siguiente->setDato(temp);
                    swapped = true;
                }
                actual = siguiente;
                siguiente = siguiente->getSiguiente();
            }
        } while (swapped);
    }
}

void ordenarTop5CiudadesPorMontoSegunPais(HashMap<string, estadisticas_pais>& mapa, Lista<string>& claves_mapaPaises) {  
    
    for (int i = 0; i < claves_mapaPaises.getTamanio(); i++) {

        string clave = claves_mapaPaises.getDato(i);

        if (!mapa.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        estadisticas_pais estadisticas = mapa.get(clave);

        if (estadisticas.ciudadesOrdenadasMonto.getTamanio() > 1) {
            quicksortListaCM(estadisticas.ciudadesOrdenadasMonto, 0, estadisticas.ciudadesOrdenadasMonto.getTamanio() - 1);
        }

        mapa.put(clave, estadisticas);
    }
}

int busquedaBinaria(Lista<producto_promedio>& lista, float monto) {
    int pos_izq = 0, pos_der = lista.getTamanio() - 1;
    int pos_mas_cercano = pos_izq + (pos_der - pos_izq) / 2;
    float menor_diferencia = fabs(lista.getDato(pos_mas_cercano).promedio - monto);

    while (pos_izq <= pos_der) {
        int pos_medio = pos_izq + (pos_der - pos_izq) / 2;
        Nodo<producto_promedio>* medio = obtenerNodoPP(lista, pos_medio);

        if (!medio) {
            return pos_mas_cercano;
        }

        float promedio_medio = medio->getDato().promedio;
        float diferencia_actual = fabs(promedio_medio - monto);

        // Verificar si este es el promedio más cercano hasta ahora
        if (diferencia_actual < menor_diferencia) {
            menor_diferencia = diferencia_actual;
            pos_mas_cercano = pos_medio;
        }

        // Si encontramos el promedio exacto, devolvemos la posición
        if (promedio_medio == monto) {
            return pos_medio;
        } else if (promedio_medio < monto) {
            pos_izq = pos_medio + 1;
        } else {
            pos_der = pos_medio - 1;
        }
    }

    // Retorna la posición más cercana si no se encuentra el valor exacto
    return pos_mas_cercano;
}

void printTop5CiudadesPorMontoSegunPais(HashMap<string, estadisticas_pais> &mapaPaises, Lista<string> &claves) {
    
    cout << endl << "------------------------" << endl;
    cout << "Top 5 ciudades por país según monto: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        
        string clave = claves.getDato(i);

        cout << "--------------------" << endl;
        cout << "País: " << clave << endl;
        
        cout << "1" << endl;

        cout << "2" << endl;

        Lista<ciudad_monto> paisactual = mapaPaises.get(clave).ciudadesOrdenadasMonto;
        
        cout << "3" << endl;

        int j = min(4, (paisactual.getTamanio() - 1)); //posicion 4 o tamaño de la lista

        while (j >= 0) { // Imprime en orden descendente
            cout << fixed << setprecision(2);
            cout << endl;
            cout << "Ciudad: " << paisactual.getDato(j).ciudad << endl;
            cout << "Monto: " << paisactual.getDato(j).total << endl; 
            cout << endl;
            j--;
        }
    }
}

void printMontoTotalPorProductoSegunPais(HashMap<string, estadisticas_pais> &mapaPaises, Lista<string> &claves) {
    cout << endl << "------------------------" << endl;
    cout << "Monto total por producto por país: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        
        string clave = claves.getDato(i);

        cout << "--------------------" << endl;
        cout << "País: " << clave << endl;
        
        if (!mapaPaises.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        Lista<producto_monto> paisactual = mapaPaises.get(clave).productosMontoTotal;

        for (int j = 0; j < paisactual.getTamanio(); j++) { // Imprime todos los productos
            producto_monto producto = paisactual.getDato(j);
            cout << fixed << setprecision(2);
            cout << endl;
            cout << "Producto: " << producto.nombre << endl;
            cout << "Monto Total: " << producto.total << endl; 
            cout << endl;
        }
    }
}

void calcularPromedioVentasPorCategoriaSegunPais(HashMap<string, estadisticas_pais>&mapa, Lista<string> &clavesPaises) {
    for (int i = 0; i < clavesPaises.getTamanio(); i++) {

        string clave = clavesPaises.getDato(i);

        if (!mapa.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        estadisticas_pais estadisticas = mapa.get(clave);

        for (int j = 0; j < estadisticas.categoriasPromedio.getTamanio(); j++) {
            categoria c = estadisticas.categoriasPromedio.getDato(j);
            c.promedio = c.total / c.ventas;
            estadisticas.categoriasPromedio.reemplazar(j, c);
        }

        mapa.put(clave, estadisticas);
    }
}

void printPromedioVentasPorCategoriaSegunPais(HashMap<string, estadisticas_pais> &mapaPaises, Lista<string> &claves) {
    cout << endl << "------------------------" << endl;
    cout << "Promedio ventas por categorias: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        
        string clave = claves.getDato(i);

        cout << "--------------------" << endl;
        cout << "País: " << clave << endl;
        
        
        if (!mapaPaises.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        Lista<categoria> paisactual = mapaPaises.get(clave).categoriasPromedio;

        for (int j = 0; j < paisactual.getTamanio(); j++) { // Imprime todos los productos
            cout << fixed << setprecision(2);
            cout << endl;
            cout << "Categoria: " << paisactual.getDato(j).nombre << endl;
            cout << "Promedio: " << paisactual.getDato(j).promedio << endl; 
            cout << endl;
        }
    }
}

void printMedioEnvioMasUtilizadoPorPais(HashMap<string, estadisticas_pais> &mapaPaises, Lista<string> &claves) {
    cout << endl << "------------------------" << endl;
    cout << "Medio de envío más utilizado por país: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        string clave = claves.getDato(i);
        
        cout << "--------------------" << endl;
        cout << "País: " << clave << endl;
        
        if (!mapaPaises.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        cout << "El medio de envío más utilizado en " << clave << " es " << mapaPaises.get(clave).mediosDeEnvio.getDato(0).nombre << endl;
    }
}

void printEstadoDeEnvioMasFrencuentePorPais(HashMap<string, estadisticas_pais> &mapaPaises, Lista<string> &claves) {
    cout << endl << "------------------------" << endl;
    cout << "Estado de Envio Mas Frecuente Por Pais: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        
        string clave = claves.getDato(i);
        
        cout << "--------------------" << endl;
        cout << "País: " << clave << endl;
        
        
        if (!mapaPaises.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        cout << "El estado de envío más frecuente en " << clave << " es " << mapaPaises.get(clave).estadosDeEnvio.getDato(0).nombre << endl;
    }
}

void printMedioEnvioMasUtilizadoPorCategoria(HashMap<string, Lista<medioenvio_cantidad>>& mapaCategoria, Lista<string>& claves) {
    cout << endl << "------------------------" << endl;
    cout << "Medio de Envio Mas Utilizado Por Categoria: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada categoria
        
        string clave = claves.getDato(i);
        
        cout << "--------------------" << endl;
        cout << "Categoría: " << clave << endl;
        
        
        if (!mapaCategoria.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        cout << "El medio de envío más utilizado en " << clave << " es " << mapaCategoria.get(clave).getDato(0).nombre << endl;
        cout << "Cantidad: " << mapaCategoria.get(clave).getDato(0).ventas << endl;
    }
}

void printProductoMasVendido(Lista<producto_cantidad>& lista) {
    cout << "El producto más vendido es " << lista.getDato(lista.getTamanio() - 1).producto << endl;
}

void printProductoMenosVendido(Lista<producto_cantidad>& lista) {
    cout << "El producto menos vendido es " << lista.getDato(0).producto << endl;
}

void printDiaMayorCantidadVentas(dia_montos& date) {
    cout << "El día con mayor cantidad de ventas (en monto de dinero) es " << date.fecha.tm_mday << "/" << date.fecha.tm_mon + 1 << "/" << date.fecha.tm_year + 1900 << endl;
}

dia_montos buscarMaxListaDiaMonto(Lista<dia_montos>& lista) {
    dia_montos monto_mayor = lista.getDato(0);
    for (int i = 0; i < lista.getTamanio(); i++) {
        dia_montos actual = lista.getDato(i);
        if (actual.total > monto_mayor.total) {
            monto_mayor = actual;
        }
    }
    return monto_mayor;
}

HashMap<string, estadisticas_pais> getListasPorPais(HashMap<unsigned int, Venta>& mapa, Lista<string>& claves, int& size) {
    
    HashMap<string, estadisticas_pais> mapaPaises(31, hashString); //12 paises en sudamerica, ocupan el 40% --> bajas colisiones

    for (int i = 1; i <= size; i++) { //IF básico de conteo --> no cuenta
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        estadisticas_pais estadisticas; // Estructura que contiene las listas de cada pais
        // creo el objeto ciudad monto (si no existe la ciudad, se insertará; si ya existe se usa para reemplazar el valor del monto)
        ciudad_monto cm = {v.ciudad, v.monto_total};
        producto_monto p = {v.producto, v.monto_total};
        categoria c = {v.categoria, 0, v.monto_total, 1};
        medioenvio_cantidad me = {v.medio_envio, 1};
        estadoenvio_cantidad es = {v.estado_envio, 1};

        if (!mapaPaises.contieneClave(v.pais)) { //Si el pais todavia no existe
            Lista<ciudad_monto> Lciudades; //creo la lista de ciudades
            Lista<producto_monto> Lproductos; // creo la lista de productos
            Lista<categoria> Lcategorias; //creo la lista de categorias
            Lista<medioenvio_cantidad> Lmedioenvio;
            Lista<estadoenvio_cantidad> Lestadoenvio;
            Lciudades.insertarUltimo(cm); //inserto la ciudad actual
            Lproductos.insertarUltimo(p); //inserto el producto actual
            Lcategorias.insertarUltimo(c); //inserto el categoria actual
            Lmedioenvio.insertarUltimo(me); 
            Lestadoenvio.insertarUltimo(es);
            // Asigno las listas al struct
            estadisticas.ciudadesOrdenadasMonto = Lciudades; 
            estadisticas.productosMontoTotal = Lproductos; 
            estadisticas.categoriasPromedio = Lcategorias;
            estadisticas.mediosDeEnvio = Lmedioenvio;
            estadisticas.estadosDeEnvio = Lestadoenvio;
            claves.insertarUltimo(v.pais); //inserto la clave a la lista de claves
        } else { // Si el pais existe
            Lista<ciudad_monto> Lciudades = mapaPaises.get(v.pais).ciudadesOrdenadasMonto;
            bool foundCiu = false;
            for (int j = 0; j < Lciudades.getTamanio() && !foundCiu; j++) { // reviso todas las ciudades de la lista
                if (Lciudades.getDato(j).ciudad == v.ciudad) {
                    cm.total = Lciudades.getDato(j).total + v.monto_total;
                    Lciudades.reemplazar(j, cm);
                    foundCiu = true; //Indico que se encontró la ciudad y altero el monto
                }
            }
            Lista<producto_monto> Lproductos = mapaPaises.get(v.pais).productosMontoTotal;
            bool foundProd = false;
            for (int j = 0; j < Lproductos.getTamanio() && !foundProd; j++) { // reviso todas las productos de la lista
                if (Lproductos.getDato(j).nombre == v.producto) {
                    p.total = Lproductos.getDato(j).total + v.monto_total;
                    Lproductos.reemplazar(j, p);
                    foundProd = true; //Indico que se encontró el producto y altero el monto
                }
            }
            Lista<categoria> Lcategorias = mapaPaises.get(v.pais).categoriasPromedio;
            bool foundCat = false;
            for (int j = 0; j < Lcategorias.getTamanio() && !foundCat; j++) { // reviso todas las categorias de la lista
                if (Lcategorias.getDato(j).nombre == v.categoria) {
                    c.total = Lcategorias.getDato(j).total + v.monto_total;
                    c.ventas = Lcategorias.getDato(j).ventas + 1;
                    Lcategorias.reemplazar(j, c);
                    foundCat = true; //Indico que se encontró el categoria y altero el monto
                }
            }
            Lista<medioenvio_cantidad> Lmedioenvio = mapaPaises.get(v.pais).mediosDeEnvio;
            bool foundMed = false;
            for (int j = 0; j < Lmedioenvio.getTamanio() && !foundMed; j++) { 
                if (Lmedioenvio.getDato(j).nombre == v.medio_envio) {
                    me.ventas = Lmedioenvio.getDato(j).ventas + 1;
                    Lmedioenvio.reemplazar(j, me);
                    foundMed = true; 
                }
            }
            Lista<estadoenvio_cantidad> Lestadoenvio = mapaPaises.get(v.pais).estadosDeEnvio;
            bool foundEst = false;
            for (int j = 0; j < Lestadoenvio.getTamanio() && !foundEst; j++) {
                if (Lestadoenvio.getDato(j).nombre == v.estado_envio) {
                    es.ventas = Lestadoenvio.getDato(j).ventas + 1;
                    Lestadoenvio.reemplazar(j, es);
                    foundEst = true; 
                }
            }

            if (!foundCiu) {
                Lciudades.insertarUltimo(cm); //inserto la ciudad si no existe
            }
            if (!foundProd) {
                Lproductos.insertarUltimo(p); //inserto el producto si no existe
            }
            if (!foundCat) {
                Lcategorias.insertarUltimo(c); //inserto el categoria si no existe
            }
            if (!foundMed) {
                Lmedioenvio.insertarUltimo(me); //inserto el medio si no existe
            }
            if (!foundEst) {
                Lestadoenvio.insertarUltimo(es); //inserto el estado si no existe
            }

            estadisticas.ciudadesOrdenadasMonto = Lciudades;
            estadisticas.productosMontoTotal = Lproductos;
            estadisticas.categoriasPromedio = Lcategorias; // Al finalizar esto, los promedios serán 0, pero los totales serán correctos
            estadisticas.mediosDeEnvio = Lmedioenvio;
            estadisticas.estadosDeEnvio = Lestadoenvio;
        }
        mapaPaises.put(v.pais, estadisticas); // Realizo la modificacion en el hashMap
    }
    ordenarTop5CiudadesPorMontoSegunPais(mapaPaises, claves); // Ordeno de menor a mayor las ciudades por montos
    calcularPromedioVentasPorCategoriaSegunPais(mapaPaises, claves); // Arreglo los promedios
    for (int i = 0; i < claves.getTamanio(); i++) {
        string pais = claves.getDato(i);
        estadisticas_pais estadisticas = mapaPaises.get(pais);
        bubbleSortEstadosDeEnvio(estadisticas.estadosDeEnvio);
        bubbleSortMediosDeEnvio(estadisticas.mediosDeEnvio);
        mapaPaises.put(pais, estadisticas);
    }
    return mapaPaises;
}

HashMap<string, Lista<medioenvio_cantidad>> getListasPorCategoria(HashMap<unsigned int, Venta>& mapa, Lista<string>& claves, int& size) {
   
    HashMap<string, Lista<medioenvio_cantidad>> mapaCategorias(11, hashString); // 4 categorias * 2, numero primo mas cercano
    
    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        medioenvio_cantidad medio = {v.medio_envio, 1};
        Lista<medioenvio_cantidad> Lmedios;

        if (!mapaCategorias.contieneClave(v.categoria)) { //No existe la categoria
            Lmedios.insertarUltimo(medio);
            claves.insertarUltimo(v.categoria);
        } else {
            Lmedios = mapaCategorias.get(v.categoria);
            bool foundMed = false;
            for (int j = 0; j < Lmedios.getTamanio() && !foundMed; j++) {
                if (Lmedios.getDato(j).nombre == v.medio_envio) {
                    medio.ventas = Lmedios.getDato(j).ventas + 1;
                    Lmedios.reemplazar(j, medio);
                    foundMed = true;
                }
            }
            if (!foundMed) {
                Lmedios.insertarUltimo(medio);
            }
        }
        mapaCategorias.put(v.categoria, Lmedios);
    }
    for (int i = 0; i < claves.getTamanio(); i++) {
        string clave = claves.getDato(i);
        Lista<medioenvio_cantidad> lista_medios = mapaCategorias.get(clave);
        bubbleSortMediosDeEnvio(lista_medios);
        mapaCategorias.put(clave, lista_medios);
    }
    return mapaCategorias;
}

Lista<producto_cantidad> getListaOrdenadaProductos(HashMap<unsigned int, Venta>& mapa, int& size) {
    // PODRIA INCORPORARSE CON COMPARACION ENTRE DOS PAISES - PRODUCTOS MAS VENDIDOS
    Lista<producto_cantidad> productos;

    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        producto_cantidad pc = {v.producto, v.cantidad};

        bool found = false;
        for (int j = 0; j < productos.getTamanio() && !found; j++) {
            if (productos.getDato(j).producto == v.producto) {
                pc.cantidad = productos.getDato(j).cantidad + v.cantidad;
                productos.reemplazar(j, pc);
                found = true;
            }
        }
        if (!found) {
            productos.insertarUltimo(pc);
        }
    }

    quicksortListaPC(productos, 0, productos.getTamanio() - 1);

    return productos;
}

dia_montos getDiaConMayorCantidadVentas(HashMap<unsigned int, Venta>& mapa, int& size) {
    
    Lista<dia_montos> fechas;

    for (int i = 1; i <= size; i++) {
        if(!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        dia_montos dia = {v.fecha, v.monto_total};

        bool found = false;
        for (int j = 0; j < fechas.getTamanio() && !found; j++) {
            if (fechas.getDato(j).fecha.tm_mday == v.fecha.tm_mday && 
                fechas.getDato(j).fecha.tm_mon == v.fecha.tm_mon && 
                fechas.getDato(j).fecha.tm_year == v.fecha.tm_year) {
                dia.total = fechas.getDato(j).total + v.monto_total;
                fechas.reemplazar(j, dia);
                found = true;
            }
        }
        if (!found) {
            fechas.insertarUltimo(dia);
        }
    }
    
    dia_montos max = buscarMaxListaDiaMonto(fechas);

    return max;
}

int agregarVenta(HashMap<unsigned int, Venta>& mapa, int& size, Pila<int>& id_disponibles, Lista<string>& claves) {
    Venta v;
    
    size++;  
    if (!id_disponibles.esVacia()) {
        v.id = id_disponibles.pop();
    } else {
        v.id = size;
    }
    cout << "Ingrese los datos de la venta con ID: " << v.id << endl;
    
    time_t tiempoActual = time(nullptr); // Obtiene el tiempo actual
    tm fecha = *localtime(&tiempoActual); // Convierte a estructura tm

    // Fijar la hora a 00:00:00
    fecha.tm_hour = 0;
    fecha.tm_min = 0;
    fecha.tm_sec = 0;

    // Convertir de nuevo a time_t para reflejar el cambio
    time_t tiempoFijo = mktime(&fecha);

    // Mostrar la fecha con la hora fija
    cout << "Fecha de la venta: " << asctime(&fecha) << endl;

    v.fecha = fecha;

    cout << "Categoría del producto: ";
    cin.ignore();
    getline(cin, v.categoria);

    cout << "Nombre del producto: ";
    getline(cin, v.producto);

    cout << "Cantidad del producto: ";
    cin >> v.cantidad;

    cout << "Precio unitario: ";
    cin >> v.precio_unitario;
    
    v.monto_total = v.precio_unitario * v.cantidad;

    cout << "Monto total de " << v.monto_total << endl;

    cout << "Nombre del cliente: ";
    cin.ignore();
    getline(cin, v.cliente);

    cout << "Nombre de la ciudad: ";
    getline(cin, v.ciudad);

    cout << "Nombre del país: ";
    getline(cin, v.pais);
    bool found = false;
    for (int i = 0; i < claves.getTamanio() && !found; i++) {
        if (claves.getDato(i) == v.pais) {
            found = true;
        }
    }
    if (!found) {
        claves.insertarUltimo(v.pais);
    }

    cout << "Medio de envío: ";
    getline(cin, v.medio_envio);

    cout << "Estado del envío: ";
    getline(cin, v.estado_envio);

    mapa.put(v.id, v);
    cout << "Carga exitosa!" << endl;
    return v.id;
}

int eliminarVenta(HashMap<unsigned int, Venta>& mapa, int &size, Pila<int>& id_disponibles) {
    int id;
    cout << "Ingrese el ID de la venta que desee eliminar." << endl;
    cin >> id;
    if(mapa.contieneClave(id)) {
        id_disponibles.push(id);
        mapa.remove(id);
        cout << "Venta eliminada!" << endl;
        return id;
    } else {
        cout << "No hay una venta con ese ID" << endl;
        return -1;
    }
}

int modificarVenta(HashMap<unsigned int, Venta>& mapa, int &size, Lista<string>& claves) {
    int id;
    cout << "Ingrese el ID de la venta que desee modificar." << endl;
    cin >> id;
    if(mapa.contieneClave(id)) {
        Venta v = mapa.get(id);
        int option;
        do {  
            cout << "----- ¿Qué desea modificar? -----" << endl;
            cout << "01) Categoria" << endl;
            cout << "02) Producto" << endl;
            cout << "03) Cantidad" << endl;
            cout << "04) Precio Unitario" << endl;
            cout << "05) Cliente" << endl;
            cout << "06) Ciudad" << endl;
            cout << "07) Pais" << endl;
            cout << "08) Medio de envío" << endl;
            cout << "09) Estado del envío" << endl;
            cout << "00) EXIT" << endl << endl;
            cin >> option;
            bool found = false;
            switch (option)
            {
            case 1:
                cout << "Categoría: " << endl;
                cin.ignore();
                getline(cin, v.categoria);
                break;
            case 2:
                cout << "Producto: " << endl;
                cin.ignore();
                getline(cin, v.producto);
                break;
            case 3:
                cout << "Cantidad: " << endl;
                cin >> v.cantidad;
                cin.ignore();
                v.monto_total = v.cantidad * v.precio_unitario;
                break;
            case 4:
                cout << "Precio Unitario: " << endl;
                cin >> v.precio_unitario;
                v.monto_total = v.cantidad * v.precio_unitario;
                break;
            case 5:
                cout << "Cliente: " << endl;
                cin.ignore();
                getline(cin, v.cliente);
                break;
            case 6:
                cout << "Ciudad: " << endl;
                cin.ignore();
                getline(cin, v.ciudad);
                break;
            case 7:
                cout << "Pais: " << endl;
                cin.ignore();
                getline(cin, v.pais);
                for (int i = 0; i < claves.getTamanio() && !found; i++) {
                    if (claves.getDato(i) == v.pais) {
                        found = true;
                    }
                }
                if (!found) {
                    claves.insertarUltimo(v.pais);
                }
                break;
            case 8:
                cout << "Medio de envío: " << endl;
                cin.ignore();
                getline(cin, v.medio_envio);
                break;
            case 9: 
                cout << "Estado de envío: " << endl;
                cin.ignore();
                getline(cin, v.estado_envio);
                break;
            case 0:
                cout << "Saliendo..." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
                break;
            }
        } while (option != 0);
        mapa.put(id, v);
        cout << "Modificaciones exitosas" << endl;
        return id;
    } else {
        cout << "No hay una venta con ese ID" << endl;
        return -1;
    }
}

void printVentasCiudad(string ciudad, HashMap<unsigned int, Venta>& mapa, int &size) {
    
    Lista<int> VentasCiudad;

    for (int i = 1; i <= size; i++) {
        if(!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        if (v.ciudad == ciudad) {
            VentasCiudad.insertarUltimo(i);
        }
    }

    VentasCiudad.print();
}

void printVentasRangoFechas(tm start, tm end, HashMap<unsigned int, Venta>& mapa, int& size, Lista<string>& paises) {
    HashMap<string, Lista<int>> mapaPaisesVentas(31, hashString);
    Lista<int> VentasFecha;
    time_t start_t = mktime(&start);
    time_t end_t = mktime(&end);

    if (start_t > end_t) {
        cout << "La fecha de inicio es mayor a la fecha de fin, vuelva a intentar." << endl;
        return;
    }

    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        tm date = v.fecha;
        time_t date_t = mktime(&date);
        if (date_t >= start_t && date_t <= end_t) {
            cout << "enter" << endl;
            if (mapaPaisesVentas.contieneClave(v.pais)) {
                VentasFecha = mapaPaisesVentas.get(v.pais);
            }
            VentasFecha.insertarUltimo(v.id);
            mapaPaisesVentas.put(v.pais, VentasFecha);
        }
    }

    for (int j = 0; j < paises.getTamanio(); j++) {
        string clave = paises.getDato(j);
        if (mapaPaisesVentas.contieneClave(clave)) {
            cout << "Ids de las ventas en " << clave << endl;
            mapaPaisesVentas.get(clave).print(); 
            cout << "------------" << endl;
        }
    }
}

Lista<int> getListaVentasPais(string pais, HashMap<unsigned int, Venta>& mapa, int& size) {
    Lista<int> VentasPais;
    // Se podría directamente utilizar en la primera funcion
    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        if (v.pais == pais) {
            VentasPais.insertarUltimo(v.id);
        }
    }

    return VentasPais;
}

void compararMontosPais(string pais1, string pais2, HashMap<unsigned int, Venta>& mapa, int& size) {
    Lista<int> listaPais1 = getListaVentasPais(pais1, mapa, size);
    Lista<int> listaPais2 = getListaVentasPais(pais2, mapa, size);
    
    float monto_pais1 = 0, monto_pais2 = 0;
    for (int i = 0; i < listaPais1.getTamanio(); i++) {
        monto_pais1 += mapa.get(listaPais1.getDato(i)).monto_total;
    }
    for (int j = 0; j < listaPais2.getTamanio(); j++) {
        monto_pais2 += mapa.get(listaPais2.getDato(j)).monto_total;
    }

    cout << fixed << setprecision(2);
    cout << "El monto total de " << pais1 << " es " << monto_pais1 << endl;
    cout << "El monto total de " << pais2 << " es " << monto_pais2 << endl;
    if (monto_pais1 > monto_pais2) {
        cout << "El monto total de ventas de " << pais1 << " es mayor al de " << pais2 << endl;
    } else if (monto_pais2 > monto_pais1) {
        cout << "El monto total de ventas de " << pais2 << " es mayor al de " << pais1 << endl;
    } else {
        cout << "Los montos totales son iguales" << endl;
    }
}

void compararProductosMasVendidos(string pais1, string pais2, HashMap<unsigned int, Venta>& mapa, int& size) {
    Lista<int> listaPais1 = getListaVentasPais(pais1, mapa, size);
    Lista<int> listaPais2 = getListaVentasPais(pais2, mapa, size);
    
    Lista<producto_cantidad> productos_pais1;
    Lista<producto_cantidad> productos_pais2;
    for (int i = 0; i < listaPais1.getTamanio(); i++) {
        Venta v = mapa.get(listaPais1.getDato(i));
        producto_cantidad pc = {v.producto, v.cantidad};

        bool found = false;
        for (int j = 0; j < productos_pais1.getTamanio() && !found; j++) {
            if (productos_pais1.getDato(j).producto == v.producto) {
                pc.cantidad = productos_pais1.getDato(j).cantidad + v.cantidad;
                productos_pais1.reemplazar(j, pc);
                found = true;
            }
        }
        if (!found) {
            productos_pais1.insertarUltimo(pc);
        }
    }
    for (int j = 0; j < listaPais2.getTamanio(); j++) {
        Venta v = mapa.get(listaPais2.getDato(j));
        producto_cantidad pc = {v.producto, v.cantidad};

        bool found = false;
        for (int j = 0; j < productos_pais2.getTamanio() && !found; j++) {
            if (productos_pais2.getDato(j).producto == v.producto) {
                pc.cantidad = productos_pais2.getDato(j).cantidad + v.cantidad;
                productos_pais2.reemplazar(j, pc);
                found = true;
            }
        }
        if (!found) {
            productos_pais2.insertarUltimo(pc);
        }
    }

    quicksortListaPC(productos_pais1, 0, productos_pais1.getTamanio() - 1);
    quicksortListaPC(productos_pais2, 0, productos_pais2.getTamanio() - 1);

    producto_cantidad mas_vendido_1 = productos_pais1.getDato(productos_pais1.getTamanio() - 1);
    producto_cantidad mas_vendido_2 = productos_pais2.getDato(productos_pais2.getTamanio() - 1);

    cout << "El producto más vendido de " << pais1 << " es " << mas_vendido_1.producto << endl;
    cout << "El producto más vendido de " << pais2 << " es " << mas_vendido_2.producto << endl;

    if (mas_vendido_1.cantidad > mas_vendido_2.cantidad) {
        cout << "El producto " << mas_vendido_1.producto << " fue el más vendido de los dos." << endl;
    } else if (mas_vendido_1.cantidad < mas_vendido_2.cantidad) {
        cout << "El producto " << mas_vendido_2.producto << " fue el más vendido de los dos." << endl;
    } else {
        cout << "Ambos productos fueron vendidos por igual." << endl;
    }
}

void compararMedioDeEnvio(string pais1, string pais2, HashMap<string, estadisticas_pais>& mapa) {
    Lista<medioenvio_cantidad> lista_pais1 = mapa.get(pais1).mediosDeEnvio;
    Lista<medioenvio_cantidad> lista_pais2 = mapa.get(pais2).mediosDeEnvio;
    bubbleSortMediosDeEnvio(lista_pais1);
    bubbleSortMediosDeEnvio(lista_pais2);
    medioenvio_cantidad may_medio_pais1 = lista_pais1.getDato(0);
    medioenvio_cantidad may_medio_pais2 = lista_pais2.getDato(0);

    cout << "El medio de envío más utilizado de " << pais1 << " es " << may_medio_pais1.nombre << endl;
    cout << "El medio de envío más utilizado de " << pais2 << " es " << may_medio_pais2.nombre << endl;

    if (may_medio_pais1.ventas > may_medio_pais2.ventas) {
        cout << "El medio de envío " << may_medio_pais1.nombre << " fue el más utilizado de los dos." << endl;
    } else if (may_medio_pais1.ventas < may_medio_pais2.ventas) {
        cout << "El medio de envio " << may_medio_pais2.nombre << " fue el más utilizado de los dos." << endl;
    } else {
        cout << "Ambos medios de envio fueron utilizados por igual." << endl;
    }
}

void compararCantidadProductos(string producto1, string producto2, HashMap<unsigned int, Venta>& mapa, int& size, Lista<string>& paises) {
    HashMap<string, int> totalProducto1(31, hashString);
    HashMap<string, int> totalProducto2(31, hashString);

    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);

        if (v.producto == producto1) {
            if (!totalProducto1.contieneClave(v.pais)) {
                totalProducto1.put(v.pais, v.cantidad);
            } else {
                totalProducto1.put(v.pais, totalProducto1.get(v.pais) + v.cantidad);
            }
        } else if (v.producto == producto2) {
            if (!totalProducto2.contieneClave(v.pais)) {
                totalProducto2.put(v.pais, v.cantidad);
            } else {
                totalProducto2.put(v.pais, totalProducto2.get(v.pais) + v.cantidad);
            }
        }
    }

    cout << "Cantidad de productos vendidos por país: " << endl;
    cout << "----------------------------------" << endl;
    for (int j = 0; j < paises.getTamanio(); j++) {
        string pais = paises.getDato(j);
        cout << pais << endl;
        cout << producto1 << ": " << totalProducto1.get(pais) << endl;
        cout << producto2 << ": " << totalProducto2.get(pais) << endl;
        cout << endl;
    }
    
}

void compararMontosProductos(string producto1, string producto2, HashMap<string, estadisticas_pais>& mapaPaises, Lista<string>& claves) {
    cout << endl << "------------------------" << endl;
    cout << "Monto total por producto por país: " << endl;

    for (int i = 0; i < claves.getTamanio(); i++) { //Print cada pais
        
        string clave = claves.getDato(i);

        cout << "--------------------" << endl;
        cout << "País: " << clave << endl << endl;
        
        if (!mapaPaises.contieneClave(clave)) {
            cout << "Error: el mapa no contiene la clave: " << clave << endl;
            continue;
        }

        Lista<producto_monto> paisactual = mapaPaises.get(clave).productosMontoTotal;

        for (int j = 0; j < paisactual.getTamanio(); j++) { // Imprime todos los productos
            producto_monto producto = paisactual.getDato(j);
            if (producto.nombre == producto1 || producto.nombre == producto2){
                cout << fixed << setprecision(2);
                cout << producto.nombre << ": " << producto.total << endl; 
            }
        } 
    }
    cout << "--------------------" << endl;
}

void buscarProductosPromedio(float monto, bool superior, string pais, HashMap<unsigned int, Venta>& mapa, int& size) {
    
    Lista<producto_promedio> lista;

    for (int i = 1; i <= size; i++) {
        if (!mapa.contieneClave(i)) continue;
        Venta v = mapa.get(i);
        if (v.pais == pais) {
            bool found = false;
            for (int j = 0; j < lista.getTamanio() && !found; j++) {
                if (lista.getDato(j).producto == v.producto) {
                    producto_promedio pp = lista.getDato(j);
                    pp.total += v.monto_total;
                    pp.ventas++;
                    pp.promedio = pp.total / pp.ventas;
                    lista.reemplazar(j, pp);
                    found = true;
                }
            }
            if (!found) {
                producto_promedio pp = {v.producto, v.monto_total, 1, v.monto_total};
                lista.insertarUltimo(pp);
            }
        }
    }

    quicksortListaPP(lista, 0, lista.getTamanio() - 1);

    int pos = busquedaBinaria(lista, monto);

    if (superior) {
        cout << "Productos con monto promedio mayor a $" << monto << " por unidad vendidos en " << pais << ": " << endl;
        if (pos < lista.getTamanio() && lista.getDato(pos).promedio <= monto) {
            pos++;
        }
        for (int l = pos; l < lista.getTamanio(); l++) {
            cout << lista.getDato(l).producto << " -> ";
        }
        cout << "NULL" << endl;
    } else {
        cout << "Productos con monto promedio menor a $" << monto << " por unidad vendidos en " << pais << ": " << endl;
        if (pos >= 0 && lista.getDato(pos).promedio >= monto) {
            pos--;
        }
        for (int l = 0; l < pos + 1; l++) {
            cout << lista.getDato(l).producto << " -> ";
        }
        cout << "NULL" << endl;
    }
}

void actualizarTodo(
    HashMap<unsigned int, Venta>& mapaVenta,
    int& sizeofmap,
    Lista<string>& claves_mapaPaises,
    HashMap<string, estadisticas_pais>& mapaPaises,
    Lista<string>& claves_mapaCategorias,
    HashMap<string, Lista<medioenvio_cantidad>>& mapaCategorias,
    Lista<producto_cantidad>& listaOrdenadaProductosPorCantidad,
    dia_montos& fechaConMasVentas)
{
    // Vaciamos estructuras anteriores
    claves_mapaPaises = Lista<string>(); // limpia lista de claves de países
    mapaPaises = HashMap<string, estadisticas_pais>(31, hashString); // reinicializa con mismo tamaño

    claves_mapaCategorias = Lista<string>(); // limpia lista de claves de categorías
    mapaCategorias = HashMap<string, Lista<medioenvio_cantidad>>(11, hashString);

    listaOrdenadaProductosPorCantidad = Lista<producto_cantidad>(); // limpia lista de productos

    // Volvemos a generar cada estructura usando las funciones existentes:
    mapaPaises = getListasPorPais(mapaVenta, claves_mapaPaises, sizeofmap);
    mapaCategorias = getListasPorCategoria(mapaVenta, claves_mapaCategorias, sizeofmap);
    listaOrdenadaProductosPorCantidad = getListaOrdenadaProductos(mapaVenta, sizeofmap);
    fechaConMasVentas = getDiaConMayorCantidadVentas(mapaVenta, sizeofmap);
}

#endif