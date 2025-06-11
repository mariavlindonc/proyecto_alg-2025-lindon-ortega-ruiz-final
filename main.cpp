#include "library.h"
#include <iostream> // cout, cin
#include <sstream> // Permite trabajar con las lineas de csv como strings (stringstream, getline)
#include <fstream> // Permite leer y escribir archivo externos (ifstream)
#include <iomanip> // Permite manipular cout para que no trunque automaticamente los float
#include <ctime>
#include "HashMap\HashMap.h" // CAMBIAR POR HASHMAP LIST PARA USAR MEJOR MANEJO DE COLISIONES
#include "Lista\Lista.h"
#include "Pila\Pila.h"
#define NOMBRE_ARCHIVO ("C:/Users/mairi/source/proyecto-2025-comision-tosetti-lindon-ortega-ruiz/ventas_sudamerica.csv")
using namespace std;

void printMenu(int n);

int main(void) {
    int sizeofmap = 0;
    float monto;
    string aux_ciudad, start_date, end_date, pais1, pais2, producto1, producto2;
    // CARGAR LOS DATOS - A través de una funcion en un archivo cpp
    cout << "Cargando datos..." << endl;
    HashMap<unsigned int, Venta> mapaVenta(5999, hashFunc);

    loadFile (mapaVenta, sizeofmap); // Mapa de ventas por ID
    cout << sizeofmap << " lineas cargadas." << endl;
    
    //START PROCESAMIENTO

    Lista<string> claves_mapaPaises; // Almaceno las claves de mapaPaises (usadas en las siguientes funciones)

    // Extract data from map --> returns struct
    HashMap<string, estadisticas_pais> mapaPaises = getListasPorPais(mapaVenta, claves_mapaPaises, sizeofmap);
    
    Lista<string> claves_mapaCategorias;
    
    HashMap<string, Lista<medioenvio_cantidad>> mapaCategorias = getListasPorCategoria(mapaVenta, claves_mapaCategorias, sizeofmap);

    Lista<producto_cantidad> listaOrdenadaProductosPorCantidad = getListaOrdenadaProductos(mapaVenta, sizeofmap);

    dia_montos fechaConMasVentas = getDiaConMayorCantidadVentas(mapaVenta, sizeofmap);

    //END PROCESAMIENTO

    // Creo una pila que almacena IDs eliminados
    Pila<int> ID_eliminados;

    bool salir = false;
    while (!salir) {
        int option;
        printMenu(0);
        cin >> option;
        switch (option) {
            case 1: {
                int process;
                printMenu(1);
                cin >> process;
                switch (process) {
                    case 1:
                        printTop5CiudadesPorMontoSegunPais(mapaPaises, claves_mapaPaises);
                        break;
                    case 2:
                        printMontoTotalPorProductoSegunPais(mapaPaises, claves_mapaPaises);
                        break;
                    case 3:
                        printPromedioVentasPorCategoriaSegunPais(mapaPaises, claves_mapaPaises);
                        break;
                    case 4:
                        printMedioEnvioMasUtilizadoPorPais(mapaPaises, claves_mapaPaises);
                        break;
                    case 5:
                        printMedioEnvioMasUtilizadoPorCategoria(mapaCategorias, claves_mapaCategorias);
                        break;
                    case 6:
                        printDiaMayorCantidadVentas(fechaConMasVentas);
                        break;
                    case 7:
                        printEstadoDeEnvioMasFrencuentePorPais(mapaPaises, claves_mapaPaises);
                        break;
                    case 8:
                        printProductoMasVendido(listaOrdenadaProductosPorCantidad);
                        break;
                    case 9:
                        printProductoMenosVendido(listaOrdenadaProductosPorCantidad);
                        break;
                    default:
                        cout << "Volviendo al menú principal...";
                        break;
                    }
            break;
        }
        case 2: {
            int process;
            printMenu(2);
            cin >> process;
            int id_modificado;
            switch (process) {
                case 1:
                    id_modificado = agregarVenta(mapaVenta, sizeofmap, ID_eliminados, claves_mapaPaises);
                    if (id_modificado != -1) {
                        actualizarTodo(mapaVenta, sizeofmap, claves_mapaPaises, mapaPaises, claves_mapaCategorias, mapaCategorias, listaOrdenadaProductosPorCantidad, fechaConMasVentas);
                    } 
                    break;
                case 2:
                    id_modificado = eliminarVenta(mapaVenta, sizeofmap, ID_eliminados);
                    if (id_modificado != -1) {
                        actualizarTodo(mapaVenta, sizeofmap, claves_mapaPaises, mapaPaises, claves_mapaCategorias, mapaCategorias, listaOrdenadaProductosPorCantidad, fechaConMasVentas);
                    }
                    break;
                case 3:
                    id_modificado = modificarVenta(mapaVenta, sizeofmap, claves_mapaPaises);
                    if (id_modificado != -1) {
                        actualizarTodo(mapaVenta, sizeofmap, claves_mapaPaises, mapaPaises, claves_mapaCategorias, mapaCategorias, listaOrdenadaProductosPorCantidad, fechaConMasVentas);
                    }
                    break;
                default:
                    cout << "Unvalid option. Leaving...";
                    break;
            }
            break;
        }
        case 3: {
            int process;
            printMenu(3);
            cin >> process;
            switch (process) {
                case 1:
                    cout << "Ingrese la ciudad: ";
                    cin >> aux_ciudad;
                    printVentasCiudad(aux_ciudad, mapaVenta, sizeofmap);
                    break;
                case 2:
                    do {
                        cout << "USAR FORMATO DD/MM/YYYY HH:MM" << endl;
                        cout << "Ingrese fecha de inicio: ";
                        cin.ignore();
                        getline(cin, start_date);
                        cout << "Ingrese fecha de fin: ";
                        getline(cin, end_date);
                    } while (!validarFecha(start_date) && !validarFecha(end_date));
                    printVentasRangoFechas(stringToDateTime(start_date), stringToDateTime(end_date), mapaVenta, sizeofmap, claves_mapaPaises);
                    break;
                case 3:
                    cout << "País 1: " << endl;
                    cin >> pais1;
                    cout << "País 2: " << endl;
                    cin >> pais2;
                    compararMontosPais(pais1, pais2, mapaVenta, sizeofmap);
                    break;
                case 4:
                    cout << "País 1: " << endl;
                    cin >> pais1;
                    cout << "País 2: " << endl;
                    cin >> pais2;
                    compararProductosMasVendidos(pais1, pais2, mapaVenta, sizeofmap);
                    break;
                case 5:
                    cout << "País 1: " << endl;
                    cin >> pais1;
                    cout << "País 2: " << endl;
                    cin >> pais2;
                    compararMedioDeEnvio(pais1, pais2, mapaPaises);
                    break;
                case 6:
                    cout << "Producto 1: " << endl;
                    cin >> producto1;
                    cout << "Producto 2: " << endl;
                    cin >> producto2;
                    compararCantidadProductos(producto1, producto2, mapaVenta, sizeofmap, claves_mapaPaises);
                    break;
                case 7:
                    cout << "Producto 1: " << endl;
                    cin >> producto1;
                    cout << "Producto 2: " << endl;
                    cin >> producto2;
                    compararMontosProductos(producto1, producto2, mapaPaises, claves_mapaPaises);
                    break;
                case 8:
                    cout << "Umbral: " << endl;
                    cin >> monto;
                    cout << "País: " << endl;
                    cin >> pais1;
                    buscarProductosPromedio(monto, false, pais1, mapaVenta, sizeofmap);
                    break;
                case 9:
                    cout << "Umbral: " << endl;
                    cin >> monto;
                    cout << "País: " << endl;
                    cin >> pais1;
                    buscarProductosPromedio(monto, true, pais1, mapaVenta, sizeofmap);
                    break;
                default:
                    cout << "Unvalid option. Leaving...";
                    break;
            }
            break;
        }
        default:
             cout << "¿Desea salir del programa? (s/n): ";
                char confirm;
                cin >> confirm;
                if (confirm == 's' || confirm == 'S') {
                    salir = true;
                    cout << "Saliendo del programa..." << endl;
                } else {
                    cout << "Volviendo al menú principal..." << endl;
                }
                break;
        }
    }
    
    return 0;
}

