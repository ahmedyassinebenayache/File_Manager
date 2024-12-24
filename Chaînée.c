
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
// si on trouve un espace vide allors on le retourne (apres chaque allocation il faudra metre a jour la tablle d'allocation)
int allouer (FILE *ms ){
    int k=-1 ,table[NbBloc]  ;
    fseek(ms, 0, SEEK_SET);
    fread(table,sizeof(int),NbBloc,ms) ;
    for (int i = 0; i < NbBloc; i++)
    {
        if (table[i]==0) {
            table[i]=1 , k=i , i=NbBloc+1 ;
        }
    }
        return k ;
}
void creer_un_fichier_chainne_non_triee(FILE *ms ,FILE *f,char nom[20],int nbEtudiant){
    // creation de metadonne
    FDmeta meta ;
    BLOC_meta_ch bloCmeta ;
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    int trouv=0,k,taille ;
    // on cherche la une metadonne vide dans la MS 
    while(!trouv){
    fread(&bloCmeta,sizeof(BLOC_ch),1,ms); 
    for (int  i = 0; i < FB; i++)
    {
        if (bloCmeta.t[i].nbEtudiant==0){
            trouv = 1 ;
            meta = bloCmeta.t[i] ;
            strcpy(meta.FDnom, nom);
            // ajouter le nombre d'etudiants et les modes d'organization
            meta.modeglobal=1 ;
            meta.modeinterne=0 ;
            meta.nbEtudiant = nbEtudiant ;
            // calculer le nombre de bloc necessaire
            taille = ceil((double)nbEtudiant / FB);
            meta.taille=taille ;
            // generer l'adresse du premier bloc
            meta.adresse = allouer(ms) ;
            // mise a jour de la tablle 
            update_Allocation_Table(ms,meta.adresse,1) ;
            // insertion du bloc apres insertion du nouvelle metadonne
            bloCmeta.t[i]=meta ;
            bloCmeta.ne++ ;
            fseek(ms, -1*sizeof(BLOC_ch), SEEK_CUR);
            fwrite(&bloCmeta,sizeof(BLOC_ch),1,ms);


        }
    }

}
    // ajouter les etudiants dans le fichier 
    rewind(f) ;
    BLOC_ch buffer;
    k = 0 ;
    for(int i=0; i<taille; i++)
    {
        buffer.ne = 0;
        int j = 0;
        while(j<FB && k<nbEtudiant)
        {
            // lire les information des etudiants
            printf("Saisir les informations de l'etudiant %d :\n", k + 1);

            printf("ID : ");
            scanf("%d", &buffer.t[j].id);

            printf("FDnom : ");
            scanf(" %[^\n]", buffer.t[j].nom);

            printf("Prenom : ");
            scanf(" %[^\n]", buffer.t[j].prenom);

            printf("Section : ");
            scanf(" %[^\n]", buffer.t[j].sec);
            buffer.ne++;
            j++;
            k++;
        }
        fwrite(&buffer, sizeof(BLOC_ch), 1, f);
    }
}   
void chargement_du_fichier_contigue_non_triee(FILE *ms,FILE *f,char nom[20]){
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    BLOC_meta_ch bloCmeta ;
    BLOC_ch buffer ;
    FDmeta meta ;
    int i=0, trouv=0 ;
    // on cherche la metadonne dans la MS a l'aide du NOM du fichier 
    do{
        fread(&bloCmeta,sizeof(BLOC_ch),1,ms);
        for ( i = 0; i < bloCmeta.ne; i++)
        {
        
            if (strcmp(bloCmeta.t[i].FDnom,nom)==0)
            {
                trouv=1,meta=bloCmeta.t[i] ;
            }
            

        }

    }while (!trouv) ;
    if(!trouv){
        printf("Le fichier n'existe pas.\n"); return ;
    }
    printf("Le fichier existe.\n");
    rewind(f) ;
    while ( i<meta.taille){
    // on lis les blocs du fichier et on les stock dans la MS
    fread(&buffer,sizeof(BLOC_ch),1,f);
    // le dernier bloc dois pointer sur -1 car il n'a pas de sucesseur 
    if (i+1==meta.taille){
        buffer.next=-1 ;
    }else{
    buffer.next=allouer(ms) ;
    // mise a jour de la tablle 
    update_Allocation_Table(ms,buffer.next,1) ;
    }
    // on insere le bloc 
    fseek(ms, (meta.adresse)*sizeof(BLOC_ch)+NbBloc*sizeof(int)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
    fwrite(&buffer,sizeof(BLOC_ch),1,ms);
    // on garde l'adresse pour inserer le bloc suivant 
    meta.adresse=buffer.next ;  
    i++ ;
    }
}
void ajouter_etudiant_fichier_contigue_non_triee(FILE *ms,FILE *f ,char nom[20]){
    Tetudiant etudiant ;
    BLOC_ch buffer2 ;
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    BLOC_meta_ch bloCmeta ;
    BLOC_ch buffer1 ;
    FDmeta meta ;   
    // numerobloc pour suivre le numero du bloc 
    // index_bloc_meta pour sauvegarder le bloc de la metadonne
    // numerometa pour suivre la metadonne dans le bloc 
    // indexmeta pour sauvegrder cette dernierre 
    int  indexmeta ,numerobloc,index_bloc_meta, numerometa=0, trouv=0 ;
    do{ numerobloc =0 ;
        fread(&bloCmeta,sizeof(BLOC_ch),1,ms);
        do{ numerometa=0 ;
            if (strcmp(bloCmeta.t[numerometa].FDnom,nom)==0)
            {
                trouv=1,indexmeta=numerometa,meta=bloCmeta.t[numerometa],index_bloc_meta=numerobloc ;
            }
            numerometa++ ;
            
        }while(!trouv&&numerometa<bloCmeta.ne) ;
        numerobloc++ ;
    }while (!trouv) ;
    if(!trouv){
        printf("Le fichier n'existe pas.\n"); return ;
    }
    printf("Le fichier existe.\n");
    // recherche du dernier bloc 
    // on va parcourir avec p (p est l'adresse du suivant) 
    int p = meta.adresse ;

    while(p!=-1){
        fseek(ms,(p)*sizeof(BLOC_ch)+ NbBloc*sizeof(int)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer1,sizeof(BLOC_ch),1,ms);
        p=buffer1.next ;
    }
    // si le bloc peut contenir un autre enriegistrement on l'ajoute
    if (buffer1.ne<FB){
        printf("Saisir les informations du nouvelle etudiant %d :\n" );
        printf("ID : ");
        scanf("%d", &buffer1.t[buffer1.ne].id);
        printf("FDnom : ");
        scanf(" %[^\n]", buffer1.t[buffer1.ne].nom);
        printf("Prenom : ");
        scanf(" %[^\n]", buffer1.t[buffer1.ne].prenom);
        printf("Section : ");
        scanf(" %[^\n]", buffer1.t[buffer1.ne].sec);
        buffer1.ne++ ;  
        // insertion en MS
        fseek(ms, -1*sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&buffer1,sizeof(BLOC_ch),1,ms);
        // on mets a jour la metadonne de f 
        bloCmeta.t[indexmeta].nbEtudiant++ ;
        fseek(ms,( index_bloc_meta)*sizeof(BLOC_ch) + NbBloc*sizeof(int), SEEK_SET);
        fwrite(&bloCmeta,sizeof(BLOC_ch),1,ms) ; 
        // insertion en fichier F
        fseek(f,(meta.taille-1)*sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer1,sizeof(BLOC_ch),1,f);
    }
    //sinon on alloue un nouvau bloc
    else{
        buffer1.next=allouer(ms) ;
        // mise a jour de la tablle 
        update_Allocation_Table(ms,buffer1.next,1) ;
        fseek(ms, -1*sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&buffer1,sizeof(BLOC_ch),1,ms);
        p=buffer1.next ;
        printf("Saisir les informations du nouvelle etudiant %d :\n" );

        printf("ID : ");
        scanf("%d", &buffer1.t[0].id);

        printf("FDnom : ");
        scanf(" %[^\n]", buffer1.t[0].nom);

        printf("Prenom : ");
        scanf(" %[^\n]", buffer1.t[0].prenom);

        printf("Section : ");
        scanf(" %[^\n]", buffer1.t[0].sec); 
        buffer1.ne=1 ;
        buffer1.next=-1 ; 
        // insertion en MS
        fseek(ms,(p)*sizeof(BLOC_ch)+ NbBloc*sizeof(int)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer1,sizeof(BLOC_ch),1,ms);
        //metre a jour les meta donne
        bloCmeta.t[indexmeta].nbEtudiant++ ;
        bloCmeta.t[indexmeta].taille++ ;
        fseek(ms,(index_bloc_meta)*sizeof(BLOC_ch) + NbBloc*sizeof(int), SEEK_SET);
        fwrite(&bloCmeta,sizeof(BLOC_ch),1,ms);
        // insertion en fichier F
        fseek(f,(meta.taille)*sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer1,sizeof(BLOC_ch),1,f);

        
    } 


}

void recherche_fichier_contigue_non_triee(FILE *ms, char nom[20],int id,int p[2],FILE *f){
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    BLOC_meta_ch bloCmeta ;
    BLOC_ch buffer, buffer2 ;
    FDmeta meta ;
    // on cherche la metadonne 
    int numerometa=0,k , trouv=0 ;
    do{
        numerometa=0 ;
        fread(&bloCmeta,sizeof(BLOC_ch),1,ms);
        do{
            if (strcmp(bloCmeta.t[numerometa].FDnom,nom)==0)
            {
                trouv=1,meta=bloCmeta.t[numerometa] ;
            }
            numerometa++ ;

        }while(!trouv&&numerometa<bloCmeta.ne) ;

    }while (!trouv) ;
    if(!trouv){
        printf("Le fichier n'existe pas.\n"); return ;
    }
    printf("Le fichier existe.\n");
    trouv = 0 ;
    // numerobloc pour suivre le numero de bloc
    // suiv pour suivre l'adresse du suivant
    int numerobloc=0 , suiv=meta.adresse ;
    while (numerobloc<meta.taille&&!trouv) 
    {   
       fseek(ms,(suiv)*sizeof(BLOC_ch)+ NbBloc*sizeof(int)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
       fread(&buffer,sizeof(BLOC_ch),1,ms);   
        for (int i = 0; i < buffer.ne; i++)
        {
            if (buffer.t[i].id=id)
            {   trouv=1 ;
                printf("etudiant trouve !\n") ;
                p[0]=numerobloc ;
                p[1]=i ;
                return;
            }
            
        }
        numerobloc++ ;
    }
    if (!trouv){
    printf("etudiant non trouve !\n"); return ;

}
}
void suppression_physique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta_ch bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress ,blocmetacount ,blocmetaindex ;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount=0 ;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex=blocmetacount ;
            }
        }
        blocmetacount++ ;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // on sauvegarde les etudiant dans un tableaux et on les supprime le fichier puis on va le recreer et le recharger de sorte a supprime physiquement l'etudiant choisi

    Tetudiant *arr = (Tetudiant *)malloc((meta.nbEtudiant-1) * sizeof(Tetudiant));
    
    // Initialization des variables
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    // nbetudiant est l'index des etudiant dans la tablle et vas finir par etre le nombre total d'etudiant
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].id != id) {
                // Gardez les étudiants qui ne correspondent pas a l'etudiant choisi
                arr[nbetudiant]=buffer.t[i] , nbetudiant++ ;
            }
        }
        // on va garder l'adresse du premier bloc car j'ai configurer la fonction charger pour deja l'utilise 
        if (currentAddress!=meta.adresse)
        {
        // mise a jour de la tablle 
        update_Allocation_Table(ms,currentAddress,0) ;
        }
        currentAddress = buffer.next; // aller vers le bloc suivant
    }
    // reset du fichier 
    fclose(f) ;
    FILE *file = fopen(nom, "w");
    fclose(f) ;
    FILE *file = fopen(nom, "rb+");
    rewind(f) ;
    for ( i = 0; i < nbetudiant; i++)
    {   int j = 0 ;
        // ajouter les etudiants bloc par bloc dans le fichier 
        while(i<nbetudiant&&j<FB){
            BLOC_ch buffer;
            buffer.ne = 0;
            buffer.t[j]=arr[i] ;
            buffer.ne++;
            j++;
        }
        blocCount ++ ;
        fwrite(&buffer, sizeof(buffer), 1, f);
    }
    
    // metre a jour les metadata
    meta.nbEtudiant = nbetudiant; // nb etudiant
    meta.taille = blocCount;       // nb de blocs
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex*sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    // on charge le fichier en MS
    chargement_du_fichier_contigue_non_triee(ms,f,nom) ;
    printf("Suppression physique terminée.\n");
    printf("Nombre d'étudiants mis à jour : %d\n", nbetudiant);
    printf("Nombre de blocs mis à jour : %d\n", blocCount);
    free(arr) ;
}
void renomer_fichier_contigue_non_triee(FILE *ms , char nom[20], char nouveaunom[20]){
    BLOC_meta_ch bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress ,blocmetacount ,blocmetaindex ;
    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount=0 ;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex=blocmetacount ;
            }
        }
        blocmetacount ++ ;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    strcpy(meta.FDnom,nouveaunom) ;
    //sauvegarde les metadonnes
    fseek(ms, -1 * sizeof(BLOC_ch), SEEK_SET);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
}
void supprime_fichier_contigue_non_triee(FILE *ms , char nom[20], char nouveaunom[20]){
    BLOC_meta_ch bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress ,blocmetacount ,blocmetaindex ;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount=0 ;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex=blocmetacount ;
            }
        }
        blocmetacount++ ;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // Initialization des variables
    currentAddress = meta.adresse;
    // on met tout les bloc du fichier a 0
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress ) * sizeof(BLOC_ch)+ NbBlocmeta * sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        // mise a jour de la tablle 
        update_Allocation_Table(ms,currentAddress,0) ;
        currentAddress = buffer.next; // Move to the next block
    }
    // metre a jour les metadata
    meta.nbEtudiant = 0; // nb etudiant
    meta.taille = 0;      // nb de blocs
    meta.adresse=-1 ;
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex*sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    printf("Suppression du fichier terminée.\n");
}
void suppression_logique_fichier_chainne_non_triee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta_ch bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress ,blocmetacount ,blocmetaindex ;
    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount=0 ;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex=blocmetacount ;
            }
        }
        blocmetacount++ ;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }  
    // Initialization des variables
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    // on cherche l'etudiant
    while (!trouv) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].id == id) {
            // supression logique
            buffer.t[i].etat=0 ;
            trouv=1 ;
            fseek(ms, -1*sizeof(BLOC_ch), SEEK_CUR); // metre a jour le bloc dans la ms
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);


        }
        currentAddress = buffer.next; // aller vers le bloc suivant
        blocCount++;
    }
    rewind(f) ;
    fseek(f, (blocCount-1)*sizeof(BLOC_ch), SEEK_CUR); // metre a jour le bloc dans le fichier f
    fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
    // metre a jour les metadata
    meta.nbEtudiant = meta.nbEtudiant-1; // nb etudiant     
    bloCmeta.t[metaIndex] = meta;
    //sauvegarde les metadonnes
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex*sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    printf("Suppression logique terminée.\n");
    printf("Nombre d'étudiants mis à jour : %d\n", meta.nbEtudiant);
}
}
void defragmentation_fichier_chainee_non_triee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta_ch bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress ,blocmetacount ,blocmetaindex ;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount=0 ;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex=blocmetacount ;
            }
        }
        blocmetacount++ ;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // on sauvegarde les etudiant dans un tableaux et on les supprime du fichier puis on va le recreer et le recharger de sorte a supprime physiquement l'etudiant choisi
    Tetudiant *arr = (Tetudiant *)malloc((meta.nbEtudiant) * sizeof(Tetudiant));
    
    // Initialization des variables
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    // nbetudiant est l'index des etudiant dans la tablle et vas finir par etre le nombre total d'etudiant
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch)+NbBlocmeta*sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].etat != 0) {
                // Gardez les étudiants qui ne sont pas supprime logiquement
                arr[nbetudiant]=buffer.t[i] , nbetudiant++ ;
            }
        }
        // on va garder l'adresse du premier bloc car j'ai configurer la fonction charger pour deja l'utilise 
        if (currentAddress!=meta.adresse)
        {
        // mise a jour de la tablle 
        update_Allocation_Table(ms,currentAddress,0) ;
        }
        currentAddress = buffer.next; // aller vers le bloc suivant
    }
    // reset du fichier 
    fclose(f) ;
    FILE *file = fopen(nom, "w");
    fclose(f) ;
    FILE *file = fopen(nom, "rb+");
    rewind(f) ;
    for ( i = 0; i < nbetudiant; i++)
    {   int j = 0 ;
        // ajouter les etudiants bloc par bloc dans le fichier 
        while(i<nbetudiant&&j<FB){
            BLOC_ch buffer;
            buffer.ne = 0;
            buffer.t[j]=arr[i] ;
            buffer.ne++;
            j++;
        }
        blocCount ++ ;
        fwrite(&buffer, sizeof(buffer), 1, f);
    }
    
    // metre a jour les metadata
    meta.nbEtudiant = nbetudiant; // nb etudiant
    meta.taille = blocCount;       // nb de blocs
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex*sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    // on charge le fichier en MS
    chargement_du_fichier_contigue_non_triee(ms,f,nom) ;
    printf("Compactage termine !.\n");
    printf("Nombre d'étudiants mis à jour : %d\n", nbetudiant);
    printf("Nombre de blocs mis à jour : %d\n", blocCount);
    free(arr) ;
}
