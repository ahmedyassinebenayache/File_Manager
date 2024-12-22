#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "machineabstraite.h"

#include <math.h>





void createFile_co(FILE *ms,  char fname[20], int structnum,  int internalmode) {
    Meta fileMeta;
    BLOCTable BuferrTable;
    BLOCMETA BuferrMeta;

  
    
    fseek(ms, 0, SEEK_SET);
    fread(&BuferrTable,MAX* sizeof(BuferrTable), 1, ms); //  Lire le tableau d'allocation
  
   int i=0; //Trouver des blocs meta vides
   while (BuferrTable.tableAllocation[i]==0 && i< BM)
   {
    i=i+1;
   }
  
if(i<BM){

  // Écrit les métadonnées du fichier dans  une meta variable
   
    fileMeta.blocnum = ceil((double)structnum / FB);// Calculer le nombre de blocs requis
    fileMeta.structnum = structnum;
    fileMeta.Globalmode = 0;
    fileMeta.internalmode =internalmode;

    int  Taill = taillede(fname);
    for (int i = 0; i < Taill ; i++){
        fileMeta.fname[i]=fname[i];
        }
    

     int j=BM; //Trouver des  BLOCS_co vides Pour premier bloc
      while (BuferrTable.tableAllocation[j]==0 && i< MAX )
      {
       j++;
       }
 
   fileMeta.firstbloc = j;
   fileMeta.etat=1; //existe

 // Écrit les métadonnées du fichier dans le ms fichier

   fseek(ms, sizeof(BuferrTable), SEEK_SET);   
    fseek(ms, (i+2)*sizeof(BuferrMeta), SEEK_SET);
    fread(&BuferrMeta, sizeof(BuferrMeta), 1, ms);


    BuferrMeta.V[BuferrMeta.nf+1]=fileMeta;
    BuferrMeta.nf++;
   
   
   
        if(BuferrMeta.nf=FB){ // Mise à jour de la table d'allocation
            BuferrTable.tableAllocation[i]=0;
            fseek(ms, 0, SEEK_SET);
            fwrite(&BuferrTable, sizeof(BuferrTable), 1,ms);  
        }
    
    

    fseek(ms, sizeof(BuferrTable), SEEK_SET);   
    fseek(ms, (i+2)*sizeof(BuferrMeta), SEEK_SET);
    fwrite(&BuferrMeta, sizeof(BuferrMeta), 1,ms);

printf("Le fichier '%s' a été créé avec succès.\n", fname);
}

printf("Il n'y a pas assez d'espace (dans les blocMeta) pour créer le fichier'%s'\n", fname);
   }




   
