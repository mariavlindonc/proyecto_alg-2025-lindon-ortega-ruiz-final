#ifndef NODO_H
#define NODO_H

template<class T>
class Nodo{
    private:
        T dato;
        Nodo<T> *siguiente;
    
    public:
        Nodo() : siguiente(nullptr) {} //AGREGAMOS UN CONSTRUCTOR POR DEFECTO
        Nodo(const T& d) : dato(d), siguiente(nullptr) {} //CONSTRUCTOR CON DATO
       T getDato(){
           return dato;
       }

       void setDato(T d){
           dato = d;
       }

       Nodo<T> *getSiguiente(){
           return siguiente;
       }

       void setSiguiente(Nodo<T> *siguiente){
           this->siguiente = siguiente;
       }


};

#endif //NODO_H