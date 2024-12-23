#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// dans le pire des cas on aura chaque bloc ayant une metadonne
#define FB 5
#define NbBloc 50
#define NbBlocmeta_co 10
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























/** 
 * Fonction pour rechercher un groupe de blocs libres dans la table d'allocation
 */
void allouer_co(int *start, int tableAllocation[NbBloc], int nbEtudiant) {
    int freeBlocks = 0;
    *start = -1;
    for (int i = 0; i < NbBloc; i++) {
        if (tableAllocation[i] == 0) { // Bloc non utilisé
            if (freeBlocks == 0) {*start = i; }// Début de la séquence libre
            freeBlocks++;
        } else {
            freeBlocks = 0; // Réinitialisation si un bloc est utilisé
        }

        if (freeBlocks == nbEtudiant) break; // Séquence suffisante trouvée
    }

    if (freeBlocks < nbEtudiant) {
        *start = -1; // Pas assez de blocs libres
    }
}






/** 
 * Fonction pour trier les élèves par ID (tri à bulles)
 */
void trierTetudiants(Tetudiant *tTetudiant, int taille) {
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (tTetudiant[j].id > tTetudiant[j + 1].id) {
                Tetudiant temp = tTetudiant[j];
                tTetudiant[j] = tTetudiant[j + 1];
                tTetudiant[j + 1] = temp;
            }
        }
    }
}







 void Charger_les_élèves_non_triée_dans_fichier_de_donnée(FILE *f, char fileName[20], int startBloc, int nbEtudiant, int taille) {
    Tetudiant record;
    BLOC_co Buferr;
    int recordIndex = 0;

    rewind(f) ;
    printf("Entrez les ineormations des élèves (ID, Nom, Prénom, Section):\n");
    for (int i = 0; i < nbEtudiant; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < taille; j++) {
            printf("Élève %d:\n", recordIndex + 1);
            record.etat = 1; // Élève existant
            printf("ID: ");
            scane("%d", &record.id);
            printf("Nom: ");
            scane(" %[^\n]", record.nom);
            printf("Prénom: ");
            scane(" %[^\n]", record.prenom);
            printf("Section: ");
            scane(" %c", &record.sec);

            Buferr.t[j] = record;
            Buferr.ne++;
            recordIndex++;
        }

        // Écriture du bloc dans fichier de donnée
      
        fwrite(&Buferr, sizeof(Buferr), 1, f);
    } }










/**
 * Fonction pour charger les élèves en mode trié
 */
void Charger_les_élèves_triée_dans_fichier_de_donnée(FILE *f, char fileName[20], int startBloc, int nbEtudiant, int taille) {
    Tetudiant *tTetudiant = malloc(taille * sizeof(Tetudiant));
    if (tTetudiant == NULL) {
        printf("Erreur : Impossible d'allouer de la mémoire.\n");
        return;
    }

    printf("Entrez les ineormations des élèves (ID, Nom, Prénom, Section):\n");
    for (int i = 0; i < taille; i++) {
        tTetudiant[i].etat = 1; // Élève existant
        printf("Élève %d:\n", i + 1);
        printf("ID: ");
        scane("%d", &tTetudiant[i].id);
        printf("Nom: ");
        scane(" %[^\n]", tTetudiant[i].nom);
        printf("Prénom: ");
        scane(" %[^\n]", tTetudiant[i].prenom);
        printf("Section: ");
        scane(" %c", &tTetudiant[i].sec);
    }

    // tri des élèves
    trierTetudiants(tTetudiant, taille);

    // Chargement des élèves dans les blocs
    BLOC_co Buferr;
    int recordIndex = 0;
    
    rewind(f) ;
    for (int i = 0; i < nbEtudiant; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < taille; j++) {
            Buferr.t[j] = tTetudiant[recordIndex++];
            Buferr.ne++;
        }

        // Écriture du bloc dans dans fichier de donnée
        
        fwrite(&Buferr, sizeof(Buferr), 1, f);
    }

    
    free(tTetudiant);
}











void creer_un_fichier(FILE *ms, FILE *f, char fname[20], int taille,  int internalmode) {//0 : non triée,   1  : triée
    FDmeta fileMeta;
    BLOC_meta_co BuferrMeta;
     int tableAllocation[NbBloc];
  
    
    
   
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    int trouv=0,k,i,taille ;int startBlock;
    // on cherche la une metadonne vide dans la MS 
    while(!trouv){
    fread(&BuferrMeta,sizeof(BLOC_co),1,ms); 
    for ( i = 0; i < FB; i++)
    {
        if (BuferrMeta.t[i].taille==0 ){
            trouv = 1 ;


            // Écrit les métadonnées du fichier dans  une meta variable
   
             fileMeta.nbEtudiant = ceil((double)taille / FB);// Calculer le nombre de blocs requis
             fileMeta.taille = taille;
             fileMeta.modeglobal = 0;
             fileMeta.modeinterne=internalmode;

             strcpy(fileMeta.FDnom,fname);

  
            //trouver des  BLOCS_co vides Pour premier bloc
            
                allouer_co(&startBlock, tableAllocation, fileMeta.nbEtudiant);
               if (startBlock == -1) {
               printf("Erreur : Pas assez d'espace libre pour charger le fichier.\n");
               return;
                }

             fileMeta.adresse = startBlock;
            

              // Écrit les métadonnées du fichier dans le ms fichier


             BuferrMeta.t[BuferrMeta.ne+1]=fileMeta;
             BuferrMeta.ne++;
   
            fseek(ms, -1*sizeof(BLOC_co), SEEK_CUR);
            fwrite(&BuferrMeta,sizeof(BLOC_co),1,ms);
          break;
        }
        }


    

    
    // Chargement des enregistrements
    if (internalmode == 0)
        Charger_les_élèves_non_triée_dans_fichier_de_donnée(f, fname, startBlock,  ceil((double)taille / FB), taille);
    else
       Charger_les_élèves_triée_dans_fichier_de_donnée(f, fname, startBlock,  ceil((double)taille / FB), taille);



printf("Le fichier '%s' a été créé avec succès.\n", fname);
}

printf("Il n'y a pas assez d'espace (dans les blocMeta) pour créer le fichier'%s'\n", fname);
   }








