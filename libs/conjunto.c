#include "conjunto.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define min(a,b) (a < b) ? a : b
#define max(a,b) (a > b) ? a : b

/* Crea y devuelve el conjunto intervalo 1:0
que se lee como vacio */
Conjunto conj_vacio() {
  Conjunto vacio = malloc(sizeof(Elemento));
  vacio->tipo = 1;
  vacio->dato = malloc(sizeof(int)*2);
  vacio->dato[0] = 1;
  vacio->dato[1] = 0;
  vacio->sig = NULL;
  return vacio;
}

/* Recibe un conjunto y devuelve si es vacio
lo cual se considera asi cuando el valor final
es menor al inicial */
int conj_es_vacio(Conjunto conj) {
  assert(conj);
  if (conj->tipo == 0)
    return 0;
  return conj->dato[0] > conj->dato[1];
}


/* Crea y devuelve el conjunto intervalo INT_MIN:INT_MAX
es decir el intervalo de todos los enteros representables en C */
Conjunto conj_enteros() {
  Conjunto ent = malloc(sizeof(Elemento));
  ent->dato = malloc(sizeof(int)*2);
  ent->tipo = 1;
  ent->dato[0] = INT_MIN;
  ent->dato[1] = INT_MAX;
  ent->sig = NULL;
  return ent;
}

/* Recibe un conjunto y devuelve si es el de todos
los enteros, es decir si es el conjunto INT_MIN:INT_MAX */
int conj_es_enteros(Conjunto conj) {
  assert(conj);
  if (conj->tipo == 0)
    return 0;
  return (conj->dato[0] == INT_MIN && conj->dato[1] == INT_MAX);
}

/* Recibe un conjunto y libera todos los recursos
utilizados por el */
void conj_destruir(Conjunto conj) {
  while (conj) {
    free(conj->dato);
    Conjunto temp = conj;
    conj = conj->sig;
    free(temp);
  }
}

/* Recibe dos enteros ini y fin y devuelve un elemento
con el intervalo ini:fin si fin es mayor a ini
o uno con solo ini sino */
Elemento* crear_elem(int ini, int fin) {
  Elemento* nuevo = malloc(sizeof(Elemento));
  nuevo->sig = NULL;
  nuevo->tipo = fin > ini;
  nuevo->dato = malloc(sizeof(int) * (nuevo->tipo + 1));
  nuevo->dato[0] = ini;
  if (nuevo->tipo)
    nuevo->dato[1] = fin;
  return nuevo;
}

/* Funcion auxiliar a conj_print que imprime un elemento
asumiendo que no es conjunto vacio
lo cual se chequea previamente */
void print_elem(Elemento* elem) {
  assert(elem);
  printf("%d", elem->dato[0]);
  if (elem->tipo) {
    /* por razones esteticas, cuando un intervalo abarca
    dos valores, se imprimen como separados */
    if (elem->dato[1] == elem->dato[0] + 1) {
      printf(",%d", elem->dato[1]);
    } else {
      printf(":%d", elem->dato[1]);
    }
  }
}

/* Recibe un conjunto y lo recorre imprimiendo sus elementos
o solo "Vacio" si es conjunto vacio */
void conj_print(Conjunto conj) {
  assert(conj);
  if (conj_es_vacio(conj)) {
    printf("Vacio\n");
  } else {
    print_elem(conj);
    for (Conjunto aux  = conj->sig; aux != NULL; aux = aux->sig) {
      printf(",");
      print_elem(aux);
    }
    puts("");
  }
}

