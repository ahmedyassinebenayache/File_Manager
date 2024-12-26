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
typedef struct Position{
    int nbrbloc;
    int mov;
}Position;


void Initialize_Disk_Ch(FILE *ms){

    int Allocation_Table[NbBloc];

    for(int i=0; i<NbBloc; i++){
        Allocation_Table[i] = 0;  //marquer tous les blocs comme libres
    }
    fseek(ms,0,SEEK_SET);

    fwrite(Allocation_Table,sizeof(int),NbBloc,ms); //écrire la table d'allocation dans le premier bloc

    FDmeta meta;             //valeurs par défaut pour les blocs métadonnées
    BLOC_meta meta_bloc;
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

void empty_MS_Ch(FILE *ms){
    Initialize_Disk_Ch(ms);
}
int Manage_Storage_Space_Ch(FILE *ms ,int num_Etudiant){
    int num_Blocs = ceil((double)num_Blocs / FB);
    int Allocation_Table[NbBloc];
    fseek(ms, 0 ,SEEK_SET);

    fread(Allocation_Table,NbBloc * sizeof(int),1,ms); //lire la table d'allocation depuis MS
    int counter = 0;
    for(int i = 0; i<NbBloc; i++){     //compter les blocs libres
        if(Allocation_Table[i] == 0){
            counter ++;
        } if(counter == num_Blocs) { break; }

    }
    if(counter < num_Blocs){  //si les blocs sont insuffisants
        printf("MS IS FULL");
        return 1;

    }else{
        return 0;            //les blocs sont suffisants, quitter avec 0
    }
}


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
void Addmetadata(FILE *ms, FDmeta M){
    BLOC_meta meta;
    fseek(ms, NbBloc* sizeof(int),SEEK_SET);
    fread(&meta, sizeof(BLOC_ch),1,ms);
    if(meta.ne!=FB){
        int i;
        for ( i = 1; i < NbBlocmeta; ++i) {
            fread(&meta, sizeof(BLOC_ch),1,ms);
            if(meta.ne!=FB){
                break;
            }
        }
        if(i==NbBlocmeta-1){
            perror("disk is full");
        }else{
            meta.t[meta.ne]=M;
            meta.ne++;
        }
    }
    rewind(ms);
}
Position Searchmetadata(FILE *ms, FDmeta M){
    BLOC_meta meta;
    fseek(ms, NbBloc* sizeof(int),SEEK_SET);
    for (int j = 0; j <NbBlocmeta ; ++j) {
        fread(&meta, sizeof(BLOC_ch), 1, ms);
        for (int i = 0; i <FB; ++i) {
            if (strcmp(meta.t[i].FDnom, M.FDnom) == 0) {
                Position X;
                X.nbrbloc=j;
                X.mov=i;
                return X;
            }
        }
    }
    Position y;
    y.mov=-1;
    y.nbrbloc=-1;
    rewind(ms);
    return y;
}
int SortedSearch(BLOC_ch buffer,int ID) {
    if (buffer.t[buffer.ne - 1].id < ID || ID<buffer.t[0].id) {
        return -1;
    } else {
        int size=(int)(buffer.ne/2);
        int left = 0;
        int right = size - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2; // Avoid overflow

            // Check if target is present at mid
            if (buffer.t[mid].id == ID) {
                return mid;
            }

            // If target is greater, ignore the left half
            if (buffer.t[mid].id < ID) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        return -1;
    }
}
void remplissagetriee(Tetudiant **T,int ne){
    for (int i = 0; i <  ne  ; ++i) {
        printf("ID : ");
        scanf("%d", &T[i]->id);

        printf("Nom : ");
        scanf(" %s", &T[i]->nom);

        printf("Prenom : ");
        scanf("%s",&T[i]->prenom);

        printf("Section : ");
        scanf("%c",&T[i]->sec);
        T[i]->etat= 1;
    }
    for (int i = 0; i < ne; i++) {
        for (int j = i+1; j < ne; ++j) {
            if(T[i]->id>T[j]->id){
                Tetudiant *temp ;
                temp=T[j];
                T[j]=T[i];
                T[i]=temp;
            }
        }
    }
}
void Creer_du_fichiertrieechainee(FILE *ms ,FILE *f,char nom[20],int nbEtudiant){
    // creation de metadonne
    FDmeta meta ;
    meta.nbEtudiant=nbEtudiant;
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    printf("donne moi le nom du fichier");
    scanf("%s",&meta.FDnom);
    meta.taille = ceil((double)nbEtudiant / FB);
    meta.adresse = allouer(ms) ;
    update_Allocation_Table(ms,meta.adresse,1) ;
    Addmetadata(ms,meta);

    // ajouter les etudiants dans le fichier
    rewind(f) ;
    Tetudiant *A= (Tetudiant*)malloc(meta.nbEtudiant* sizeof(Tetudiant));
    remplissagetriee(&A,meta.nbEtudiant);
    int size =meta.nbEtudiant;
    BLOC_ch buffer;
    int j=0;
    while(size<FB && j<meta.nbEtudiant ){
        for (int i = 0; i < FB; i++){
            buffer.t[i]=A[j];
            j++;
        }
        fwrite(&buffer, sizeof(BLOC_ch),1,f);
        size-=FB;
    }
    for(int i=0; i<size;i++){
        buffer.t[i]=A[j+i];
    }
    fwrite(&buffer, sizeof(BLOC_ch),1,f);
    free(A);
}
void creer_un_fichier_chainee_non_triee(FILE *ms ,FILE *f,char nom[20],int nbEtudiant){
    // creation de metadonne
    FDmeta meta ;
    BLOC_meta bloCmeta ;
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
                // insertion du bloc apres insertion du nouvelle metadonne
                bloCmeta.t[i]=meta ;
                bloCmeta.ne++ ;
                fseek(ms, -1*sizeof(BLOC_ch), SEEK_CUR);
                fwrite(&bloCmeta,sizeof(BLOC_ch),1,ms);
                // mise a jour de la tablle
                update_Allocation_Table(ms,meta.adresse,1) ;


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
void chargement_fichier_chainee(FILE *ms,FILE *f,char nom[20]){
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    BLOC_meta bloCmeta ;
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
Position Recherche_Sur_un_fichier_chainee_triee(int ID,FILE *ms,char nom[20]){
    int z=sizeof(int)*NbBloc;
    fseek(ms,z,SEEK_SET);
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    Position metapos;
    BLOC_ch buffer;
    FDmeta meta;
    strcpy(meta.FDnom, nom);
    metapos = Searchmetadata(ms, meta);
    fseek(ms,metapos.nbrbloc*sizeof(BLOC_ch)+metapos.mov* sizeof(FDmeta),SEEK_CUR );
    fread(&meta, sizeof(FDmeta),1,ms);
    fseek(ms,z+(NbBlocmeta+meta.adresse)* sizeof(BLOC_ch),SEEK_SET);
    fread(&buffer,sizeof(BLOC_ch),1,ms);//read the first block
    Position x;
    x.mov=0;
    x.nbrbloc=meta.adresse;
    while (buffer.next!=-1) {

        if(SortedSearch(buffer, ID)!=-1){//check if the id is in the block
            x.mov= SortedSearch(buffer,ID);
            return x;
        }
        rewind(ms);
        x.nbrbloc=buffer.next;
        fseek(ms,((buffer.next+NbBlocmeta))*sizeof(BLOC_ch)+z,SEEK_SET); //go to the next block
        fread(&buffer, sizeof(BLOC_ch),1,ms);//read the next block
    }
    perror("id not found");
    Position y;
    y.nbrbloc=-1;
    y.mov=-1;
    return y;
}
void ajouter_etudiant_fichier_chainee_non_triee(FILE *ms,FILE *f ,char nom[20]){
    Tetudiant etudiant ;
    BLOC_ch buffer2 ;
    fseek(ms, NbBloc*sizeof(int), SEEK_SET);
    BLOC_meta bloCmeta ;
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
void insertiondansunfichierTriee(FILE *ms, FDmeta m, Tetudiant x) {
    // Move the file pointer to the starting address of the linked list in the file
    fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + m.adresse) * sizeof(BLOC_ch), SEEK_SET);

    BLOC_ch buffer; // Temporary buffer to hold a block of students
    m.nbEtudiant++; // Increment the number of students
    m.taille = ceil((double) m.nbEtudiant / FB); // Recalculate the file size based on the number of students

    Tetudiant A[m.nbEtudiant]; // Temporary array to hold all students in memory
    int j = 0;

    // Read all blocks and copy their data into the temporary array A
    fread(&buffer, sizeof(BLOC_ch), 1, ms);
    while (buffer.next != -1) {
        for (int i = 0; i < buffer.ne; ++i) {
            A[j] = buffer.t[i]; // Copy each student into the array
            j++;
        }
        // Move to the next block in the linked list
        fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + buffer.next) * sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
    }

    int bool = 0; // Flag to indicate whether the new student has been inserted
    int i = 0;
    j = m.nbEtudiant;

    // Insert the new student (x) into the correct position in the sorted array
    while (i < m.nbEtudiant) {
        // Check if x should be inserted between A[i] and A[i+1]
        if (A[i].id < x.id && x.id > A[i + 1].id) {
            while (j > i + 1) { // Shift elements to make space for x
                A[j] = A[j - 1];
                j--;
            }
            A[i + 1] = x; // Insert x into the array
            bool = 1; // Mark the insertion as complete
        }
        if (bool == 1) { // Exit loop once insertion is done
            break;
        }
        i++;
    }

    j = 0; // Reset the index for writing back data to the file

    // Write updated data back into the file blocks
    fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + m.adresse) * sizeof(BLOC_ch), SEEK_SET);
    int previous;
    while (buffer.next != -1) {
        if (buffer.next != -1) {
            previous = buffer.next;
        }
        for (int i = 0; i < buffer.ne; ++i) {
            buffer.t[i] = A[j]; // Copy data from the array into the buffer
            j++;
        }
        // Move to the next block in the linked list and write back the buffer
        fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + buffer.next) * sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
    }
    int l = 0;
    // If there are still students left in the array, allocate a new block
    if (j != m.nbEtudiant) {
        if (buffer.ne == FB) {
            int k;
            k = allouer(ms); // Allocate a new block in the file
            if (k != -1) {
                buffer.next = k;// Link the new block to the current chain
                update_Allocation_Table(ms, buffer.next, 1);
                while (j < m.nbEtudiant) {
                    buffer.t[l] = A[j];
                    j++;
                    l++;
                }
                buffer.ne = l; // Update the number of students in the new block
                fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + buffer.next) * sizeof(BLOC_ch), SEEK_SET);
                fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
            } else {
                perror("Disk is full"); // Handle allocation failure
                return;
            }
        } else {
            l = buffer.ne - 1;
            while (j < m.nbEtudiant) {
                buffer.t[l] = A[j];
                j++;
                l++;
            }
            buffer.ne = l; // Update the number of students in the last block
            fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta + previous) * sizeof(BLOC_ch), SEEK_SET);
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
        }
    }
}
void recherche_fichier_chainee_non_triee(FILE *ms, char nom[20], int id, int p[2], FILE *f){
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    BLOC_meta bloCmeta;
    BLOC_ch buffer, buffer2;
    FDmeta meta;
    // on cherche la metadonne
    int numerometa = 0, k, trouv = 0;
    do {
        numerometa = 0;
        fread(&bloCmeta, sizeof(BLOC_ch), 1, ms);
        do {
            if (strcmp(bloCmeta.t[numerometa].FDnom, nom) == 0) {
                trouv = 1, meta = bloCmeta.t[numerometa];
            }
            numerometa++;

        } while (!trouv && numerometa < bloCmeta.ne);

    } while (!trouv);
    if (!trouv) {
        printf("Le fichier n'existe pas.\n");
        return;
    }
    printf("Le fichier existe.\n");
    trouv = 0;
    // numerobloc pour suivre le numero de bloc
    // suiv pour suivre l'adresse du suivant
    int numerobloc = 0, suiv = meta.adresse;
    while (numerobloc < meta.taille && !trouv) {
        fseek(ms, (suiv) * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (int i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].id = id) {
                trouv = 1;
                printf("etudiant trouve !\n");
                p[0] = numerobloc;
                p[1] = i;
                return;
            }

        }
        numerobloc++;
    }
    if (!trouv) {
        printf("etudiant non trouve !\n");
        return;

    }
}
void suppression_physique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
            }
        }
        blocmetacount++;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // on sauvegarde les etudiant dans un tableaux et on les supprime le fichier puis on va le recreer et le recharger de sorte a supprime physiquement l'etudiant choisi

    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant - 1) * sizeof(Tetudiant));

    // Initialization des variables
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    // nbetudiant est l'index des etudiant dans la tablle et vas finir par etre le nombre total d'etudiant
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch),
              SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].id != id) {
                // Gardez les étudiants qui ne correspondent pas a l'etudiant choisi
                arr[nbetudiant] = buffer.t[i], nbetudiant++;
            }
        }
        // on va garder l'adresse du premier bloc car j'ai configurer la fonction charger pour deja l'utilise
        if (currentAddress != meta.adresse) {
            // mise a jour de la tablle
            update_Allocation_Table(ms, currentAddress, 0);
        }
        currentAddress = buffer.next; // aller vers le bloc suivant
    }
    // reset du fichier
    fclose(f);
    FILE *file = fopen(nom, "w");
    fclose(f);
    file = fopen(nom, "rb+");
    rewind(f);
    for (i = 0; i < nbetudiant; i++) {
        int j = 0;
        // ajouter les etudiants bloc par bloc dans le fichier
        while (i < nbetudiant && j < FB) {
            BLOC_ch buffer;
            buffer.ne = 0;
            buffer.t[j] = arr[i];
            buffer.ne++;
            j++;
        }
        blocCount++;
        fwrite(&buffer, sizeof(buffer), 1, f);
    }

    // metre a jour les metadata
    meta.nbEtudiant = nbetudiant; // nb etudiant
    meta.taille = blocCount;       // nb de blocs
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex * sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    // on charge le fichier en MS
    chargement_fichier_chainee(ms, f, nom);
    printf("Suppression physique terminée.\n");
    printf("Nombre d'étudiants mis à jour : %d\n", nbetudiant);
    printf("Nombre de blocs mis à jour : %d\n", blocCount);
    free(arr);
}
void renomer_fichier_chainee(FILE *ms, char nom[20], char nouveaunom[20]) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
            }
        }
        blocmetacount++;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    strcpy(meta.FDnom, nouveaunom);
    //sauvegarde les metadonnes
    fseek(ms, -1 * sizeof(BLOC_ch), SEEK_SET);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
}
void supprime_fichier_chainee(FILE *ms, char nom[20], char nouveaunom[20]) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
            }
        }
        blocmetacount++;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // Initialization des variables
    currentAddress = meta.adresse;
    // on met tout les bloc du fichier a 0
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch),
              SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        // mise a jour de la tablle
        update_Allocation_Table(ms, currentAddress, 0);
        currentAddress = buffer.next; // Move to the next block
    }
    // metre a jour les metadata
    meta.nbEtudiant = 0; // nb etudiant
    meta.taille = 0;      // nb de blocs
    meta.adresse = -1;
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex * sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    printf("Suppression du fichier terminée.\n");
}
void suppression_logique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
            }
        }
        blocmetacount++;
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
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch),
              SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].id == id) {
                // supression logique
                buffer.t[i].etat = 0;
                trouv = 1;
                fseek(ms, -1 * sizeof(BLOC_ch), SEEK_CUR); // metre a jour le bloc dans la ms
                fwrite(&buffer, sizeof(BLOC_ch), 1, ms);


            }
            currentAddress = buffer.next; // aller vers le bloc suivant
            blocCount++;
        }
        rewind(f);
        fseek(f, (blocCount - 1) * sizeof(BLOC_ch), SEEK_CUR); // metre a jour le bloc dans le fichier f
        fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
        // metre a jour les metadata
        meta.nbEtudiant = meta.nbEtudiant - 1; // nb etudiant
        bloCmeta.t[metaIndex] = meta;
        //sauvegarde les metadonnes
        fseek(ms, NbBloc * sizeof(int), SEEK_SET);
        fseek(ms, blocmetaindex * sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
        printf("Suppression logique terminée.\n");
        printf("Nombre d'étudiants mis à jour : %d\n", meta.nbEtudiant);
    }
}
void defragmentation_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    // Localiser les métadonnées en fonction du nom du fichier
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_ch), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
            }
        }
        blocmetacount++;
    }

    if (!trouv) {
        printf("Fichier non trouvé.\n");
        return;
    }
    // on sauvegarde les etudiant dans un tableaux et on les supprime du fichier puis on va le recreer et le recharger de sorte a supprime physiquement l'etudiant choisi
    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant) * sizeof(Tetudiant));

    // Initialization des variables
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    // nbetudiant est l'index des etudiant dans la tablle et vas finir par etre le nombre total d'etudiant
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch),
              SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].etat != 0) {
                // Gardez les étudiants qui ne sont pas supprime logiquement
                arr[nbetudiant] = buffer.t[i], nbetudiant++;
            }
        }
        // on va garder l'adresse du premier bloc car j'ai configurer la fonction charger pour deja l'utilise
        if (currentAddress != meta.adresse) {
            // mise a jour de la tablle
            update_Allocation_Table(ms, currentAddress, 0);
        }
        currentAddress = buffer.next; // aller vers le bloc suivant
    }
    // reset du fichier
    fclose(f);
    FILE *file = fopen(nom, "w");
    fclose(f);
    file = fopen(nom, "rb+");
    rewind(f);
    for (i = 0; i < nbetudiant; i++) {
        int j = 0;
        // ajouter les etudiants bloc par bloc dans le fichier
        while (i < nbetudiant && j < FB) {
            BLOC_ch buffer;
            buffer.ne = 0;
            buffer.t[j] = arr[i];
            buffer.ne++;
            j++;
        }
        blocCount++;
        fwrite(&buffer, sizeof(buffer), 1, f);
    }

    // metre a jour les metadata
    meta.nbEtudiant = nbetudiant; // nb etudiant
    meta.taille = blocCount;       // nb de blocs
    bloCmeta.t[metaIndex] = meta;

    //sauvegarde les metadonnes
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex * sizeof(BLOC_ch), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_ch), 1, ms);
    // on charge le fichier en MS
    chargement_fichier_chainee(ms, f, nom);
    printf("Compactage termine !.\n");
    printf("Nombre d'étudiants mis à jour : %d\n", nbetudiant);
    printf("Nombre de blocs mis à jour : %d\n", blocCount);
    free(arr);
}