/**
 * Fonction pour charger un fichier
 */
void chargerFichier_co(FILE *ms,FILE *f, char fileName[20], int taille ) {
    int tableAllocation[NbBloc];
    BLOC_meta_co BuferrMeta;
    BLOC_co Buferr;
    int nbEtudiant, startBlock;




// Recherche du fichier dans les métadonnées
   fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    int found = 0, k = 0, j = 0;

    while (k < NbBlocmeta_co && found == 0) {
        fread(&BuferrMeta, sizeof(BuferrMeta), 1, ms);

        for (j = 0; j < BuferrMeta.ne && found == 0; j++) {
            if (strcmp(BuferrMeta.t[j].FDnom, fileName) == 0) found = 1;
        }
        k++;
    }

    if (found == 0) {
        printf("Erreur : Le fichier '%s' n'existe pas dans les métadonnées.\n", fileName);
        return;
    }

   printf("Le fichier existe.\n");
   
    nbEtudiant= BuferrMeta.t[j].nbEtudiant;
    startBlock=BuferrMeta.t[j].adresse;


     int i=0;
    rewind(f) ;
   while ( i<nbEtudiant){
     // on lis les blocs du fichier et on les stock dans la MS
    fread(&Buferr,sizeof(BLOC_co),1,f);
      // on insere le bloc 
    fseek(ms, NbBloc*sizeof(int) + NbBlocmeta_co * sizeof(BLOC_co) + (startBlock + i) * sizeof(BLOC_co), SEEK_SET);
    fwrite(&Buferr,sizeof(BLOC_co),1,ms);

   }

   printf("Le fichier '%s' a été chargé en mode non trié avec succès.\n", fileName);

    // Mise à jour de la table d'allocation
    for (int i = startBlock; i < startBlock + nbEtudiant; i++) {
         update_Allocation_table(ms,i , 1)  ;
    }

}










   // Fonction pour renommer un fichier
void Renommer_co(FILE *ms, char *oldName, char *newName) {
    BLOC_meta_co BuferrMeta;
    int found = 0;

    // Positionner le curseur sur les blocs dédiés aux métadonnées
    fseek(ms, NbBloc*sizeof(int), SEEK_SET); // Sauter la table d'allocation

    // Parcourir les blocs de métadonnées
    for (int i = 0; i < NbBlocmeta_co; i++) {
        fread(&BuferrMeta, sizeof(BLOC_co), 1, ms); // Lire un bloc de métadonnées

        // Rechercher le fichier par son nom
        for (int j = 0; j < BuferrMeta.ne; j++) {
            if (strcmp(BuferrMeta.t[j].FDnom, oldName) == 0) {
                // Fichier trouvé, modifier son nom
                strcpy(BuferrMeta.t[j].FDnom, newName);
                found = 1;

                // Revenir en arrière pour réécrire le bloc mis à jour
                fseek(ms, -sizeof(BLOC_co), SEEK_CUR);
                fwrite(&BuferrMeta, sizeof(BLOC_co), 1, ms);

                printf("Le fichier '%s' a été renommé en '%s'.\n", oldName, newName);
                break;
            }
        }

        if (found) break; // Arrêter la recherche si le fichier est trouvé
    }

    if (!found) {
        printf("Erreur : Le fichier '%s' n'existe pas.\n", oldName);
    }
}






 // Fonction pour supprime un fichier
void supprime_fichier_contigue(FILE *ms, char nom[20]) {
    BLOC_meta_co BuferrMeta;
    FDmeta meta;
    int i, trouv = 0, metaIndex, BlocMetacount, BlocMetaindex, startAddress, numBlocks;
   

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Passer la table d'allocation
    BlocMetacount = 0;
    while (!trouv && fread(&BuferrMeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < BuferrMeta.ne; i++) {
            if (strcmp(BuferrMeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = BuferrMeta.t[i];
                metaIndex = i;
                BlocMetaindex = BlocMetacount;
                break;
            }
        }
        BlocMetacount++;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }

    // Initialisation des variables
    startAddress = meta.adresse; // nbEtudiant du premier bloc
    numBlocks = meta.nbEtudiant;    // Nombre de blocs alloués

    // Mettre à jour la table d'allocation (libérer les blocs du fichier)
    
    for (int j = startAddress; j < startAddress + numBlocks; j++) {
         update_Allocation_table(ms,j , 0);// Marquer les blocs comme libres
    }

    // Mettre à jour les métadonnées
    meta.taille = 0; // Nombre d'étudiants
    meta.nbEtudiant = 0;     // Nombre de blocs
    meta.adresse = -1;   // Aucun bloc alloué
    BuferrMeta.t[metaIndex] = meta;

    // Sauvegarder les métadonnées mises à jour
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, BlocMetaindex * sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&BuferrMeta, sizeof(BLOC_ch), 1, ms);

  

    printf("Suppression du fichier terminée.\n");
    printf("Nombre de blocs libérés : %d\n", numBlocks);
}
