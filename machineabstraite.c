#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "machineabstraite.h"

#include <math.h>



/** 
 * Fonction pour rechercher un groupe de blocs libres dans la table d'allocation
 */
void allouer_co(int *start, int tableAllocation[MAX], int blocnum) {
    int freeBlocks = 0;
    *start = -1;
    for (int i = BM + 1; i < MAX; i++) {
        if (tableAllocation[i] == 0) { // Bloc non utilisé
            if (freeBlocks == 0) *start = i; // Début de la séquence libre
            freeBlocks++;
        } else {
            freeBlocks = 0; // Réinitialisation si un bloc est utilisé
        }

        if (freeBlocks == blocnum) break; // Séquence suffisante trouvée
    }

    if (freeBlocks < blocnum) {
        *start = -1; // Pas assez de blocs libres
    }
}






/** 
 * Fonction pour trier les élèves par ID (tri à bulles)
 */
void trierEleves(eleve *Televe, int structnum) {
    for (int i = 0; i < structnum - 1; i++) {
        for (int j = 0; j < structnum - i - 1; j++) {
            if (Televe[j].id > Televe[j + 1].id) {
                eleve temp = Televe[j];
                Televe[j] = Televe[j + 1];
                Televe[j + 1] = temp;
            }
        }
    }
}







 void Charger_les_élèves_non_triée_dans_fichier_de_donnée(FILE *f, char fileName[20], int startBloc, int blocnum, int structnum) {
    eleve record;
    BLOC_co Buferr;
    int recordIndex = 0;

    rewind(f) ;
    printf("Entrez les informations des élèves (ID, Nom, Prénom, Section):\n");
    for (int i = 0; i < blocnum; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < structnum; j++) {
            printf("Élève %d:\n", recordIndex + 1);
            record.etat = 1; // Élève existant
            printf("ID: ");
            scanf("%d", &record.id);
            printf("Nom: ");
            scanf(" %[^\n]", record.nom);
            printf("Prénom: ");
            scanf(" %[^\n]", record.Prenom);
            printf("Section: ");
            scanf(" %c", &record.sec);

            Buferr.T[j] = record;
            Buferr.ne++;
            recordIndex++;
        }

        // Écriture du bloc dans fichier de donnée
      
        fwrite(&Buferr, sizeof(Buferr), 1, f);
    } }










/**
 * Fonction pour charger les élèves en mode trié
 */
void Charger_les_élèves_triée_dans_fichier_de_donnée(FILE *f, char fileName[20], int startBloc, int blocnum, int structnum) {
    eleve *Televe = malloc(structnum * sizeof(eleve));
    if (Televe == NULL) {
        printf("Erreur : Impossible d'allouer de la mémoire.\n");
        return;
    }

    printf("Entrez les informations des élèves (ID, Nom, Prénom, Section):\n");
    for (int i = 0; i < structnum; i++) {
        Televe[i].etat = 1; // Élève existant
        printf("Élève %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &Televe[i].id);
        printf("Nom: ");
        scanf(" %[^\n]", Televe[i].nom);
        printf("Prénom: ");
        scanf(" %[^\n]", Televe[i].Prenom);
        printf("Section: ");
        scanf(" %c", &Televe[i].sec);
    }

    // Tri des élèves
    trierEleves(Televe, structnum);

    // Chargement des élèves dans les blocs
    BLOC_co Buferr;
    int recordIndex = 0;
    
    rewind(f) ;
    for (int i = 0; i < blocnum; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < structnum; j++) {
            Buferr.T[j] = Televe[recordIndex++];
            Buferr.ne++;
        }

        // Écriture du bloc dans dans fichier de donnée
        
        fwrite(&Buferr, sizeof(Buferr), 1, f);
    }

    
    free(Televe);
}











