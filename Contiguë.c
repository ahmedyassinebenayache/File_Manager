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
//chercher méta-données 
FDmeta Searchmetadata(FILE *ms, char fname[30]){
    BLOC_meta_co meta;
    fseek(ms, MAX* sizeof(int),SEEK_SET);
        for (int j = 0; j <NbBLOC_meta_co ; ++j) {
            fread(&meta, sizeof(BLOC_ch), 1, ms);
            for (int i = 0; i <FB; ++i) {
                if (strcmp(meta.t[i].fname, fname) == 1) {                    
                    return meta.t[i];
                }
            }
        }        
    }

//Mis à jour méta-données 
void MAJMETADATA(FILE *ms, FDmeta M){
    BLOC_meta_co meta;
    int cont_block_meta = 0;
    while(cont_block_meta < NbBLOC_meta_co){
        rewind(ms);
        fseek(ms, (MAX + cont_block_meta)* sizeof(int),SEEK_SET);
        fread(&meta, sizeof(BLOC_ch),1,ms);
        int cont_meta = 0;
        while(cont_meta < meta.ne){
            if(strcmp(meta.t[cont_meta].fname, M.fname)==0){
                meta.t[cont_meta] = M;
                fseek(ms, (MAX + cont_block_meta)* sizeof(int), SEEK_SET);
                fwrite(&meta, sizeof(BLOC_ch),1,ms);
                return;
            }
            cont_meta++;
        }
        cont_block_meta++;
    }
    perror("fichier non trouvé");
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


//insertion d'un nouveau etudiant
void insertion_co(FILE *MS, char file_name[30]){
    BLOC_meta_co F_m;
    FDmeta META;
    Tetudiant nouv_Tetudiant;

    printf("Entrer les ineormations de votre nouveau etudiant\n");    
    printf("Le nom\n");
    scane("%s",nouv_Tetudiant.nom);        
    printf("Le prenom\n");
    scane("%s",nouv_Tetudiant.prenom);
    printf("L'identifiant\n");
    scane("%d",&nouv_Tetudiant.id);
    printf("La section\n");
    scane("%c",&nouv_Tetudiant.sec);
    nouv_Tetudiant.etat = 1;    

    
    rewind(MS);
    META = Searchmetadata(MS,&file_name);

    if(strcmp(file_name,META.fname) == 0){
        BLOCTable alloc_table;
        BLOC_co buffer;
        rewind(MS);        
        int add_first_block = META.adresse ;
        fseek(MS, ((add_first_block + META.taille) - 1)*sizeof(BLOC_meta_co)+MAX*sizeof(int),SEEK_SET);
        fread(&buffer, sizeof(BLOC_co), 1, MS);
        if(META.modeinterne == 0){        
            if(buffer.ne < FB){
                buffer.t[buffer.ne] = nouv_Tetudiant;
                buffer.ne++;
                META.nbEtudian ++;
                fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
                fwrite(&buffer, sizeof(BLOC_co), 1, MS);
                MAJMETADATA(MS, META);
            }else{
                rewind(MS);
                fread(&alloc_table, sizeof(BLOCTable), 1, MS);
                int conteur = (add_first_block + META.taille) - 1;    
                if(alloc_table.tableAllocation[conteur + 1] == 0){
                    //Faire entrer ce nouveau etudiant dans un nouveau block
                    META.taille = META.taille + 1;
                    META.nbEtudian = META.nbEtudian + 1;            
                    MAJMETADATA(MS, META);
                    rewind(MS);
                    fseek(MS,(conteur-1)*sizeof(BLOC_meta_co), SEEK_SET);
                    buffer.t[0] = nouv_Tetudiant;
                    buffer.ne = 1;
                    fwrite(&buffer, sizeof(buffer), 1, MS);                          
                }else{
                    printf("Impossible d'ajouter un etudiant, la table de allocation est pleine\n");
                }
            }
        }else{
            int block_counteur = 0;
            int nb_block = META.taille;
            while(block_counteur < nb_block){
                rewind(MS);
                fseek(MS, ((add_first_block + block_counteur) - 1)*sizeof(BLOC_meta_co)+MAX*sizeof(int),SEEK_SET);
                fread(&buffer, sizeof(BLOC_co), 1, MS);
                int conteur_buffer = 0;
                while(conteur_buffer < buffer.ne && buffer.t[conteur_buffer].id < nouv_Tetudiant.id){
                    conteur_buffer++;
                }
                if(buffer.t[conteur_buffer].id > nouv_Tetudiant.id){
                    if(buffer.ne < FB){
                        int temp = conteur_buffer;
                        conteur_buffer = buffer.ne - 1;
                        while(conteur_buffer != temp ){
                            buffer.t[conteur_buffer + 1] = buffer.t[conteur_buffer];
                            conteur_buffer--;
                        }
                        buffer.t[conteur_buffer] = nouv_Tetudiant;
                        buffer.ne++;                        
                        fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
                        fwrite(&buffer, sizeof(BLOC_co), 1, MS);                            
                    }else{                        
                        BLOC_co temp_buffer;
                        temp_buffer = buffer;
                        int nbe = 0;
                        while(temp_buffer.ne == FB){
                            fread(&buffer, sizeof(BLOC_co), 1,MS);
                            nbe = nbe + temp_buffer.ne;                            
                        }
                        Tetudiant vect[nbe];                        
                        if(temp_buffer.ne != FB ){
                            Tetudiant vect[nbe];
                            temp_buffer = buffer;
                            //traité le block actuel
                            int j = 0;
                            int i;
                            for(i = 0; i <= FB ; i++){
                                if(i == conteur_buffer){
                                    vect[i] = nouv_Tetudiant;
                                    vect[i + 1] = temp_buffer.t[j];
                                    i++;                                   
                                }else{
                                    vect[i] = temp_buffer.t[j];
                                }
                                i++;
                                j++;
                            }
                            //traité le reste                           
                            int r_nbe = nbe - (FB + 1);
                            fseek(MS, ((add_first_block + block_counteur) - 1)*sizeof(BLOC_meta_co)+MAX*sizeof(int),SEEK_SET);
                            while(r_nbe > 0){
                                fread(&temp_buffer, sizeof(BLOC_meta_co), 1,MS);
                                j = 0;
                                while(j < temp_buffer.ne ){
                                    vect[i] = temp_buffer.t[j];
                                    i++;
                                    j++;
                                }
                            }
                            temp_buffer.ne++;
                            fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
                            fwrite(&temp_buffer, sizeof(BLOC_meta_co), 1, MS);
                            fseek(MS, ((add_first_block + block_counteur) - 1)*sizeof(BLOC_meta_co)+MAX*sizeof(int),SEEK_SET);  
                            fread(&buffer, sizeof(BLOC_co), 1, MS);
                            j = 0;                          
                            for(i = 0; i<nbe ; i++){
                                if(j < buffer.ne){                                    
                                    buffer.t[j] = vect[i];
                                    j++;
                                }else{
                                    j = 0;
                                    fread(&buffer, sizeof(BLOC_co), 1, MS);
                                }
                            }                               
                        }else{
                            printf("Impossible d'ajouter un etudiant, la table de allocation est pleine\n");
                        }                        
                    }                    
                    META.nbEtudian ++;                    
                    MAJMETADATA(MS, META);
                    return;
                }else{
                    block_counteur++;
                }                
            }
                
        }            
        
    }else{
        printf("Fichier %s non trouvé\n", file_name);
        
        return;
    }
}

//La recherche d'un etudiant
void Recherche_co(FILE *MS, int id_Tetudiant, int *num_block, int *deplacement) {
    BLOC_meta_co F_m;
    FDmeta m_eta;
    BLOC_co buffer;

    // Ouvrir le fichier en mode lecture binaire
    
    if (MS == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    int conteur_block_meta = 0;
    fseek(MS, MAX* sizeof(int),SEEK_SET);
    // Parcourir les blocs de métadonnées
    while (conteur_block_meta < NbBLOC_meta_co) {        
        fread(&F_m, sizeof(BLOC_meta_co), 1, MS);
        // Parcourir les métadonnées de chaque fichier
        for (int conteur_meta = 0; conteur_meta < F_m.ne; conteur_meta++) {
            m_eta = F_m.t[conteur_meta];

            // Mode non_triée
            if (m_eta.modeinterne == 0) {
                for (int n_blk = 0; n_blk < m_eta.taille; n_blk++) {
                    fseek(MS, ((m_eta.adresse  + n_blk) - 1) * sizeof(BLOC_co), SEEK_SET);
                    fread(&buffer, sizeof(BLOC_co), 1, MS);

                    for (int cont_buff = 0; cont_buff < buffer.ne; cont_buff++) {
                        if (buffer.t[cont_buff].id == id_Tetudiant) {
                            *num_block = m_eta.adresse  + n_blk;
                            *deplacement = cont_buff;
                            fclose(MS);
                            return;
                        }
                    }
                }
            } 
            // Mode triée
            else {
                for (int n_blk = 0; n_blk < m_eta.taille; n_blk++) {
                    fseek(MS, ((m_eta.adresse  + n_blk) - 1) * sizeof(BLOC_co), SEEK_SET);
                    fread(&buffer, sizeof(BLOC_co), 1, MS);

                    int debut_block = 0;
                    int fin_block = buffer.ne - 1;

                    // Recherche binaire dans le bloc
                    while (debut_block <= fin_block) {
                        int milieu_block = (debut_block + fin_block) / 2;

                        if (buffer.t[milieu_block].id == id_Tetudiant) {
                            *num_block = m_eta.adresse  + n_blk;
                            *deplacement = milieu_block;
                            fclose(MS);
                            return;
                        }

                        if (buffer.t[milieu_block].id > id_Tetudiant) {
                            fin_block = milieu_block - 1;
                        } else {
                            debut_block = milieu_block + 1;
                        }
                    }
                }
            }
        }
        conteur_block_meta++;
    }

    // Si l'étudiant n'a pas été trouvé
    printf("L'étudiant avec l'identifiant %d n'a pas été trouvé\n", id_Tetudiant);
    fclose(MS);
}


//Suppression d'un enregistrement logiquement 
void Suppression_Enregistrement_logique_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]) {
    BLOC_meta_co F_M;
    FDmeta me_ta;

    MS = fopen("disk.dat", "r+"); // Correction pour ouvrir un fichier avec un nom fixe
    if (MS == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    me_ta = Searchmetadata(MS, file_name);
    if (strcmp(file_name, me_ta.fname) == 0) {
        BLOC_co buffer;
        int num_block = -1;
        int deplacement = -1;

        Recherche(MS, ID_SUPP_Tetudiant, &num_block, &deplacement);
        if (num_block != -1 && deplacement != -1) {
            // Positionnement au bloc concerné
            fseek(MS, (num_block - 1) * sizeof(BLOC_co), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);

            // Suppression logique
            buffer.t[deplacement].etat = 0;               

            // Écriture des modifications
            fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
            fwrite(&buffer, sizeof(BLOC_co), 1, MS);
            MAJMETADATA(MS, me_ta);

            printf("L'étudiant a été supprimé avec succès.\n");
        } else {
            printf("L'étudiant avec l'ID %d n'a pas été trouvé.\n", ID_SUPP_Tetudiant);
        }
    } else {
        printf("Fichier %s non trouvé.\n", file_name);
    }

    fclose(MS);
}

//Suppression d'un enregistrement physiquement 
void Suppression_Enregistrement_physic_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]) {

    BLOC_meta_co F_M;
    FDmeta me_ta;

    // Ouvrir le fichier en mode lecture/écriture binaire
    MS = fopen(file_name, "rb+");
    if (!MS) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    // Lire les métadonnées principales
    me_ta = Searchmetadata(MS, file_name);
    if (strcmp(me_ta.fname, file_name) == 0) { // Fichier trouvé

        BLOC_co buffer;
        int num_block = -1;
        int deplacement = -1;

        // Recherche de l'étudiant à supprimer
        Recherche(MS, ID_SUPP_Tetudiant, &num_block, &deplacement);

        if (num_block != -1 && deplacement != -1) {
            // Charger le bloc contenant l'étudiant
            fseek(MS, (num_block - 1) * sizeof(BLOC_co), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);

            // Supprimer l'étudiant et compacter le bloc
            for (int i = deplacement; i < buffer.ne - 1; i++) {
                buffer.t[i] = buffer.t[i + 1];
            }
            buffer.ne--;
            me_ta.nbEtudian--;

            // Écrire le bloc mis à jour dans le fichier
            fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
            fwrite(&buffer, sizeof(BLOC_co), 1, MS);

            // Réorganiser les blocs restants
            int total_elements = me_ta.nbEtudian;
            Tetudiant vect[total_elements];
            int index_vect = 0;

            // Charger tous les blocs en mémoire
            fseek(MS, (me_ta.adresse  - 1) * sizeof(BLOC_co), SEEK_SET);
            for (int i = 0; i < me_ta.taille; i++) {
                fread(&buffer, sizeof(BLOC_co), 1, MS);
                for (int j = 0; j < buffer.ne; j++) {
                    vect[index_vect++] = buffer.t[j];
                }
            }

            // Réécrire les blocs réorganisés
            fseek(MS, (me_ta.adresse  - 1) * sizeof(BLOC_co), SEEK_SET);
            int elements_written = 0;
            while (total_elements > 0) {
                if (total_elements > FB) {
                    buffer.ne = FB;
                } else {
                    buffer.ne = total_elements;
                }
                for (int i = 0; i < buffer.ne; i++) {
                    buffer.t[i] = vect[elements_written++];
                }
                total_elements -= buffer.ne;
                fwrite(&buffer, sizeof(BLOC_co), 1, MS);
            }

            // Mettre à jour les métadonnées
            me_ta.taille = (me_ta.nbEtudian + FB - 1) / FB; // Nombre de blocs nécessaires
            MAJMETADATA(MS, me_ta);

            printf("L'étudiant avec ID %d a été supprimé avec succès.\n", ID_SUPP_Tetudiant);
        } else {
            printf("L'étudiant avec ID %d n'a pas été trouvé.\n", ID_SUPP_Tetudiant);
        }
    } else {
        printf("Fichier %s non trouvé dans les métadonnées.\n", file_name);
    }

    // Fermer le fichier
    fclose(MS);
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
