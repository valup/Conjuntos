#include "chash.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Recibe una capacidad y funciones para hash, paso y comparacion
y crea una tabla de conjuntos con esa informacion */
ConjHash* chash_crear(unsigned capacidad, FuncionHash hash, FuncionPaso paso, FuncionIgualdad igual) {
  ConjHash* tabla = malloc(sizeof(ConjHash));
  tabla->hash = hash;
  tabla->paso = paso;
  tabla->igual = igual;
  tabla->numElems = 0;
  tabla->capacidad = capacidad;
  tabla->tabla = malloc(sizeof(CasillaHash) * capacidad);

  /* se inicializan las casillas con datos nulos */
  for (unsigned idx = 0; idx < capacidad; ++idx) {
    tabla->tabla[idx].clave = NULL;
    tabla->tabla[idx].conj = NULL;
  }

  return tabla;
}

/* Recibe una tabla de conjuntos, una clave y un nuevo conjunto
e inserta el conjunto en la tabla, asociado a la clave dada */
void chash_insertar(ConjHash* tabla, char* clave, Conjunto conj) {
  /* si el factor de carga supera el limite establecido
  se redimensiona la tabla para duplicar la capacidad
  esto significa que la tabla nunca se llena */
  if (tabla->numElems/tabla->capacidad > LIM)
    chash_redimensionar(tabla);

  /* se calcula la posición de la clave dada de acuerdo a la función hash */
  unsigned idx = tabla->hash(clave) % tabla->capacidad;
  /* se recorre la tabla hasta hallar una casilla vacia
  y gracias a la redimension siempre habra lugar */
  for (;tabla->tabla[idx].conj; idx = (idx+tabla->paso(clave))%tabla->capacidad) {
    /* si se halla la misma clave se actualiza el conjunto */
    if (tabla->igual(tabla->tabla[idx].clave, clave)) {
      free(clave);
      conj_destruir(tabla->tabla[idx].conj);
      tabla->tabla[idx].conj = conj;
      return;
    }
  }
  /* si se inserta el conjunto en una casilla nueva
  aumenta el numero total de elementos */
  tabla->numElems++;
  tabla->tabla[idx].clave = clave;
  tabla->tabla[idx].conj = conj;
}

/* Recibe una tabla de conjuntos y una clave
busca el conjunto asociado en la tabla y retorna un puntero al mismo
o en caso de no hallarlo retorna un puntero nulo */
Conjunto chash_buscar(ConjHash* tabla, char* clave) {
  /* se calcula la posición de la clave dada de acuerdo a la función hash */
  unsigned idx = tabla->hash(clave) % tabla->capacidad;
  /* se recorren las posiciones posibles en la tabla
  con la funcion paso para buscar el conjunto
  el valor de la funcion de paso no debe ser divisor del tam
  de la tabla para poder recorrerla toda y no volver al principio */
  for (;tabla->tabla[idx].clave; idx = (idx+tabla->paso(clave))%tabla->capacidad) {
    if (tabla->igual(tabla->tabla[idx].clave, clave))
      return tabla->tabla[idx].conj;
  }
  return NULL;
}

/* Recibe una tabla de conjuntos y duplica su capacidad
luego rehashea todos los elementos para insertarlos
en la nueva tabla */
void chash_redimensionar(ConjHash* tabla) {
  tabla->capacidad *= 2;
  /* se crea e inicializa una tabla nueva
  con el doble de tam de la anterior */
  CasillaHash* tablaNueva = malloc(sizeof(CasillaHash) * tabla->capacidad);
  for (size_t i = 0; i < tabla->capacidad; i++) {
    tablaNueva[i].clave = NULL;
    tablaNueva[i].conj = NULL;
  }

  unsigned idx;
  /* se recorre la tabla original en orden
  hasta haber movido la cantidad de elementos que tenia */
  for (size_t i = 0, j = 0; j < tabla->numElems; i++) {
    if (tabla->tabla[i].clave) {
      j++;
      idx = tabla->hash(tabla->tabla[i].clave) % tabla->capacidad;
      for (;tablaNueva[idx].clave; idx = (idx+tabla->paso(tabla->tabla[i].clave))%tabla->capacidad);
      tablaNueva[idx].clave = tabla->tabla[i].clave;
      tablaNueva[idx].conj = tabla->tabla[i].conj;
    }
  }

  CasillaHash* temp = tabla->tabla;
  tabla->tabla = tablaNueva;
  free(temp);
}

/* Destruye la tabla y todos sus conjuntos */
void chash_destruir(ConjHash* tabla) {
  /* se recorre la tabla en orden hasta eliminar
  la cantidad de conjuntos que tenia */
  for (size_t i = 0, j = 0; j < tabla->numElems; i++) {
    if (tabla->tabla[i].clave) {
      j++;
      free(tabla->tabla[i].clave);
      conj_destruir(tabla->tabla[i].conj);
    }
  }
  free(tabla->tabla);
  free(tabla);
}
