#include "contiguë.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Operations for Contiguous
/* Memory */
void Initialize_Disk_Co(FILE *ms){

    int Allocation_Table[NbBloc];

    for(int i=0; i<NbBloc; i++){
        Allocation_Table[i] = 0;  // mark all blocks as free
    }
    fseek(ms,0,SEEK_SET);

    fwrite(Allocation_Table,sizeof(int),NbBloc,ms); // write the allocation table in the first block

    FDmeta meta;             // default values for metadata blocks
    BLOC_meta meta_bloc;
    strcpy(meta.FDnom,"");
    meta.taille = 0;
    meta.nbEtudiant = 0;
    meta.adresse = -1;
    meta.modeglobal = 0;
    meta.modeinterne = 0;
    fseek(ms,NbBloc * sizeof(int) ,SEEK_SET);  // move the cursor to the next block
    // default values for metadata blocks
    meta_bloc.ne = 0;
    for(int j=0; j<NbBloc; j++){

        for(int i= 0; i<FB; i++){

        meta_bloc.t[i] = meta;
        }
    fwrite(&meta_bloc ,sizeof(meta_bloc), 1,ms); // write the metadata blocks in ms
    }
}

void update_Allocation_Table_co(FILE *ms,int bloc_adress , int b){
    int Allocation_Table[NbBloc];
    fseek(ms,0,SEEK_SET);                                     // Move the file pointer to the beginning of the file

    fread(Allocation_Table,NbBloc *sizeof(int),1,ms);

    Allocation_Table[bloc_adress] = b;   // Update the allocation table at the specified block address

    fseek(ms,0,SEEK_SET);                                    // Move the file pointer to the beginning of the file

    fwrite(Allocation_Table, NbBloc * sizeof(int),1,ms);       // Write the updated allocation table to the file
 }

 void empty_MS_Co(FILE *ms) {
    Initialize_Disk_Co(ms); // Initialize the disk for contiguous storage
}

void compactdisk_co(FILE *ms) {
    rewind(ms);

    int allocation_Table[NbBloc];
    fread(allocation_Table, sizeof(int), NbBloc, ms);

    int current = 0; // Current free block

    BLOC_co buffer1; // Buffers to store data blocks
    BLOC_co buffer2;
    BLOC_meta buffer3; // Buffer for metadata

    while (current < NbBloc) {
        fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + current * sizeof(BLOC_co), SEEK_SET);
        fread(&buffer1, sizeof(BLOC_co), 1, ms);

        if (allocation_Table[current] == 0 && buffer1.ne == 0) {
            // Find a used block to relocate
            for (int i = 0; i < NbBloc; i++) {
                fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + i * sizeof(BLOC_co), SEEK_SET);
                fread(&buffer2, sizeof(BLOC_co), 1, ms);

                if (allocation_Table[i] == 1 || (allocation_Table[i] == 0 && buffer2.ne > 0)) {
                    allocation_Table[i] = 0;

                    // Update metadata if this is the first block of the file
                    for (int j = 0; j < NbBlocmeta; j++) {
                        fseek(ms, NbBloc * sizeof(int) + j * sizeof(BLOC_meta), SEEK_SET);
                        fread(&buffer3, sizeof(BLOC_meta), 1, ms);
                        for (int k = 0; k < buffer3.ne; k++) {
                            if (buffer3.t[k].adresse == i) {
                                buffer3.t[k].adresse = current;
                                fseek(ms, NbBloc * sizeof(int) + j * sizeof(BLOC_meta), SEEK_SET);
                                fwrite(&buffer3, sizeof(BLOC_meta), 1, ms);
                                break;
                            }
                        }
                    }

                    // Move the block
                    fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + current * sizeof(BLOC_co), SEEK_SET);
                    fwrite(&buffer2, sizeof(BLOC_co), 1, ms);

                    // Clear the old block
                    buffer2.ne = 0;
                    fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + i * sizeof(BLOC_co), SEEK_SET);
                    fwrite(&buffer2, sizeof(BLOC_co), 1, ms);

                    break;
                }
            }
        }
        current++;
    }

    // Update allocation table
    rewind(ms);
    fwrite(allocation_Table, sizeof(int), NbBloc, ms);
}

