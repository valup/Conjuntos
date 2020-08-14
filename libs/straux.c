#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "straux.h"

/* Recibe un string e intenta convertirlo en int
inicialmente convirtiendolo en long y chequeando que el resultado sea valido
en cuyo caso se guarda en un puntero y se retorna
sino se retorna NULL */
int* string_to_int(char* str) {
  // el puntero endp indica hasta donde se pudo convertir str
  char* endp = NULL;
  // se convierte en base 10
  long n = strtol(str, &endp, 10);
  /* si el puntero sigue al principio o no llego a /0
  (es decir que no se pudo convertir todo el string)
  o si el resultado esta fuera del rango representable por enteros
  no es valido y se retorna NULL */
  if (endp == str || *endp != '\0' || n < INT_MIN || n > INT_MAX)
    return NULL;
  int* m = malloc(sizeof(int));
  *m = (int) n;
  return m;
}

/* Recibe un string y lo devuelve un puntero
al primer caracter no espacio */
char* strim_izq(char* str) {
  for (;str[0] == ' '; str++);
  return str;
}

/* Recibe un string y elimina los espacios luego del ultimo
caracter no espacio */
char* strim_der(char* str) {
  size_t i = strlen(str);
  for (;i > 0 && str[i-1] == ' '; i--);
  str[i] = '\0';
  return str;
}

// Recibe un string y remueve los espacios de ambos extremos
char* strim(char* str) {
  return strim_der(strim_izq(str));
}