/* Recibe un elemento y un entero e intenta unir el entero al elemento
Retorna 1 si se pudo unir o 0 sino */
int unir_nro(Elemento* elem, int n) {
  assert(elem);
  /* si el valor termina antes o empieza despues que el elemento
  y no hay consecutividad, no se puede unir */
  if (elem->dato[0] - n > 1 || n - elem->dato[elem->tipo] > 1)
    return 0;
  /* si el entero pertenece a los valores comprendidos por el elemento
  ya se considera unido y se retorna sin hacer cambios */
  if (n >= elem->dato[0] && n <= elem->dato[elem->tipo])
    return 1;

  /* sino se puede formar un intervalo
  asi que si el elemento no es ya un intervalo se hace lugar */
  if (!elem->tipo)
    elem->dato = realloc(elem->dato, sizeof(int) * 2);

  elem->dato[0] = min(n, elem->dato[0]);
  elem->dato[1] = max(n, elem->dato[elem->tipo]);
  elem->tipo = 1;
  return 1;
}

/* Recibe dos elementos, destino y fuente
e intenta unirlos guardando el resultado en el destino
Retorna 1 si se pudo unir o 0 sino */
int unir_elem(Elemento* dest, Elemento* fuente) {
  if (!fuente)
    return 0;
  /* si la fuente termina antes o empieza despues que el destino
  y no hay consecutividad, no se pueden unir */
  if (dest->dato[0]-fuente->dato[fuente->tipo]>1 || fuente->dato[0]-dest->dato[dest->tipo]>1)
    return 0;
  /* si la fuente pertenece a los valores comprendidos por el destino
  ya se considera unido y se retorna sin hacer cambios */
  if (dest->dato[0]<=fuente->dato[0] && dest->dato[dest->tipo]>=fuente->dato[fuente->tipo])
    return 1;
  /* sino se puede formar un intervalo
  asi que si el destino no es ya un intervalo se hace lugar */
  if (!dest->tipo)
    dest->dato = realloc(dest->dato, sizeof(int) * 2);
  /* la union se forma con el menor inicio y el mayor final */
  dest->dato[0] = min(dest->dato[0], fuente->dato[0]);
  dest->dato[1] = max(dest->dato[dest->tipo], fuente->dato[fuente->tipo]);
  dest->tipo = 1;
  return 1;
}

/* Recibe un elemento y devuelve uno nuevo con los mismos valores */
Elemento* copiar_elem(Elemento* elem) {
  Elemento* copia = malloc(sizeof(Elemento));
  copia->tipo = elem->tipo;
  copia->dato = malloc(sizeof(int) * (copia->tipo + 1));
  copia->dato[0] = elem->dato[0];
  if (copia->tipo)
    copia->dato[1] = elem->dato[1];
  copia->sig = NULL;
  return copia;
}

/* Recibe dos elementos y devuelve uno nuevo
con los valores de la interseccion entre ellos */
Elemento* intersecar_elems(Elemento* e1, Elemento* e2) {
  if (!e1 || !e2)
    return NULL;
  /* si ninguno de los elementos termina antes
  de que empiece el otro entonces hay interseccion */
  if (e1->dato[e1->tipo] >= e2->dato[0] && e2->dato[e2->tipo] >= e1->dato[0]) {
    Elemento* nuevo = malloc(sizeof(Elemento));
    /* si alguno de los elementos tiene un solo valor
    o coinciden en un solo valor la interseccion
    sera un elemento individual (tipo 0) */
    if (!e1->tipo || !e2->tipo || e1->dato[e1->tipo] == e2->dato[0] || e2->dato[e2->tipo] == e1->dato[0]) {
      nuevo->tipo = 0;
      nuevo->dato = malloc(sizeof(int));
      /* se chequea cual es el valor de la interseccion */
      if (!e2->tipo || e1->dato[e1->tipo] == e2->dato[0]) {
        *nuevo->dato = e2->dato[0];
      } else {
        *nuevo->dato = e1->dato[0];
      }
    /* si ambos elementos son intervalos y coinciden en varios valores
    la interseccion es un intervalo */
    } else {
      nuevo->tipo = 1;
      nuevo->dato = malloc(sizeof(int) * 2);
      /* la interseccion se forma con el mayor comienzo y el menor final */
      nuevo->dato[0] = max(e1->dato[0], e2->dato[0]);
      nuevo->dato[1] = min(e1->dato[1], e2->dato[1]);
    }
    nuevo->sig = NULL;
    return nuevo;
  }
  return NULL;
}
