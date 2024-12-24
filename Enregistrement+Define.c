#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// dans le pire des cas on aura chaque bloc ayant une metadonne
#define FB 5
#define NbBloc 50
#define NbBlocmeta 10
#define Nbmeta 50

typedef struct Tetudiant Tetudiant;
struct Tetudiant
{
    int etat ; 
    int id;
    char nom[20];
    char prenom[20];
    char sec ;
};

typedef struct BLOC_ch BLOC_ch;
struct BLOC_ch
{
    Tetudiant t[FB];
    int ne;
    int next ;
};
typedef struct BLOC_meta_ch BLOC_meta_ch;
struct BLOC_meta_ch
{
    FDmeta t[FB];
    int ne;
    int next ;    
};

typedef struct BLOC_meta_co BLOC_meta_co;
struct BLOC_meta_co
{
    FDmeta t[FB];
    int ne; 
};

typedef struct BLOC_co BLOC_co;
struct BLOC_co
{
    Tetudiant t[FB];
    int ne;
};
typedef struct FDmeta FDmeta;
struct FDmeta
{
    char FDnom[20];
    int taille;
    int nbEtudiant;
    int adresse ;
    int modeglobal ; // 0 : contigue, 1 chainee
    int modeinterne ; // 0 : non triee , 1 : triee

};
