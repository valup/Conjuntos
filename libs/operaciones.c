#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "operaciones.h"
#include "conjunto.h"
#include "straux.h"

/* Recibe un conjunto y copia sus valores a uno nuevo */
Conjunto conj_copiar(Conjunto conj) {
  if (!conj)
    return NULL;
  Conjunto copia = copiar_elem(conj);
  for(Conjunto aux = conj->sig, auxC = copia; aux; aux = aux->sig, auxC = auxC->sig) {
    auxC->sig = copiar_elem(aux);
  }
  return copia;
}

/* Recibe un conjunto y un puntero a un entero
y coloca el puntero en el conjunto manteniendo el orden menor a mayor */
Conjunto conj_agregar_orden(Conjunto conj, int* n) {
  if (!conj || *conj->dato - *n > 1) {
    /* si no hay otros elementos o es menor al primer elemento
    con distancia mayor a 1, se coloca al principio */
    Elemento* nuevo = malloc(sizeof(Elemento));
    nuevo->tipo = 0;
    nuevo->dato = n;
    nuevo->sig = conj;
    return nuevo;
  } else if (unir_nro(conj, *n)) {
    /* si se pudo unir al elemento
    (es igual / pertenece al intervalo / crea o expande un intervalo)
    se libera el puntero */
    /* vale mencionar que se consideran 2 numeros consecutivos
    como un intervalo para facilitar futuras operaciones */
    free(n);
  } else {
    /* si no se pudo lo anterior se sigue buscando
    en la posicion siguiente */
    conj->sig = conj_agregar_orden(conj->sig, n);
  }

  /* se chequea si la adicion del nuevo valor
  puede unir dos elementos del conjunto en un solo intervalo */
  if (conj->sig && unir_elem(conj, conj->sig)) {
    Conjunto temp = conj->sig;
    conj->sig = temp->sig;
    free(temp->dato);
    free(temp);
  }

  return conj;
}

/* Recibe un string, lo interpreta como conjunto definido
por comprension (intervalo), y retorna dicho conjunto formado */
Conjunto conj_comprension(char* com) {
  long start, end;
  int args = sscanf(com, "{x : %ld <= x <= %ld}", &start, &end);
  Conjunto conj = NULL;
  /* se chequea que los valores se hayan obtenido correctamente
  y sean enteros validos */
  if (args == 2 && start <= INT_MAX && start >= INT_MIN && end <= INT_MAX && end >= INT_MIN) {
    conj = malloc(sizeof(Elemento));
    conj->tipo = 1;
    conj->dato = malloc(sizeof(int) * 2);
    conj->dato[0] = (int) start;
    conj->dato[1] = (int) end;
    conj->sig = NULL;
  }

  return conj;
}

/* Recibe un string, lo interpreta como conjunto definido
por extension (valores separados), y retorna dicho conjunto formado */
Conjunto conj_extension(char* com) {
  Conjunto conj = NULL;
  /* se eliminan los corchetes para luego
  realizar el escaneo de los elementos */
  sscanf(com, "{%[^}]s", com);
  char* token = strtok(com, ",");
  /* se repite el proceso hasta no leer mas elementos */
  while (token) {
    /* se intenta convertir el token en un entero */
    int* n = string_to_int(strim(token));
    /* si hubo algun error se considera invalido el conjunto */
    if (!n) {
      conj_destruir(conj);
      return NULL;
    }
    /* se agrega el nuevo valor al conjunto en orden menor a mayor */
    conj = conj_agregar_orden(conj, n);
    token = strtok(NULL, ",");
  }

  return conj;
}

/* Recibe una tabla de conjuntos, un string correspondiente al nombre
del nuevo conjunto y otro string con la definicion de dicho conjunto
e intenta crear un conjunto segun la definicion para agregarlo a la tabla
o si no es posible se libera el nombre */
void conj_nuevo(ConjHash* conjs, char* nom, char* com) {
  Conjunto conj;
  /* los conjuntos por extension no deberian incluir
  la letra 'x' asi que si esta se asume que es por comprension */
  if (strchr(com,'x')) {
    conj = conj_comprension(com);
  } else {
    conj = conj_extension(com);
  }
  /* si el conjunto es NULL hubo error
  sino se inserta */
  if (conj) {
    chash_insertar(conjs, nom, conj);
    printf("\nConjunto creado exitosamente\n\n");
  } else {
    printf("\nERROR: conjunto invalido\n\n");
    free(nom);
  }
}

