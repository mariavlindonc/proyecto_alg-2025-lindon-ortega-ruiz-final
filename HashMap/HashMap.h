// REEMPLAZA TODO EL CONTENIDO DEL ARCHIVO CON ESTE
#ifndef U05_HASH_HASHMAP_HASHMAP_H_
#define U05_HASH_HASHMAP_HASHMAP_H_

#include "HashEntry.h"
#include <stdexcept>
#include <iostream>

template <class K, class T>
class HashMap {
private:
  HashEntry<K, T> **tabla;
  unsigned int tamanio;

  static unsigned int hashFunc(K clave);
  unsigned int (*hashFuncP)(K clave);

public:
  explicit HashMap(unsigned int k);
  HashMap(unsigned int k, unsigned int (*hashFuncP)(K clave));
  HashMap(const HashMap<K, T> &other);
  HashMap<K, T> &operator=(const HashMap<K, T> &other);
  ~HashMap();

  T &get(K clave);
  void put(K clave, T valor);
  void remove(K clave);

  bool esVacio();
  void print();
  bool contieneClave(K clave);
};

// Constructor por defecto
template <class K, class T>
HashMap<K, T>::HashMap(unsigned int k) {
  tamanio = k;
  tabla = new HashEntry<K, T> *[tamanio];
  for (unsigned int i = 0; i < tamanio; i++) {
    tabla[i] = nullptr;
  }
  hashFuncP = hashFunc;
}

// Constructor con función de hash externa
template <class K, class T>
HashMap<K, T>::HashMap(unsigned int k, unsigned int (*fp)(K)) {
  tamanio = k;
  tabla = new HashEntry<K, T> *[tamanio];
  for (unsigned int i = 0; i < tamanio; i++) {
    tabla[i] = nullptr;
  }
  hashFuncP = fp;
}

// Constructor por copia
template <class K, class T>
HashMap<K, T>::HashMap(const HashMap<K, T> &other) {
  tamanio = other.tamanio;
  hashFuncP = other.hashFuncP;
  tabla = new HashEntry<K, T> *[tamanio];
  for (unsigned int i = 0; i < tamanio; i++) {
    if (other.tabla[i])
      tabla[i] = new HashEntry<K, T>(*other.tabla[i]);
    else
      tabla[i] = nullptr;
  }
}

// Operador de asignación
template <class K, class T>
HashMap<K, T> &HashMap<K, T>::operator=(const HashMap<K, T> &other) {
  if (this != &other) {
    for (unsigned int i = 0; i < tamanio; i++)
      delete tabla[i];
    delete[] tabla;

    tamanio = other.tamanio;
    hashFuncP = other.hashFuncP;
    tabla = new HashEntry<K, T> *[tamanio];
    for (unsigned int i = 0; i < tamanio; i++) {
      if (other.tabla[i])
        tabla[i] = new HashEntry<K, T>(*other.tabla[i]);
      else
        tabla[i] = nullptr;
    }
  }
  return *this;
}

// Destructor
template <class K, class T>
HashMap<K, T>::~HashMap() {
  for (unsigned int i = 0; i < tamanio; i++) {
    delete tabla[i];
  }
  delete[] tabla;
}

// GET por referencia
template <class K, class T>
T &HashMap<K, T>::get(K clave) {
  unsigned int pos = hashFuncP(clave) % tamanio;
  unsigned int originalPos = pos;

  do {
    if (tabla[pos] == nullptr) throw 404;
    if (tabla[pos]->getClave() == clave) return tabla[pos]->getValor();
    pos = (pos + 1) % tamanio;
  } while (pos != originalPos);

  throw 409;
}

// PUT con reemplazo
template <class K, class T>
void HashMap<K, T>::put(K clave, T valor) {
  unsigned int pos = hashFuncP(clave) % tamanio;
  unsigned int originalPos = pos;
  unsigned int intentos = 0;

  while (tabla[pos] != nullptr && intentos < tamanio) {
    if (tabla[pos]->getClave() == clave) {
      tabla[pos]->setValor(valor);
      return;
    }
    pos = (pos + 1) % tamanio;
    intentos++;
  }

  if (intentos < tamanio)
    tabla[pos] = new HashEntry<K, T>(clave, valor);
  else
    throw std::runtime_error("HashMap lleno. No se pudo insertar.");
}

// REMOVE con rehash del cluster
template <class K, class T>
void HashMap<K, T>::remove(K clave) {
  unsigned int pos = hashFuncP(clave) % tamanio;
  unsigned int originalPos = pos;

  while (tabla[pos] && tabla[pos]->getClave() != clave) {
    pos = (pos + 1) % tamanio;
    if (pos == originalPos) return;
  }

  if (!tabla[pos]) return;
  delete tabla[pos];
  tabla[pos] = nullptr;

  pos = (pos + 1) % tamanio;
  while (tabla[pos]) {
    HashEntry<K, T> *rehash = tabla[pos];
    tabla[pos] = nullptr;
    put(rehash->getClave(), rehash->getValor());
    delete rehash;
    pos = (pos + 1) % tamanio;
  }
}

// FUNCIONES ADICIONALES

template <class K, class T>
bool HashMap<K, T>::esVacio() {
  for (unsigned int i = 0; i < tamanio; i++) {
    if (tabla[i] != nullptr) return false;
  }
  return true;
}

template <class K, class T>
unsigned int HashMap<K, T>::hashFunc(K clave) {
  return (unsigned int)clave;
}

template <class K, class T>
void HashMap<K, T>::print() {
  std::cout << "i\tClave\t\tValor" << std::endl;
  std::cout << "----------------------" << std::endl;
  for (unsigned int i = 0; i < tamanio; i++) {
    std::cout << i << "\t";
    if (tabla[i] != nullptr) {
      std::cout << tabla[i]->getClave() << "\t\t" << tabla[i]->getValor();
    }
    std::cout << std::endl;
  }
}

template <class K, class T>
bool HashMap<K, T>::contieneClave(K clave) {
  unsigned int pos = hashFuncP(clave) % tamanio;
  unsigned int originalPos = pos;

  do {
    if (tabla[pos] == nullptr) return false;
    if (tabla[pos]->getClave() == clave) return true;
    pos = (pos + 1) % tamanio;
  } while (pos != originalPos);

  return false;
}

#endif // U05_HASH_HASHMAP_HASHMAP_H_