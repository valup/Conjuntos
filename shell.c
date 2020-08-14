#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "libs/chash.h"
#include "libs/operaciones.h"
#include "libs/straux.h"

#define MAX_STDIN 100

/* Funcion de hasheo de forma rolling hash polinomial
explicada en el informe */
unsigned hash(char* str) {
  unsigned p = 97, total = 0;
  for (size_t i = 0; i < strlen(str); i++) {
    total += str[i] * pow(p, i);
  }
  return total;
}

/* Funcion de paso para hashing doble
que retorna el valor ASCII de la primera letra
de la clave */
unsigned paso(char* str) {
  return (unsigned) str[0];
}

/* Compara claves string */
int iguales(char* clave1, char* clave2) {
  return !strcmp(clave1, clave2);
}

/* Imprime informacion sobre los comandos aceptados por el interprete */
void help() {
    printf("\nComandos:\n");
    printf("  alias = {x1,x2,...,xn}:\n");
    printf("    Crea el conjunto alias de valores enteros individuales x1,x2,...,xn\n");
    printf("  alias = {x: a <= x <= b}:\n");
    printf("    Crea el conjunto alias que comprende todos los enteros entre a y b\n");
    printf("  alias = a1 | a2:\n");
    printf("    Crea el conjunto alias como union de los conjuntos a1 y a2\n");
    printf("  alias = a1 & a2:\n");
    printf("    Crea el conjunto alias como interseccion de los conjuntos a1 y a2\n");
    printf("  alias = a1 - a2:\n");
    printf("    Crea el conjunto alias restando el conjunto a2 al conjunto a1\n");
    printf("  alias = ~ a1:\n");
    printf("    Crea el conjunto alias como complemento del conjunto a1\n");
    printf("  imprimir alias:\n");
    printf("    Imprime los valores del conjunto alias\n");
    printf("  salir:\n");
    printf("    Cierra el interprete\n\n");
    printf("Valores aceptados: enteros entre %d y %d\n", INT_MIN, INT_MAX);
    printf("  Otros valores no son representables y no seran aceptados\n\n");
}

/* Recibe una tabla de conjuntos y un string buffer de comando
y procesa comandos que no crean nuevos conjuntos */
int otro_com(ConjHash* conjs, char* buf) {
  if (!strcmp(buf, "salir")) {
    chash_destruir(conjs);
    return 0;
  }
  if (!strcmp(buf, "help")) {
    help();
  } else {
    char* com = strtok(buf, " ");
    if (com && !strcmp(com, "imprimir")) {
      char* nom = strtok(NULL, "\n");
      if (!nom || (nom = strim_izq(nom))[0] == '\0') {
        printf("\nERROR: nombre vacio\n\n");
      } else {
        Conjunto conj = chash_buscar(conjs, nom);
        if (conj) {
          puts("");
          conj_print(conj);
          puts("");
        } else {
          printf("\nERROR: no existe el conjunto %s\n\n", nom);
        }
      }
    } else {
      printf("\nERROR: comando invalido\n\n");
    }
  }
  return 1;
}

/* Recibe una tabla de conjuntos y un string buffer
y procesa el comando guardado en el buffer */
int procesar(ConjHash* conjs, char* buf) {
  if (strchr(buf, '=')) {
    char *nomBuf = strtok(buf, "=");
    if (!nomBuf || (nomBuf = strim_der(nomBuf))[0] == '\0') {
      printf("\nERROR: nombre vacio\n\n");
      return 1;
    }
    char *nom = malloc(sizeof(char)*(strlen(nomBuf)+1));
    strcpy(nom, nomBuf);

    char* com = strtok(NULL, "\n");
    if (!com || (com = strim_izq(com))[0] == '\0') {
      printf("\nERROR: comando vacio\n\n");
      return 1;
    }

    if (com[0] == '{' && com[strlen(com)-1] == '}') {
      conj_nuevo(conjs, nom, com);
    } else if (strchr(com, '|')) {
      unir(conjs, nom, com);
    } else if (strchr(com, '&')) {
      intersecar(conjs, nom, com);
    } else if (strchr(com, '-')) {
      restar(conjs, nom, com);
    } else if (com[0] == '~') {
      complemento(conjs, nom, com);
    } else {
      printf("\nERROR: comando invalido\n\n");
    }

    return 1;
  }

  return otro_com(conjs, buf);
}

int main() {
  printf("\nIngrese 'help' para informacion sobre los comandos y valores aceptados\n\n");

  ConjHash* conjs = chash_crear(TAM_INI, hash, paso, iguales);

  int continuar = 1;
  char buf[MAX_STDIN];

  while (continuar) {
    printf(">> ");
    fgets(buf, MAX_STDIN - 1, stdin);
    /* si no se guardo el salto de linea en el buffer
    significa que elcomando es demasiado largo */
    if (!strchr(buf, '\n')) {
      printf("\nERROR: comando demasiado largo\n\n");
      /* en este caso se debe seguir escaneando hasta vaciar stdin */
      for(fgets(buf, MAX_STDIN - 1, stdin); !strchr(buf, '\n'); fgets(buf, MAX_STDIN - 1, stdin));
    } else {
      /* si se guardo el salto de linea se lo elimina del buffer
      antes de procesar */
      buf[strlen(buf)-1] = '\0';
      if (strim(buf)[0] != '\0')
        continuar = procesar(conjs, strim(buf));
    }
  }

  return 0;
}