/* Recibe dos conjuntos y devuelve uno nuevo con la union de ambos */
Conjunto conj_union(Conjunto c1, Conjunto c2) {
  if (!c1)
    return conj_copiar(c2);
  if (!c2)
    return conj_copiar(c2);
  /* se copia el primer elemento del primer conjunto al nuevo conjunto
  asumiendo que empieza antes que el segundo */
  Conjunto nuevo = copiar_elem(c1), aux = nuevo;
  c1 = c1->sig;

  while (1) {
    /* se intenta unir el siguiente elemento del segundo conjunto
    al nuevo elemento */
    if (!unir_elem(aux, c2)) {
      /* si se habia terminado de recorrer c1
      se copia el resto de c2 */
      if (!c1) {
        aux->sig = conj_copiar(c2);
      /* si no se puede se sigue al siguiente nuevo elemento
      con el conjunto que empieza antes primero */
      } else if (c1->dato[0] < c2->dato[0]) {
        aux->sig = conj_union(c1, c2);
      } else {
        aux->sig = conj_union(c2, c1);
      }
      return nuevo;
    }
    /* si no hay mas que unir de c1 se copia el resto de c2 */
    if (!c1) {
      aux->sig = conj_copiar(c2->sig);
      return nuevo;
    }
    /* se intercambian los punteros para repetir el proceso
    con el otro conjunto */
    Conjunto temp = c1;
    c1 = c2->sig;
    c2 = temp;
  }
}

/* Recibe una tabla de conjuntos, un string correspondiente al nombre
del nuevo conjunto y otro string con la definicion de una union entre conjuntos
e intenta crear un conjunto segun la definicion para agregarlo a la tabla
o si no es posible se libera el nombre */
void unir(ConjHash* conjs, char* nom, char* com) {
  char* nom1 = strtok(com, "|");
  char* nom2 = strtok(NULL, "\n");
  if (!nom1 || !nom2 || (nom1 = strim_der(nom1))[0] == '\0' || (nom2 = strim_izq(nom2))[0] == '\0') {
    printf("\nERROR: nombre vacio\n\n");
    free(nom);
    return;
  }

  Conjunto c1 = chash_buscar(conjs, nom1);
  if (!c1) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom1);
    free(nom);
    return;
  }
  Conjunto c2 = chash_buscar(conjs, nom2);
  if (!c2) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom2);
    free(nom);
    return;
  }

  /* si alguno de los conjuntos es los enteros, la union tambien */
  if (conj_es_enteros(c1) || conj_es_enteros(c2)) {
    chash_insertar(conjs, nom, conj_vacio());
  /* si alguno de los conjuntos es vacio, la union es el otro */
  } else if (conj_es_vacio(c1)) {
    chash_insertar(conjs, nom, conj_copiar(c2));
  } else if (conj_es_vacio(c2)) {
    chash_insertar(conjs, nom, conj_copiar(c1));
  /* para calcular la union se pone primero
  el conjunto que empieza antes */
  } else if (c1->dato[0] < c2->dato[0]) {
    chash_insertar(conjs, nom, conj_union(c1, c2));
  } else {
    chash_insertar(conjs, nom, conj_union(c2, c1));
  }

  printf("\nConjunto creado exitosamente\n\n");
}

/* Recibe dos conjuntos y devuelve uno nuevo con la interseccion de ambos */
Conjunto conj_inter(Conjunto c1, Conjunto c2) {
  Conjunto nuevo = NULL;

  /* se intenta intersecar elementos hasta obtener la primera interseccion
  o llegar al final de uno de los conjuntos */
  while (!(nuevo = intersecar_elems(c1, c2))) {
    /* se recorre el primer conjunto mientras tenga elementos
    y sea menor al comienzo del segundo */
    for (;c1 && c1->dato[c1->tipo] < c2->dato[0]; c1 = c1->sig);
    /* si se recorrio todo el conjunto no hay interseccion */
    if (!c1)
      return conj_vacio();
    /* se intercambian los punteros para repetir el proceso
    con el otro conjunto */
    Conjunto temp = c1;
    c1 = c2;
    c2 = temp;
  }
  /* si se llega al final de alguno de los conjuntos
  no hay mas intersecciones */
  if (!c1 || !c2)
    return nuevo;
  /* si el elemento del primer conjunto sigue despues del elemento del segundo
  y el segundo tiene mas elementos, puede haber otra interseccion */
  if (c1->dato[c1->tipo] > c2->dato[c2->tipo] && c2->sig) {
    nuevo->sig = conj_inter(c1, c2->sig);
  /* sino se verifica lo mismo de la otra forma */
} else if (c1->dato[c1->tipo] <= c2->dato[c2->tipo] && c1->sig) {
    nuevo->sig = conj_inter(c2, c1->sig);
  }
  /* si no se hallaron mas intersecciones y retorno vacio
  se elimina esto y se deja en NULL */
  if (nuevo->sig && conj_es_vacio(nuevo->sig)) {
    conj_destruir(nuevo->sig);
    nuevo->sig = NULL;
  }

  return nuevo;
}

