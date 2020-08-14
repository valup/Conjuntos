#ifndef __CONJ_H__
#define __CONJ_H__

#include "chash.h"

void conj_nuevo(ConjHash* conjs, char* nom, char* com);

void unir(ConjHash* conjs, char* nom, char* com);

void intersecar(ConjHash* conjs, char* nom, char* com);

void restar(ConjHash* conjs, char* nom, char* com);

void complemento(ConjHash* conjs, char* nom, char* com);

#endif
