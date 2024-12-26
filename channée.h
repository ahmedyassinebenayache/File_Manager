#ifndef CHAINEE_H
#define CHAINEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define TAILLE_MAX 100

// Structure definitions
typedef struct Element {
    char nom[TAILLE_MAX];
    struct Element* suivant;
} Element;

typedef struct Liste {
    Element* tete;
} Liste;

// Function prototypes
Liste* creerListe();
Element* creerElement(const char* nom);
void insererElement(Liste* liste, const char* nom);
void afficherListe(const Liste* liste);
void libererListe(Liste* liste);

#endif // CHAINEE_H