/* Recibe una tabla de conjuntos, un string correspondiente al nombre
del nuevo conjunto y otro string con la definicion de una interseccion entre conjuntos
e intenta crear un conjunto segun la definicion para agregarlo a la tabla
o si no es posible se libera el nombre */
void intersecar(ConjHash* conjs, char* nom, char* com) {
  char* nom1 = strtok(com, "&");
  char* nom2 = strtok(NULL, "\n");
  if (!nom1 || !nom2 || (nom1 = strim_der(nom1))[0] == '\0' || (nom2 = strim_izq(nom2))[0] == '\0') {
    printf("\nERROR: nombre vacio\n\n");
    free(nom);
    return;
  }

  Conjunto c1 = chash_buscar(conjs, nom1);
  if (!c1) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom1);
    free(nom);
    return;
  }
  Conjunto c2 = chash_buscar(conjs, nom2);
  if (!c2) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom2);
    free(nom);
    return;
  }

  /* si alguno de los conjuntos es vacio, la interseccion tambien */
  if (conj_es_vacio(c1) || conj_es_vacio(c2)) {
    chash_insertar(conjs, nom, conj_vacio());
  /* si alguno de los conjuntos es los enteros
  la interseccion es el otro */
  } else if (conj_es_enteros(c1)) {
    chash_insertar(conjs, nom, conj_copiar(c2));
  } else if (conj_es_enteros(c2)) {
    chash_insertar(conjs, nom, conj_copiar(c1));
  /* para calcular la interseccion se pone primero
  el conjunto que empieza antes */
  } else if (c1->dato[0] < c2->dato[0]) {
    chash_insertar(conjs, nom, conj_inter(c1, c2));
  } else {
    chash_insertar(conjs, nom, conj_inter(c2, c1));
  }

  printf("\nConjunto creado exitosamente\n\n");
}