// Function to search for a group of free blocks in the allocation table
void allouer_co(int *start, int tableAllocation[NbBloc], int nbEtudiant) {
    int freeBlocks = 0;
    *start = -1;
    for (int i = 0; i < NbBloc; i++) {
        if (tableAllocation[i] == 0) { // Unused block
            if (freeBlocks == 0) {*start = i; } // Start of the free sequence
            freeBlocks++;
        } else {
            freeBlocks = 0; // Reset if a block is used
        }

        if (freeBlocks == nbEtudiant) break; // Sufficient sequence found
    }

    if (freeBlocks < nbEtudiant) {
        *start = -1; // Not enough free blocks
    }
}

// Function to sort students by ID (bubble sort)
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

/*File*/
void creer_un_fichier_co(FILE *ms, FILE *f, char FDnom[20], int nbEtudiant, int internalmode) {
    FDmeta fileMeta;
    BLOC_meta BuferrMeta;
    int *tableAllocation = (int *)malloc(NbBloc * sizeof(int));
    if (tableAllocation == NULL) {
        printf("Error: Unable to allocate memory for the allocation table.\n");
        return;
    }
    int taille = (int)ceil((double)nbEtudiant / FB);

    // Read the allocation table
    fseek(ms, 0, SEEK_SET);
    fread(tableAllocation, sizeof(int), NbBloc, ms);

    // Find an empty descriptive block
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    int trouv = 0, startBlock = -1;

    for (int j = 0; j < NbBlocmeta && !trouv; j++) {
        fread(&BuferrMeta, sizeof(BLOC_meta), 1, ms);

        for (int i = 0; i < FB; i++) {
            if (BuferrMeta.t[i].taille == 0) {
                trouv = 1;
                fileMeta.nbEtudiant = nbEtudiant;
                fileMeta.taille = taille;
                fileMeta.modeglobal = 0;
                fileMeta.modeinterne = internalmode;
                strcpy(fileMeta.FDnom, FDnom);

                allouer_co(&startBlock, tableAllocation, taille);
                if (startBlock == -1) {
                    printf("Error: Not enough free space to create the file.\n");
                    free(tableAllocation);
                    return;
                }

                fileMeta.adresse = startBlock;

                BuferrMeta.t[i] = fileMeta;
                BuferrMeta.ne++;

                fseek(ms, NbBloc * sizeof(int) + j * sizeof(BLOC_meta), SEEK_SET);
                fwrite(&BuferrMeta, sizeof(BLOC_meta), 1, ms);
                break;
            }
        }
    }

    if (!trouv) {
        printf("Error: No available metadata space to create the file.\n");
        free(tableAllocation);
        return;
    }

    // Loading students into blocks
    if (internalmode == 0) {
        Load_unsorted_students_into_data_file(f, FDnom, startBlock, nbEtudiant, taille);
    } else {
        Load_sorted_students_into_data_file(f, FDnom, startBlock, nbEtudiant, taille);
    }

    // Update allocation table
    for (int i = startBlock; i < startBlock + taille; i++) {
        update_Allocation_Table_co(ms, i, 1);
    }

    printf("File '%s' created successfully.\n", FDnom);
    free(tableAllocation);
}

// Function to load students in unsorted mode
void Load_unsorted_students_into_data_file(FILE *f, char fileName[20], int startBloc, int nbEtudiant, int taille) {
    Tetudiant record;
    BLOC_co Buferr;
    int recordIndex = 0;

    rewind(f);
    printf("Enter student information (ID, Name, First name, Section):\n");
    for (int i = 0; i < taille; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < nbEtudiant; j++) {
            printf("Student %d:\n", recordIndex + 1);
            record.etat = 1; // Existing student
            printf("ID: ");
            scanf("%d", &record.id);
            printf("Name: ");
            scanf(" %s[^\n]", record.nom);
            printf("First name: ");
            scanf(" %s[^\n]", record.prenom);
            printf("Section: ");
            scanf(" %c", &record.sec);

            Buferr.t[j] = record;
            Buferr.ne++;
            recordIndex++;
        }

        // Write the block into the data file
        fwrite(&Buferr, sizeof(BLOC_co), 1, f);
    }
}

