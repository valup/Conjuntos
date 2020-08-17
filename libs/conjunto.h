#ifndef __CONJUNTO_H__
#define __CONJUNTO_H__

typedef struct _Elemento {
  int* dato;
  unsigned tipo : 1;
  struct _Elemento* sig;
} Elemento;

typedef Elemento* Conjunto;

Conjunto conj_vacio();

int conj_es_vacio(Conjunto conj);

Conjunto conj_enteros();

int conj_es_enteros(Conjunto conj);

void conj_destruir(Conjunto conj);

Elemento* crear_elem(int ini, int fin);

void print_elem(Elemento* elem);

void conj_print(Conjunto conj);

int unir_nro(Elemento* elem, int n);

int unir_elem(Elemento* dest, Elemento* fuente);

Elemento* copiar_elem(Elemento* elem);

Elemento* intersecar_elems(Elemento* e1, Elemento* e2);

#endif
