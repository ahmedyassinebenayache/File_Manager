#ifndef CONTIGUE_H_INCLUDED
#define CONTIGUE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// dans le pire des cas on aura chaque bloc ayant une metadonne
#define FB 5
#define NbBloc 50
#define NbBlocmeta 10
#define Nbmeta 50

typedef struct FDmeta FDmeta;
struct FDmeta {
    char FDnom[20];
    int taille;
    int nbEtudiant;
    int adresse;
    int modeglobal; // 0 : contigue, 1 chainee
    int modeinterne; // 0 : non triee, 1 : triee
};

typedef struct Tetudiant Tetudiant;
struct Tetudiant {
    int etat;
    int id;
    char nom[20];
    char prenom[20];
    char sec[20]; // changed to string for section
};

typedef struct BLOC_ch BLOC_ch;
struct BLOC_ch {
    Tetudiant t[FB];
    int ne;
    int next;
};

typedef struct BLOC_meta_ch BLOC_meta_ch;
struct BLOC_meta_ch {
    FDmeta t[FB];
    int ne;
    int next;
};

typedef struct BLOC_meta BLOC_meta;
struct BLOC_meta {
    FDmeta t[FB];
    int ne;
};

typedef struct BLOC_co BLOC_co;
struct BLOC_co {
    Tetudiant t[FB];
    int ne;
};

// Function declarations (implement them in the corresponding .c file)
void Initialize_Disk_Co(FILE *ms);
void update_Allocation_Table_co(FILE *ms, int bloc_adress, int b);
void empty_MS_Co(FILE *ms);
void Compact_Disk_Co(FILE *ms);
int Manage_Storage_Space_Co(FILE *ms, int num_Etudiant);




void creer_un_fichier_co(FILE *ms, FILE *f, char FDnom[20], int taille, int internalmode);// 0 : non triee, 1 : triee

// void creer_un_fichier_co(FILE *ms, FILE *f, char FDnom[20], int taille, 1);    1 : triee
// void creer_un_fichier_co(FILE *ms, FILE *f, char FDnom[20], int taille, 0);    0 : non triee
 


void chargerFichier_co(FILE *ms, FILE *f, char fileName[20]);
void insertion_co(FILE *MS, char file_name[30]);
void Recherche_co(FILE *MS, int id_Tetudiant, int *num_block, int *deplacement);
void Suppression_Enregistrement_logique_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]);
void Suppression_Enregistrement_physic_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]);
void Renommer_co(FILE *ms, char *oldName, char *newName);
void supprime_fichier_contigue(FILE *ms, char nom[20]);
void defragmentation_co(int adr_premierbloc, int nbr_blocs, FILE *ms);

#endif // CONTIGUE_H_INCLUDED
