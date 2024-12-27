#ifndef CHAINEE_H
#define CHAINEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

typedef struct BLOC_ch BLOC_ch;
struct BLOC_ch
{
    Tetudiant t[FB];
    int ne;
    int next ;
};
typedef struct BLOC_meta BLOC_meta;
struct BLOC_meta
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

// Function prototypes
void Initialize_Disk_Ch(FILE *ms);
void update_Allocation_Table(FILE *ms,int bloc_adress , int b);
void empty_MS_Ch(FILE *ms);
int Manage_Storage_Space_Ch(FILE *ms ,int num_Etudiant);
 // CHAINEE_H


void Creer_du_fichier_triee_chainee(FILE *ms ,FILE *f,char nom[20],int nbEtudiant);
void creer_un_fichier_chainee_non_triee(FILE *ms ,FILE *f,char nom[20],int nbEtudiant);




Position Recherche_Sur_un_fichier_chainee_triee(int ID,FILE *ms,char nom[20]);
void recherche_fichier_chainee_non_triee(FILE *ms, char nom[20], int id, int p[2], FILE *f);




void ajouter_etudiant_fichier_chainee_non_triee(FILE *ms,FILE *f ,char nom[20]);
void insertion_dans_un_fichier_Triee(FILE *ms, FDmeta m, Tetudiant x);



void chargement_fichier_chainee(FILE *ms,FILE *f,char nom[20]);
void suppression_physique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);
void suppression_logique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);
void renomer_fichier_chainee(FILE *ms, char nom[20], char nouveaunom[20]);
void supprime_fichier_chainee(FILE *ms, char nom[20]);
void defragmentation_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);
#endif
