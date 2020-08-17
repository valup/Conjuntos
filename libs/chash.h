#ifndef __CHASH_H__
#define __CHASH_H__

#include "conjunto.h"

#define LIM 0.7
#define TAM_INI 503

typedef unsigned (*FuncionHash)(char* clave);

typedef unsigned (*FuncionPaso)(char* clave);

typedef int (*FuncionIgualdad)(char* clave1, char* clave2);

typedef struct _CasillaHash {
  char* clave;
  Conjunto conj;
} CasillaHash;

typedef struct {
  CasillaHash* tabla;
  unsigned numElems;
  unsigned capacidad;
  FuncionHash hash;
  FuncionPaso paso;
  FuncionIgualdad igual;
} ConjHash;

ConjHash* chash_crear(unsigned capacidad, FuncionHash hash, FuncionPaso paso, FuncionIgualdad igual);

void chash_insertar(ConjHash* tabla, char* clave, Conjunto conj);

Conjunto chash_buscar(ConjHash* tabla, char* clave);

void chash_redimensionar(ConjHash* tabla);

void chash_destruir(ConjHash* tabla);

#endif