// Function to load students in sorted mode
void Load_sorted_students_into_data_file(FILE *f, char fileName[20], int startBloc, int nbEtudiant, int taille) {
   Tetudiant *tTetudiant = malloc(nbEtudiant * sizeof(Tetudiant));
    if (tTetudiant == NULL) {
        printf("Error: Unable to allocate memory.\n");
        return;
    }

    printf("Enter the student information (ID, Last Name, First Name, Section):\n");
    for (int i = 0; i < nbEtudiant; i++) {
        tTetudiant[i].etat = 1; // Existing student
        printf("Student %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &tTetudiant[i].id);
        printf("Last Name: ");
        scanf(" %s[^\n]", tTetudiant[i].nom);
        printf("First Name: ");
        scanf(" %s[^\n]", tTetudiant[i].prenom);
        printf("Section: ");
        scanf(" %c", &tTetudiant[i].sec);
    }

    // sort the students
    trierTetudiants(tTetudiant, nbEtudiant);

    // Load students into blocks
    BLOC_co Buferr;
    int recordIndex = 0;

    rewind(f);
    for (int i = 0; i < taille; i++) {
        Buferr.ne = 0;

        for (int j = 0; j < FB && recordIndex < nbEtudiant; j++) {
            Buferr.t[j] = tTetudiant[recordIndex++];
            Buferr.ne++;
        }

        // Write the block into the data file
        fwrite(&Buferr, sizeof(BLOC_co), 1, f);
    }

    free(tTetudiant);
}

void chargerFichier_co(FILE *ms, FILE *f, char fileName[20]) {
    if (ms == NULL || f == NULL) {
        printf("Error: Invalid file pointer.\n");
        return;
    }

    BLOC_meta BuferrMeta;
    BLOC_co buffer;
    FDmeta meta;
    int found = 0;

    // Find file metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    for (int i = 0; i < NbBlocmeta; i++) {
        fread(&BuferrMeta, sizeof(BLOC_meta), 1, ms);
        for (int j = 0; j < BuferrMeta.ne; j++) {
            if (strcmp(BuferrMeta.t[j].FDnom, fileName) == 0) {
                found = 1;
                meta = BuferrMeta.t[j];
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        printf("Error: File '%s' not found.\n", fileName);
        return;
    }

    // Load blocks to data file
    fseek(f, 0, SEEK_SET);
    for (int currentBlock = 0; currentBlock < meta.taille; currentBlock++) {
        fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) +
              (meta.adresse + currentBlock) * sizeof(BLOC_co), SEEK_SET);
        fread(&buffer, sizeof(BLOC_co), 1, f);
        fwrite(&buffer, sizeof(BLOC_co), 1, ms);
    }

    printf("File '%s' successfully loaded into secondary memory.\n", fileName);
}

FDmeta Searchmetadata_Co(FILE *ms, char FDnom[20]) {
    FDmeta m;
    BLOC_meta meta;
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    for (int j = 0; j < NbBlocmeta; ++j) {
        fread(&meta, sizeof(BLOC_meta), 1, ms);
        for (int i = 0; i < FB; ++i) {
            if (strcmp(meta.t[i].FDnom, FDnom) == 0) {
                m = meta.t[i];
                return m;
            }
        }
    }
    // If file is not found, set the address to -1 to indicate failure
    m.adresse = -1;
    return m;
}

// Update metadata
void MAJMETADATA(FILE *ms, FDmeta M){
    BLOC_meta meta;
    int cont_block_meta = 0;
    while (cont_block_meta < NbBlocmeta) {
        rewind(ms);
        fseek(ms, (NbBloc + cont_block_meta) * sizeof(int), SEEK_SET);
        fread(&meta, sizeof(BLOC_meta), 1, ms);
        int cont_meta = 0;
        while (cont_meta < meta.ne) {
            if (strcmp(meta.t[cont_meta].FDnom, M.FDnom) == 0) {
                meta.t[cont_meta] = M;
                fseek(ms, (NbBloc + cont_block_meta) * sizeof(int), SEEK_SET);
                fwrite(&meta, sizeof(BLOC_meta), 1, ms);
                return;
            }
            cont_meta++;
        }
        cont_block_meta++;
    }
    perror("file not found");
}

