#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FB 5
#define MAX 50
#define BM 10 //Nombre de blocsMETA






typedef struct Meta{
    int etat; // 0:supprime , 1:existe
    char fname[20];
    int blocnum;
    int structnum;
    int firstbloc;
    int Globalmode;// 0 : contigue, 1 chainee
    int internalmode;//0 : non triée, 1  : triée
}Meta;


typedef struct BLOCMETA{
    Meta V[FB];
    int nf; //nombre de ficchier de donnees
}BLOCMETA;


typedef struct eleve{
    int etat; // 0:supprime , 1:existe
    int id;
    char nom[30];
    char Prenom[20];
    char sec;
}eleve;



typedef struct BLOC_co{//pour le mode contigue
    eleve T[FB];
    int ne;//nombre d'element
}BLOC_co;



typedef struct BLOC_ch{//pour le mode chainee
    eleve T[FB];
    int next;
    int ne; //nombre d'element
}BLOC_ch;



