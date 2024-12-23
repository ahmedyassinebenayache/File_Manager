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

typedef struct BLOC_ch BLOC_ch;
struct BLOC_ch
{
    Tetudiant t[FB];
    int ne;
    int next ;
};
typedef struct 
{
    char FDnom[20];
    int taille;
    int nbEtudiant;
    int adresse ;
    int modeglobal ; // 0 : contigue, 1 chainee
    int modeinterne ; // 0 : non triee , 1 : triee
 
} FDmeta ;

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



void Initialize_Disk_Co(FILE *ms){

 int Allocation_Table[NbBloc];

 for(int i=0; i<NbBloc; i++){
    Allocation_Table[i] = 0;  //marquer tous les blocs comme libres
 }
 fseek(ms,0,SEEK_SET);

 fwrite(Allocation_Table,sizeof(int),NbBloc,ms); //écrire la table d'allocation dans le premier bloc
                            
  FDmeta meta;             //valeurs par défaut pour les blocs métadonnées
  BLOC_meta_co meta_bloc;
  strcpy(meta.FDnom,"");
  meta.taille = 0;
  meta.nbEtudiant = 0;
  meta.adresse = -1;
  meta.modeglobal = 0;
  meta.modeinterne = 0;
 fseek(ms,NbBloc * sizeof(int) ,SEEK_SET);  //déplacer le curseur vers le bloc suivant
 //valeurs par défaut pour les blocs métadonnées
 meta_bloc.ne = 0;
 for(int j=0; j<NbBloc; j++){

 for(int i= 0; i<FB; i++){   

  meta_bloc.t[i] = meta;
 }
 fwrite(&meta_bloc ,sizeof(meta_bloc), 1,ms); //écrire les blocs métadonnées dans MS
 }


 }

 void update_Allocation_Table(FILE *ms,int bloc_adress , int b){ 
    int Allocation_Table[NbBloc];                              
    fseek(ms,0,SEEK_SET);                                     // Déplace le pointeur de fichier au début du fichier

    fread(Allocation_Table,NbBloc *sizeof(int),1,ms);

    Allocation_Table[bloc_adress] = b;   // Met à jour la table d'allocation à l'adresse du bloc spécifiée

    fseek(ms,0,SEEK_SET);                                    // Déplace le pointeur de fichier au début du fichier

    fwrite(Allocation_Table, NbBloc * sizeof(int),1,ms);       // Écrit la table d'allocation mise à jour dans le fichier

 }

void empty_MS_Co(FILE *ms){
    Initialize_Disk_Co(ms);
}
void Compact_Disk_Co(FILE *ms){
   
    BLOC_co bloc;
    BLOC_meta_co meta;
    int Allocation_Table[NbBloc];                     //lire la table d'allocation

    fseek(ms ,0 ,SEEK_SET);
    fread(Allocation_Table,NbBloc * sizeof(int),1,ms);

    int current = 0 ;                     
    for(int i = 0; i<NbBloc; i++){
    if(Allocation_Table[i]==1){           //chercher un bloc non vide
        if( i != current){                
          Allocation_Table[i]= 0;        //marquer le bloc comme vide
          Allocation_Table[current]=1;   //marquer le bloc comme non vide

          fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_co)+ i *sizeof(BLOC_co) ,SEEK_SET); //copier les données du bloc i vers le bloc actuel

          fread(&bloc,sizeof (BLOC_co),1,ms);

          fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_co)+ current *sizeof(BLOC_co) ,SEEK_SET);

          fwrite(&bloc, sizeof(bloc),1,ms);
          
                                              //si le bloc i est le premier bloc du fichier, mettre à jour le bloc métadonnées

          for(int k=0; k<NbBlocmeta;k++){
            fseek(ms, NbBloc * sizeof(int) + k *sizeof(BLOC_co),SEEK_SET);
            fread(&meta,sizeof(BLOC_co),1,ms);
            for(int j=0; j<FB;j++){
                if(meta.t->adresse == i){
                    meta.t[j].adresse = current;
                fseek(ms,NbBloc * sizeof(int) +k*sizeof(BLOC_co),SEEK_SET);
                fwrite(&meta,sizeof(BLOC_co),1,ms);
                break;
                }
            }
          }
         current++;             
          }
    }
    }
   fseek(ms,0,SEEK_SET);
   
   fwrite(Allocation_Table,NbBloc * sizeof(int),1,ms);  
}
int Manage_Storage_Space_Co(FILE *ms ,int num_Blocs){
    int Allocation_Table[NbBloc];
    fseek(ms, 0 ,SEEK_SET);

    fread(Allocation_Table,NbBloc * sizeof(int),1,ms); //lire la table d'allocation depuis MS
    int counter = 0;
    for(int i = 0; i<NbBloc; i++){     //compter les blocs libres consécutifs
        if(Allocation_Table[i] == 0){
            counter ++;
        }else{ if(counter == num_Blocs) { break; }
        else{counter = 0;}
    }
    if(counter < num_Blocs){  //si les blocs sont insuffisants, compacter le disque
        Compact_Disk_Co(ms);
        
    }else{
        return 0;            //les blocs sont suffisants, quitter avec 0
    }
     counter = 0 ;           //réinitialiser le compteur

     fseek(ms, 0 ,SEEK_SET);//relire la table d'allocation

     fread(Allocation_Table,NbBloc * sizeof(int),1,ms);

    for(int i = 0; i<NbBloc; i++){ 
          //vérification des blocs après la compactage

        if(Allocation_Table[i] == 0){
            counter ++;
        }else{ break;}
    }
    if(counter >=num_Blocs){   //les blocs sont suffisants, quitter avec 0

        return 0;
    }else{
        printf("MS is Full "); //MS est plein, quitter avec 1

        return 1;
    }

}
}