void Display_fichier_co(FILE *MS, char filename[20]) {
    BLOC_co buffer;
    FDmeta m_eta;

    // Search for the metadata of the file
    m_eta = Searchmetadata_Co(MS, filename);

    // Check if the file exists
    if (m_eta.adresse == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    // Display the content of the file block by block
    for (int n_blk = 0; n_blk < m_eta.taille; n_blk++) {
        printf("Reading block %d\n", n_blk);
        fseek(MS, ((m_eta.adresse + n_blk)) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
        fread(&buffer, sizeof(BLOC_co), 1, MS);
        printf("Content of the read block:\n");
        for (int cont_buff = 0; cont_buff < buffer.ne; cont_buff++) {
            printf("Student ID: %d, Name: %s, First Name: %s, State: %d\n",
                   buffer.t[cont_buff].id, buffer.t[cont_buff].nom, buffer.t[cont_buff].prenom, buffer.t[cont_buff].etat);
        }
    }
}

// Function to rename a file
void Renommer_co(FILE *ms, char *oldName, char *newName) {
    BLOC_meta BuferrMeta;
    int found = 0;

    // Position the cursor on the blocks dedicated to metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Skip the allocation table

    // Traverse the metadata blocks
    for (int i = 0; i < NbBlocmeta; i++) {
        fread(&BuferrMeta, sizeof(BLOC_meta), 1, ms); // Read a metadata block

        // Search for the file by its name
        for (int j = 0; j < BuferrMeta.ne; j++) {
            if (strcmp(BuferrMeta.t[j].FDnom, oldName) == 0) {
                // File found, change its name
                strcpy(BuferrMeta.t[j].FDnom, newName);
                found = 1;

                // Go back to rewrite the updated block
                fseek(ms, -sizeof(BLOC_meta), SEEK_CUR);
                fwrite(&BuferrMeta, sizeof(BLOC_meta), 1, ms);

                printf("File '%s' has been renamed to '%s'.\n", oldName, newName);
                break;
            }
        }

        if (found) break; // Stop searching if the file is found
    }

    if (!found) {
        printf("Error: File '%s' does not exist.\n", oldName);
    }
}

// Function to delete a file
void supprime_fichier_contigue(FILE *ms, char nom[20]) {
    BLOC_meta BuferrMeta;
    FDmeta meta;
    int i, found = 0, metaIndex, BlocMetacount, BlocMetaindex, startAddress, numBlocks;

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Skip the allocation table
    BlocMetacount = 0;
    while (!found && fread(&BuferrMeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < BuferrMeta.ne; i++) {
            if (strcmp(BuferrMeta.t[i].FDnom, nom) == 0) {
                found = 1;
                meta = BuferrMeta.t[i];
                metaIndex = i;
                BlocMetaindex = BlocMetacount;
                break;
            }
        }
        BlocMetacount++;
    }

    if (!found) {
        printf("File not found.\n");
        return;
    }

    // Initialize variables
    startAddress = meta.adresse; // Starting address of the first block
    numBlocks = meta.nbEtudiant; // Number of allocated blocks

    // Update the allocation table (free the file's blocks)
    for (int j = startAddress; j < startAddress + numBlocks; j++) {
        update_Allocation_Table_co(ms, j, 0); // Mark blocks as free
    }

    // Update the metadata
    meta.taille = 0; // Number of students
    meta.nbEtudiant = 0; // Number of blocks
    meta.adresse = -1; // No blocks allocated
    strcpy(meta.FDnom, "");

    BuferrMeta.t[metaIndex] = meta;

    // Save the updated metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, BlocMetaindex * sizeof(BLOC_meta), SEEK_CUR);
    fwrite(&BuferrMeta, sizeof(BLOC_meta), 1, ms);

    printf("File deletion completed.\n");
    printf("Number of blocks released: %d\n", numBlocks);
}

// Function to insert a student into a contiguous file
void insertion_co(FILE *MS, char file_name[30]){
    BLOC_meta F_m;
    FDmeta META;
    Tetudiant nouv_Tetudiant;

    // Prompt for new student information
    printf("Enter the information for the new student\n");
    printf("Name: ");
    scanf("%49s", nouv_Tetudiant.nom);  // Limit to 49 characters to avoid overflow

    // Read the first name
    printf("First name: ");
    scanf("%49s", nouv_Tetudiant.prenom);

    // Read the ID
    printf("ID: ");
    scanf("%d", &nouv_Tetudiant.id);

    // Read the section
    printf("Section: ");
    getchar(); // Absorb the newline left in the buffer by the previous `scanf`
    scanf("%c", &nouv_Tetudiant.sec);

    // Set the state
    nouv_Tetudiant.etat = 1;

    printf("DEBUG: New student details - Name: %s, First Name: %s, ID: %d, Section: %c\n", nouv_Tetudiant.nom, nouv_Tetudiant.prenom, nouv_Tetudiant.id, nouv_Tetudiant.sec);

    rewind(MS);
    META = Searchmetadata_Co(MS, file_name);

    if(strcmp(file_name, META.FDnom) == 0){
        int alloc_table[NbBloc];
        BLOC_co buffer;
        rewind(MS);
        int add_first_block = META.adresse;
        fseek(MS, (add_first_block) * sizeof(BLOC_co) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_co), 1, MS);
        if(META.modeinterne == 0){
            int i = 0;
            while(META.taille > i){
                if(buffer.ne < FB){
                    buffer.t[buffer.ne] = nouv_Tetudiant;
                    buffer.ne++;
                    META.nbEtudiant++;
                    fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
                    fwrite(&buffer, sizeof(BLOC_co), 1, MS);
                    MAJMETADATA(MS, META);
                    printf("Insertion successful\n");
                    return;
                }
                i++;
                fseek(MS, (add_first_block + i) * sizeof(BLOC_co) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
                fread(&buffer, sizeof(BLOC_co), 1, MS);
            }
            if(i == META.taille){
                printf("There is no more storage\n");
                return;
            }
        } else {
            printf("DEBUG: Entering sorted mode insertion\n");
            if(META.taille == ceil((double)(META.nbEtudiant + 1) / FB)){
                Tetudiant *temp_v;
                temp_v = (Tetudiant *)malloc((META.nbEtudiant + 1) * sizeof(Tetudiant));

                int counter = 0;
                while(counter < META.nbEtudiant){
                    for(int i = 0; buffer.ne > i; i++){
                        temp_v[counter] = buffer.t[i];
                        counter++;
                    }
                    fread(&buffer, sizeof(BLOC_co), 1, MS);
                }
                // Insert the new student
                printf("DEBUG: Inserting new student with ID: %d\n", nouv_Tetudiant.id);
                int inserted = 0;
                int i;
                // Check if the new student should be the first element
                for (i = META.nbEtudiant - 1; (i >= 0 && temp_v[i].id > nouv_Tetudiant.id); i--) {
                    temp_v[i + 1] = temp_v[i];  // Shift elements to the right
                }
                // Insert the value at the found position
                temp_v[i + 1] = nouv_Tetudiant;
                fseek(MS, (add_first_block) * sizeof(BLOC_co) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
                i = 0;
                counter = 0;
                while(i < META.taille && counter < META.nbEtudiant + 1){
                    fread(&buffer, sizeof(BLOC_co), 1, MS);
                    int j;
                    for(j = 0; j < FB && counter < META.nbEtudiant + 1; j++){
                        buffer.t[j] = temp_v[counter];
                        counter++;
                    }
                    buffer.ne = j;
                    fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
                    fwrite(&buffer, sizeof(BLOC_co), 1, MS);
                    i++;
                }

                printf("Insertion successful\n");
                META.nbEtudiant++;
                MAJMETADATA(MS, META);
                free(temp_v);
                return;
            } else {
                printf("There is no more storage\n");
                return;
            }
        }
    } else {
        printf("File %s not found\n", file_name);
        return;
    }
}

void defragmentation_co(FILE *ms, FILE *f, char nom[20]) {
    BLOC_meta bloCmeta;
    BLOC_co buffer, tempBuffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!trouv && fread(&bloCmeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            printf("Checking file: %s\n", bloCmeta.t[i].FDnom);
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
                printf("File found: %s (Index: %d, BlocMetaIndex: %d)\n", nom, metaIndex, blocmetaindex);
            }
        }
        blocmetacount++;
    }

    if (!trouv) {
        printf("File not found.\n");
        return;
    }

    // Create an array for the remaining students
    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant) * sizeof(Tetudiant));
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    i = 0;
    printf("Size: %d\n", meta.taille);

    // Traverse the blocks of the file
    while (i < meta.taille) {
        printf("Reading block at address: %d\n", currentAddress);
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_co), 1, ms);
        printf("Block read: %d students\n", buffer.ne);

        for (int k = 0; k < buffer.ne; k++) {
            printf("Student ID: %d\n", buffer.t[k].id);
            if (buffer.t[k].etat == 1) {
                arr[nbetudiant] = buffer.t[k];
                nbetudiant++;
            }
        }

        if (currentAddress != meta.adresse) {
            printf("Updating allocation table for block: %d\n", currentAddress);
            update_Allocation_Table_co(ms, currentAddress, 0);
        }
        i++;
        currentAddress++;
    }

    printf("Total number of students after defragmentation: %d\n", nbetudiant);
    rewind(f);
    i = 0;
    while (i < nbetudiant) {
        int j = 0;
        BLOC_co buffer;
        buffer.ne = 0;
        while (i < nbetudiant && j < FB) {
            buffer.t[j] = arr[i];
            buffer.ne++;
            i++;
            j++;
        }
        blocCount++;

        fwrite(&buffer, sizeof(BLOC_co), 1, f);
        printf("Block written: %d students\n", buffer.ne);
    }

    // Update metadata
    meta.nbEtudiant = nbetudiant;
    meta.taille = blocCount;
    bloCmeta.t[metaIndex] = meta;

    // Save metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex * sizeof(BLOC_meta), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);

    printf("Metadata updated: Number of students = %d, Number of blocks = %d, Address = %d\n", meta.nbEtudiant, meta.taille, meta.adresse);

    // Load the file into secondary memory
    chargerFichier_co(ms, f, nom);
    printf("Defragmentation completed.\n");
    printf("Number of students updated: %d\n", nbetudiant);
    printf("Number of blocks updated: %d\n", blocCount);

    free(arr);
}