/* Recibe dos conjuntos y devuelve uno nuevo restando el segundo al primero */
Conjunto conj_resta(Conjunto c1, Conjunto c2) {
  Conjunto nuevo = NULL, aux;
  /* ini se utiliza para guardar el proximo inicio de un intervalo */
  int ini = INT_MIN;
  /* como c1 es el conjunto al que se le resta, se lo recorre completamente */
  for (;c1; c1 = c1->sig) {
    /* si no hay elementos que se puedan restar, se copia el resto de c1 */
    if (!c2) {
      if (!nuevo)
        return conj_copiar(c1);
      aux->sig = conj_copiar(c1);
      return nuevo;
    }
    /* se recorre c2 mientras sus elementos
    terminen antes del comienzo de c1 */
    for(;c2 && c2->dato[c2->tipo] < c1->dato[0]; c2 = c2->sig);
    /* si el proximo elemento de c2 no tiene interseccion con c1
    es decir que no empieza despues de que termine
    se copia el elemento de c1 */
    if (c2->dato[0] > c1->dato[c1->tipo]) {
      if (!nuevo) {
        nuevo = copiar_elem(c1);
        aux = nuevo;
      } else {
        aux->sig = copiar_elem(c1);
        aux = aux->sig;
      }
    } else {
      /* se restan los elementos de c2 mientras tengan interseccion con c1
      pero si la interseccion es total se sigue de largo para ignorar
      el elemento actual de c1 ya que se elimina por completo */
      for (;c2 && c2->dato[0] <= c1->dato[c1->tipo] && (c2->dato[0] > c1->dato[0] || c2->dato[c2->tipo] < c1->dato[c1->tipo]); c2 = c2->sig) {
        /* si c1 empieza antes que c2 ya hay un elemento que crear */
        if (c2->dato[0] > c1->dato[0]) {
          /* si el proximo inicio es menor al inicio de c1
          el nuevo inicio es el de c1 */
          if (ini < c1->dato[0])
            ini = c1->dato[0];
          /* el nuevo elemento abarca desde ini hasta antes de c2 */
          if (!nuevo) {
            nuevo = crear_elem(ini, c2->dato[0] - 1);
            aux = nuevo;
          } else {
            aux->sig = crear_elem(ini, c2->dato[0] - 1);
            aux = aux->sig;
          }
        }
        /* en cualquier caso se toma el consecutivo al final de c2
        como proximo comienzo */
        ini = c2->dato[c2->tipo] + 1;
        /* y si c2 cubre el resto de los valores de c1
        se termina el loop */
        if (c2->dato[c2->tipo] >= c1->dato[c1->tipo])
          break;
      }
      /* si al terminar el loop ini queda entre los valores de c1
      significa que hay otro elemento que crear entre ini y el final de c1 */
      if (ini >= c1->dato[0] && ini <= c1->dato[c1->tipo]) {
        if (!nuevo) {
          nuevo = crear_elem(ini, c1->dato[c1->tipo]);
          aux = nuevo;
        } else if (aux->dato[aux->tipo] < ini){
          aux->sig = crear_elem(ini, c1->dato[c1->tipo]);
          aux = aux->sig;
        }
      }
    }
  }
  /* si al terminar todo el recorrido no se crearon elementos
  el resultado es vacio */
  if (!nuevo)
    return conj_vacio();
  return nuevo;
}

/* Recibe una tabla de conjuntos, un string correspondiente al nombre
del nuevo conjunto y otro string con la definicion de una resta entre conjuntos
e intenta crear un conjunto segun la definicion para agregarlo a la tabla
o si no es posible se libera el nombre */
void restar(ConjHash* conjs, char* nom, char* com) {
  char* nom1 = strtok(com, "-");
  char* nom2 = strtok(NULL, "\n");
  if (!nom1 || !nom2 || (nom1 = strim_der(nom1))[0] == '\0' || (nom2 = strim_izq(nom2))[0] == '\0') {
    printf("\nERROR: nombre vacio\n\n");
    free(nom);
    return;
  }

  Conjunto c1 = chash_buscar(conjs, nom1);
  if (!c1) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom1);
    free(nom);
    return;
  }

  Conjunto c2 = chash_buscar(conjs, nom2);
  if (!c2) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom2);
    free(nom);
    return;
  }

  if (conj_es_vacio(c1) || conj_es_enteros(c2)) {
    chash_insertar(conjs, nom, conj_vacio());
  } else if (conj_es_vacio(c2)) {
    chash_insertar(conjs, nom, conj_copiar(c1));
  } else {
    chash_insertar(conjs, nom, conj_resta(c1, c2));
  }

  printf("\nConjunto creado exitosamente\n\n");
}

/* Recibe una tabla de conjuntos, un string correspondiente al nombre
del nuevo conjunto y otro string con la definicion del complemento de un conjunto
e intenta crear un conjunto segun la definicion para agregarlo a la tabla
o si no es posible se libera el nombre */
void complemento(ConjHash* conjs, char* nom, char* com) {
  char* nom1 = strim_izq(com + 1);
  if (nom1[0] == '\0') {
    printf("\nERROR: nombre vacio\n\n");
    free(nom);
    return;
  }

  Conjunto c = chash_buscar(conjs, nom1);
  if (!c) {
    printf("\nERROR: no existe el conjunto %s\n\n", nom1);
    free(nom);
    return;
  }

  /* si el conjunto es vacio su complemento son los enteros */
  if (conj_es_vacio(c)) {
    chash_insertar(conjs, nom, conj_enteros());
  /* y viceversa */
  } else if (conj_es_enteros(c)) {
    chash_insertar(conjs, nom, conj_vacio());
  /* el complemento de un conjunto equivale a restar el conjunto al
  conjunto de enteros (~A = Z-A) */
  } else {
    Conjunto enteros = conj_enteros();
    chash_insertar(conjs, nom, conj_resta(enteros, c));
    conj_destruir(enteros);
  }

  printf("\nConjunto creado exitosamente\n\n");
}