void createFile_co(FILE *ms, FILE *f, char fname[20], int structnum,  int internalmode) {//0 : non triée,   1  : triée
    Meta fileMeta;
    BLOCMETA BuferrMeta;
     int tableAllocation[MAX];
  
    
    
   
    fseek(ms, MAX*sizeof(int), SEEK_SET);
    int trouv=0,k,i,taille ;int startBlock;
    // on cherche la une metadonne vide dans la MS 
    while(!trouv){
    fread(&BuferrMeta,sizeof(BLOC_co),1,ms); 
    for ( i = 0; i < FB; i++)
    {
        if (BuferrMeta.V[i].structnum==0){
            trouv = 1 ;
  
            // Écrit les métadonnées du fichier dans  une meta variable
   
             fileMeta.blocnum = ceil((double)structnum / FB);// Calculer le nombre de blocs requis
             fileMeta.structnum = structnum;
             fileMeta.Globalmode = 0;
             fileMeta.internalmode =internalmode;

             int  Taill = taillede(fname);
              for (int r = 0; r < Taill ; r++){
                fileMeta.fname[r]=fname[r];
                    }


  
            //Trouver des  BLOCS_co vides Pour premier bloc
            
           allouer_co(&startBlock, tableAllocation, fileMeta.blocnum);
           if (startBlock == -1) {
          printf("Erreur : Pas assez d'espace libre pour charger le fichier.\n");
           return;
             }
         fileMeta.firstbloc = startBlock;

         fileMeta.etat=1; //existe

         // Écrit les métadonnées du fichier dans le ms fichier

       

        
          BuferrMeta.V[BuferrMeta.nf+1]=fileMeta;
          BuferrMeta.nf++;
   
            fseek(ms, -1*sizeof(BLOC_co), SEEK_CUR);
            fwrite(&BuferrMeta,sizeof(BLOC_co),1,ms);
          break;
        }}





        if(BuferrMeta.nf=FB){ // Mise à jour de la table d'allocation
            tableAllocation[i]=0;
            fseek(ms, 0, SEEK_SET);
            fwrite(&tableAllocation, sizeof(int), MAX, ms);  
        }
    
    

    
    // Chargement des enregistrements
    if (internalmode == 0)
        Charger_les_élèves_non_triée_dans_fichier_de_donnée(f, fname, startBlock,  ceil((double)structnum / FB), structnum);
    else
       Charger_les_élèves_triée_dans_fichier_de_donnée(f, fname, startBlock,  ceil((double)structnum / FB), structnum);



printf("Le fichier '%s' a été créé avec succès.\n", fname);
}

printf("Il n'y a pas assez d'espace (dans les blocMeta) pour créer le fichier'%s'\n", fname);
   }








/**
 * Fonction pour charger un fichier
 */
void chargerFichier_co(FILE *ms,FILE *f, char fileName[20], int structnum ) {
    int tableAllocation[MAX];
    BLOCMETA BuferrMeta;
    BLOC_co Buferr;
    int blocnum, startBlock;




// Recherche du fichier dans les métadonnées
   fseek(ms, MAX*sizeof(int), SEEK_SET);
    int found = 0, k = 0, j = 0;

    while (k < BM && found == 0) {
        fread(&BuferrMeta, sizeof(BuferrMeta), 1, ms);

        for (j = 0; j < BuferrMeta.nf && found == 0; j++) {
            if (strcmp(BuferrMeta.V[j].fname, fileName) == 0) found = 1;
        }
        k++;
    }

    if (found == 0) {
        printf("Erreur : Le fichier '%s' n'existe pas dans les métadonnées.\n", fileName);
        return;
    }

   printf("Le fichier existe.\n");
   
    blocnum= BuferrMeta.V[j].blocnum;
    startBlock=BuferrMeta.V[j].firstbloc;


     int i=0;
    rewind(f) ;
   while ( i<blocnum){
     // on lis les blocs du fichier et on les stock dans la MS
    fread(&Buferr,sizeof(BLOC_co),1,f);
      // on insere le bloc 
    fseek(ms, MAX*sizeof(int) + BM * sizeof(BLOCMETA) + (startBlock + i) * sizeof(BLOC_co), SEEK_SET);
    fwrite(&Buferr,sizeof(BLOC_co),1,ms);

   }

   printf("Le fichier '%s' a été chargé en mode non trié avec succès.\n", fileName);

    // Lecture de la table d'allocation
    fseek(ms, 0, SEEK_SET);
    if (fread(&tableAllocation, sizeof(int), MAX, ms) != 1) {
        printf("Erreur : Impossible de lire la table d'allocation.\n");
        return;
    }

    // Mise à jour de la table d'allocation
    for (int i = startBlock; i < startBlock + blocnum; i++) {
        tableAllocation[i] = 1;
    }

    fseek(ms, 0, SEEK_SET);
    fwrite(&tableAllocation, sizeof(int), MAX, ms);

    
    
}










   // Fonction pour renommer un fichier
void Renommer_co(FILE *ms, char *oldName, char *newName) {
    BLOCMETA BuferrMeta;
    int found = 0;

    // Positionner le curseur sur les blocs dédiés aux métadonnées
    fseek(ms, MAX*sizeof(int), SEEK_SET); // Sauter la table d'allocation

    // Parcourir les blocs de métadonnées
    for (int i = 0; i < BM; i++) {
        fread(&BuferrMeta, sizeof(BLOCMETA), 1, ms); // Lire un bloc de métadonnées

        // Rechercher le fichier par son nom
        for (int j = 0; j < BuferrMeta.nf; j++) {
            if (strcmp(BuferrMeta.V[j].fname, oldName) == 0) {
                // Fichier trouvé, modifier son nom
                strcpy(BuferrMeta.V[j].fname, newName);
                found = 1;

                // Revenir en arrière pour réécrire le bloc mis à jour
                fseek(ms, -sizeof(BLOCMETA), SEEK_CUR);
                fwrite(&BuferrMeta, sizeof(BLOCMETA), 1, ms);

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