void Recherche_co(FILE *MS, int id_Tetudiant, char filename[20], int *num_block, int *deplacement) {

    FDmeta m_eta;
    BLOC_co buffer;

    // Open the file in binary read mode
    if (MS == NULL) {
        perror("Error opening the file");
        return;
    }

    m_eta = Searchmetadata_Co(MS, filename);
    *num_block = -1;
    *deplacement = -1;
    printf("File name %s\n", m_eta.FDnom);
    printf("File address %d\n", m_eta.adresse);
    printf("File size %d\n", m_eta.taille);
    printf("Number of students in the file %d\n", m_eta.nbEtudiant);


    // Unsorted mode
    if (m_eta.modeinterne == 0) {
        for (int n_blk = 0; n_blk < m_eta.taille; n_blk++) {
            printf("Reading block %d\n", n_blk);
            fseek(MS, ((m_eta.adresse + n_blk)) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);
            printf("Content of the read block:\n");
            for (int cont_buff = 0; cont_buff < buffer.ne; cont_buff++) {
                printf("Student ID: %d, Name: %s\n", buffer.t[cont_buff].id, buffer.t[cont_buff].nom);
                if (buffer.t[cont_buff].id == id_Tetudiant) {
                    *num_block = m_eta.adresse + n_blk;
                    *deplacement = cont_buff;
                    printf("Student found in block %d, position %d\n", *num_block, *deplacement);
                    return;
                }
            }
        }
        // If the student is not found
        printf("The student was not found\n");
    }
    // Sorted mode
    else {
        for (int n_blk = 0; n_blk < m_eta.taille; n_blk++) {
            printf("Reading block %d\n", n_blk);
            fseek(MS, ((m_eta.adresse + n_blk)) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);
            printf("Content of the read block:\n");
            for (int cont_buff = 0; cont_buff < buffer.ne; cont_buff++) {
                printf("Student ID: %d, Name: %s\n", buffer.t[cont_buff].id, buffer.t[cont_buff].nom);
            }

            int start_block = 0;
            int end_block = buffer.ne - 1;

            // Binary search in the block
            while (start_block <= end_block) {
                int mid_block = (start_block + end_block) / 2;
                printf("Binary search - start: %d, end: %d, middle: %d\n", start_block, end_block, mid_block);

                if (buffer.t[mid_block].id == id_Tetudiant) {
                    *num_block = m_eta.adresse + n_blk;
                    *deplacement = mid_block;
                    printf("Student found in block %d, position %d\n", *num_block, *deplacement);
                    return;
                }

                if (buffer.t[mid_block].id > id_Tetudiant) {
                    end_block = mid_block - 1;
                } else {
                    start_block = mid_block + 1;
                }
            }
        }
    }
}