void printMenu(int n) {
    switch (n)
    {
    case 0:
        cout << "--------- MENU ----------" << endl;
        cout << "01) Revisar estadísticas" << endl;
        cout << "02) Modificar datos" << endl;
        cout << "03) Realizar una consulta" << endl;
        cout << "Ingrese cualquier otro número para salir." << endl;
        cout << endl;
        break;
    case 1:
        cout << "--------- Estadisticas ----------" << endl;
        cout << "01) Top 5 ciudades por monto segun pais" << endl;
        cout << "02) Montos totales por producto segun pais" << endl;
        cout << "03) Promedio de ventas por categoria segun pais" << endl;
        cout << "04) Medio de envío más utilizado por país." << endl;
        cout << "05) Medio de envío más utilizado por categoría." << endl;
        cout << "06) Día con mayor cantidad de ventas (por monto de dinero) en toda la base de datos." << endl;
        cout << "07) Estado de envío más frecuente por país." << endl;
        cout << "08) Producto más vendido en cantidad total (no en dinero, sino en unidades)." << endl;
        cout << "09) Producto menos vendido en cantidad total." << endl;
        cout << "Ingrese cualquier otro número para volver." << endl;
        cout << endl;
        break;
    case 2:
        cout << "---- Modificacion de datos ----" << endl;
        cout << "01) Agregar una venta" << endl;
        cout << "02) Eliminar una venta" << endl;
        cout << "03) Modificar una venta" << endl;
        cout << "Ingrese cualquier otro número para volver." << endl;
        cout << endl;
        break;
    case 3:
        cout << "------ Consultas Dinamicas -----" << endl;
        cout << "01) Ventas de una ciudad" << endl;
        cout << "02) Ventas en un rango de fechas" << endl;
        cout << "03) Comparar dos paises por monto total" << endl;
        cout << "04) Comparar dos paises por cantidad vendida" << endl;
        cout << "05) Comparar dos paises por medio de envío" << endl;
        cout << "06) Comparar dos productos por cantidad total" << endl;
        cout << "07) Comparar dos products por monto" << endl;
        cout << "08) Buscar productos vendidos en promedio por debajo de un monto" << endl;
        cout << "09) Buscar productos vendidos en promedio por encima de un monto" << endl;
        cout << endl;
        break;
    default:
        cout << "Saliendo...";
        break;
    }
}