void Suppression_Enregistrement_logique_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]) {
    BLOC_meta F_M;
    FDmeta me_ta;

    if (MS == NULL) {
        perror("Error opening the file");
        return;
    }

    me_ta = Searchmetadata_Co(MS, file_name);
    if (strcmp(file_name, me_ta.FDnom) == 0) {
        BLOC_co buffer;
        int num_block = -1;
        int deplacement = -1;

        Recherche_co(MS, ID_SUPP_Tetudiant, file_name, &num_block, &deplacement);

        if (num_block != -1 && deplacement != -1) {
            // Position to the concerned block
            fseek(MS, (num_block) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);

            // Logical deletion
            buffer.t[deplacement].etat = 0;

            // Write the changes
            fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
            fwrite(&buffer, sizeof(BLOC_co), 1, MS);
            MAJMETADATA(MS, me_ta);

            printf("The student has been successfully deleted.\n");
        } else {
            printf("The student with ID %d was not found.\n", ID_SUPP_Tetudiant);
        }
    } else {
        printf("File %s not found.\n", file_name);
    }
}

void Suppression_Enregistrement_physic_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]) {

    BLOC_meta F_M;
    FDmeta me_ta;

    // Open the file in binary read/write mode
    if (!MS) {
        perror("Error opening the file");
        return;
    }

    // Read the main metadata
    me_ta = Searchmetadata_Co(MS, file_name);
    if (strcmp(me_ta.FDnom, file_name) == 0) { // File found

        BLOC_co buffer;
        int num_block = -1;
        int deplacement = -1;

        // Search for the student to delete
        Recherche_co(MS, ID_SUPP_Tetudiant, file_name, &num_block, &deplacement);

        if (num_block != -1 && deplacement != -1) {
            // Load the block containing the student
            fseek(MS, (num_block) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
            fread(&buffer, sizeof(BLOC_co), 1, MS);

            // Delete the student and compact the block
            for (int i = deplacement; i < buffer.ne - 1; i++) {
                buffer.t[i] = buffer.t[i + 1];
            }
            buffer.ne--;
            me_ta.nbEtudiant--;

            // Write the updated block to the file
            fseek(MS, -sizeof(BLOC_co), SEEK_CUR);
            fwrite(&buffer, sizeof(BLOC_co), 1, MS);

            // Reorganize the remaining blocks
            int total_elements = me_ta.nbEtudiant;
            Tetudiant vect[total_elements];
            int index_vect = 0;

            // Load all blocks into memory
            fseek(MS, (me_ta.adresse) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
            for (int i = 0; i < me_ta.taille; i++) {
                fread(&buffer, sizeof(BLOC_co), 1, MS);
                for (int j = 0; j < buffer.ne; j++) {
                    vect[index_vect] = buffer.t[j];
                    index_vect++;
                }
            }

            // Rewrite the reorganized blocks
            fseek(MS, (me_ta.adresse) * sizeof(BLOC_co) + NbBlocmeta * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
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

            // Update the metadata
            me_ta.taille = ceil((double)me_ta.nbEtudiant / FB); // Number of blocks needed
            MAJMETADATA(MS, me_ta);
            printf("The student with ID %d has been successfully deleted.\n", ID_SUPP_Tetudiant);
            if (total_elements == 0) {
                update_Allocation_Table_co(MS, me_ta.adresse + me_ta.taille, 0);
            }

            return;

        } else {
            printf("The student with ID %d was not found.\n", ID_SUPP_Tetudiant);
            return;
        }
    } else {
        printf("File %s not found in the metadata.\n", file_name);
        return;
    }
}
