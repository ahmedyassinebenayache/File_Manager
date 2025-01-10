#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "chaînée.h"
// dans le pire des cas on aura chaque bloc ayant une metadonne

typedef struct Position{
    int nbrbloc;
    int mov;
}Position;

void Initialize_Disk_Ch(FILE *ms){
    int Allocation_Table[NbBloc];

    for(int i = 0; i < NbBloc; i++){
        Allocation_Table[i] = 0;  // mark all blocks as free
    }
    fseek(ms, 0, SEEK_SET);

    fwrite(Allocation_Table, sizeof(int), NbBloc, ms); // write the allocation table to the first block

    FDmeta meta;             // default values for metadata blocks
    BLOC_meta meta_bloc;
    strcpy(meta.FDnom, " ");
    meta.taille = 0;
    meta.nbEtudiant = 0;
    meta.adresse = -1;
    meta.modeglobal = 0;
    meta.modeinterne = 0;
    BLOC_ch buffer;
    buffer.ne = 0;
    buffer.next = -1;
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);  // move the cursor to the next block
    // default values for metadata blocks
    meta_bloc.ne = 0;
    for(int j = 0; j < NbBlocmeta; j++){
        for(int i = 0; i < FB; i++){
            meta_bloc.t[i] = meta;
        }
        fwrite(&meta_bloc, sizeof(BLOC_meta), 1, ms); // write the metadata blocks to MS
    }
}

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

void update_Allocation_Table(FILE *ms, int bloc_adress, int b) {
    int Allocation_Table[NbBloc];
    fseek(ms, 0, SEEK_SET);
    fread(Allocation_Table, sizeof(int), NbBloc, ms);

    Allocation_Table[bloc_adress] = b; // Update the allocation status
    fseek(ms, 0, SEEK_SET);
    fwrite(Allocation_Table, sizeof(int), NbBloc, ms);
}

void empty_MS_Ch(FILE *ms) {
    Initialize_Disk_Ch(ms); // Initialize the disk for linked storage
}

int Manage_Storage_Space_Ch(FILE *ms, int num_Etudiant) {
    int num_Blocs = ceil((double)num_Etudiant / FB); // Calculate required blocks
    int Allocation_Table[NbBloc];
    fseek(ms, 0, SEEK_SET);

    fread(&Allocation_Table, sizeof(int), NbBloc, ms); // Read the allocation table from the disk
    int counter = 0;
    for (int i = 0; i < NbBloc; i++) { // Count the free blocks
        if (Allocation_Table[i] == 0) {
            counter++;
        }
        if (counter == num_Blocs) {
            break;
        }
    }
    if (counter < num_Blocs) { // If not enough blocks
        printf("MS IS FULL\n");
        return 1;
    } else {
        return 0; // Enough blocks available
    }
}

Position Searchmetadata(FILE *ms, FDmeta M) {
    BLOC_meta meta;
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    for (int j = 0; j < NbBlocmeta; ++j) {
        fread(&meta, sizeof(BLOC_meta), 1, ms);
        for (int i = 0; i < FB; ++i) {
            if (strcmp(meta.t[i].FDnom, M.FDnom) == 0) {
                Position X;
                X.nbrbloc = j;
                X.mov = i;
                return X;
            }
        }
    }
    Position y;
    y.mov = -1;
    y.nbrbloc = -1;
    rewind(ms);
    return y;
}

int SortedSearch(BLOC_ch buffer, int ID) {
    if (buffer.t[buffer.ne - 1].id < ID || ID < buffer.t[0].id) {
        return -1; // ID is out of the bounds of the sorted block
    } else {
        int left = 0;
        int right = buffer.ne - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2; // Avoid overflow

            // Check if the target is present at mid
            if (buffer.t[mid].id == ID) {
                return mid;
            }

            // If the target is greater, ignore the left half
            if (buffer.t[mid].id < ID) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1; // ID not found
    }
}

/* File */
void Creer_du_fichierchainee(FILE *ms, FILE *f, int nbEtudiant, char nom[20], int sorted) {
    // Debugging: Start of the function
    printf("Starting Creer_du_fichiertrieechainee\n");

    // Create metadata
    FDmeta m;
    m.nbEtudiant = nbEtudiant;
    strcpy(m.FDnom, nom);
    m.taille = ceil((double)nbEtudiant / FB);
    m.adresse = allouer(ms);
    printf("Metadata created: nbEtudiant=%d, FDnom=%s, taille=%d, adresse=%d\n", m.nbEtudiant, m.FDnom, m.taille, m.adresse);

    update_Allocation_Table(ms, m.adresse, 1);

    BLOC_meta meta;
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fread(&meta, sizeof(BLOC_meta), 1, ms);
    printf("Initial meta block read: ne=%d\n", meta.ne);

    if (meta.ne == FB) {
        int i;
        for (i = 1; i < NbBlocmeta; ++i) {
            fread(&meta, sizeof(BLOC_meta), 1, ms);
            printf("Checking meta block %d: ne=%d\n", i, meta.ne);
            if (meta.ne != FB) {
                break;
            }
        }
        if (i == NbBlocmeta - 1) {
            perror("Disk is full");
            return; // Exit if disk is full
        } else {
            meta.t[meta.ne] = m;
            meta.ne++;
            fseek(ms, -1 * sizeof(BLOC_meta), SEEK_CUR);
            fwrite(&meta, sizeof(BLOC_meta), 1, ms);
            printf("Meta block updated: ne=%d\n", meta.ne);
        }
    } else {
        meta.t[meta.ne] = m;
        meta.ne++;
        fseek(ms, -1 * sizeof(BLOC_meta), SEEK_CUR);
        fwrite(&meta, sizeof(BLOC_meta), 1, ms);
        printf("Meta block directly updated: ne=%d\n", meta.ne);
    }

    // Add students to the file
    Tetudiant *T;
    T = (Tetudiant *)malloc(nbEtudiant * sizeof(Tetudiant));
    for (int i = 0; i < nbEtudiant; i++) {
        printf("ID : ");
        scanf("%d", &T[i].id);
        getchar(); // Clear the buffer after reading an integer

        printf("Name : ");
        fgets(T[i].nom, sizeof(T[i].nom), stdin);
        T[i].nom[strcspn(T[i].nom, "\n")] = '\0'; // Remove the newline

        printf("Surname : ");
        fgets(T[i].prenom, sizeof(T[i].prenom), stdin);
        T[i].prenom[strcspn(T[i].prenom, "\n")] = '\0'; // Remove the newline

        printf("Section : ");
        scanf(" %c", &T[i].sec); // Notice the space before %c to skip whitespace
        getchar(); // Clear the buffer after reading a character

        T[i].etat = 1;

        printf("Student %d: ID=%d, Name=%s, Surname=%s, Section=%c\n", i, T[i].id, T[i].nom, T[i].prenom, T[i].sec);
    }

    // Sort students by ID
    if (sorted == 1) {
        for (int i = 0; i < nbEtudiant; i++) {
            for (int j = i + 1; j < nbEtudiant; ++j) {
                if (T[i].id > T[j].id) {
                    Tetudiant temp;
                    temp = T[j];
                    T[j] = T[i];
                    T[i] = temp;
                }
            }
        }
    }
    printf("Students sorted by ID\n");
    for (int i = 0; i < nbEtudiant; i++) {
        printf("Sorted Student %d: ID=%d, Name=%s, Surname=%s, Section=%c\n", i, T[i].id, T[i].nom, T[i].prenom, T[i].sec);
    }

    int size = m.nbEtudiant;
    BLOC_ch buffer;
    int j = 0;
    int i;
    while (size > FB) {
        for (i = 0; i < FB; i++) {
            buffer.t[i] = T[j];
            j++;
        }
        size -= FB;
        buffer.ne = FB;

        fwrite(&buffer, sizeof(BLOC_ch), 1, f);
        printf("Written buffer with %d students, remaining size=%d\n", buffer.ne, size);
        buffer.ne = 0;
        buffer.next = 0;
    }

    if (j >= m.nbEtudiant) {
        printf("All students written to file\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        buffer.t[i] = T[j + i];
        buffer.ne = size;
    }
    buffer.ne = size;
    buffer.next = 0;
    fwrite(&buffer, sizeof(BLOC_ch), 1, f);
    printf("Final buffer written with remaining students\n");

    // Free allocated memory
    free(T);
    printf("Function completed\n");
}

void chargement_fichier_chainee(FILE *ms, FILE *f, char nom[20]) {
    int flag = 0;
    BLOC_ch buffer;

    // Position in the file
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    BLOC_meta bloCmeta;
    FDmeta meta;

    int i = 0, trouv = 0, cpt = 0;

    printf("[DEBUG] Searching for metadata for the file: %s\n", nom);

    // Search for the metadata in the master file
    do {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("[DEBUG] Reading metadata block %d\n", cpt);

        for (i = 0; i < bloCmeta.ne; i++) {
            printf("[DEBUG] Checking file: %s\n", bloCmeta.t[i].FDnom);
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                meta = bloCmeta.t[i];
                trouv = 1;
                printf("[DEBUG] File found: %s, Address: %d, Size: %d\n", meta.FDnom, meta.adresse, meta.taille);
                break;
            }
        }
        cpt++;
    } while (trouv == 0 && cpt <= 10);

    if (cpt > 10) {
        perror("[ERROR] Disk is full: unable to find the metadata.");
    } else {
        printf("[DEBUG] Starting to read the file and insert it into secondary memory.\n");

        i = 0;
        while (i < meta.taille) {
            // Read blocks from the source file
            fseek(f, i * sizeof(BLOC_ch), SEEK_SET);
            fread(&buffer, sizeof(BLOC_ch), 1, f);
            printf("[DEBUG] Reading block %d from the source file.\n", i);

            for (int j = 0; j < buffer.ne; ++j) {
                printf("[DEBUG] Record found: %s\n", buffer.t[j].prenom);
            }

            // Manage the address of the next block
            if (i == meta.taille - 1) {
                buffer.next = -1; // Last block, no successor
                printf("[DEBUG] Last block: next = -1\n");
            } else {
                buffer.next = allouer(ms); // Allocate a new block
                printf("[DEBUG] New block allocated at address: %d\n", buffer.next);

                // Update the allocation table
                update_Allocation_Table(ms, buffer.next, 1);
                printf("[DEBUG] Allocation table updated for block %d\n", buffer.next);
            }

            // Insert the block into secondary memory
            fseek(ms, (meta.adresse) * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
            printf("[DEBUG] Block %d inserted at address %d\n", i, meta.adresse);

            // Update the address for the next block
            meta.adresse = buffer.next;
            i++;
        }
    }
}

void Displayfile(FILE *ms, char filename[20]) {
    BLOC_meta blockMeta;
    BLOC_ch buffer;
    FDmeta meta;
    int found = 0;

    // Move the file pointer to the start of metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);

    // Search for the metadata using the filename
    while (!found && fread(&blockMeta, sizeof(BLOC_meta), 1, ms) == 1) {
        for (int i = 0; i < blockMeta.ne; i++) {
            if (strcmp(blockMeta.t[i].FDnom, filename) == 0) {
                found = 1;
                meta = blockMeta.t[i];
                break;
            }
        }
    }

    if (!found) {
        printf("[ERROR] The file '%s' does not exist.\n", filename);
        return;
    }

    printf("[INFO] The file '%s' exists. Loading data...\n", filename);
    printf("[DEBUG] Metadata: Address = %d, Size = %d\n", meta.adresse, meta.taille);

    // Validate metadata address
    if (meta.adresse < 0 || meta.adresse >= NbBloc) {
        printf("[ERROR] Invalid starting address in metadata: %d\n", meta.adresse);
        return;
    }

    // Start displaying the file data block by block
    int current_address = meta.adresse;
    int block_count = 1;

    while (current_address != -1) {
        // Calculate the offset of the current block
        long block_offset = current_address * sizeof(BLOC_ch)
                          + NbBloc * sizeof(int)
                          + NbBlocmeta * sizeof(BLOC_meta);

        // Validate block offset
        if (block_offset < 0) {
            printf("[ERROR] Invalid block offset: %ld\n", block_offset);
            return;
        }

        fseek(ms, block_offset, SEEK_SET);

        // Read the current block from secondary memory
        if (fread(&buffer, sizeof(BLOC_ch), 1, ms) != 1) {
            printf("[ERROR] Unable to read block at address %d. Aborting...\n", current_address);
            return;
        }

        printf("[INFO] Block %d:\n", block_count);
        for (int j = 0; j < buffer.ne; j++) {
            printf("  ID: %d, Name: %s, Surname: %s, Section: %c, Status: %d\n",
                   buffer.t[j].id, buffer.t[j].nom, buffer.t[j].prenom, buffer.t[j].sec, buffer.t[j].etat);
        }
        printf("  Next Block Address: %d\n", buffer.next);

        // Validate the next address
        if (buffer.next < -1 || buffer.next >= NbBloc) {
            printf("[ERROR] Invalid next block address: %d\n", buffer.next);
            return;
        }

        current_address = buffer.next;
        block_count++;
    }

    printf("[INFO] File loading completed.\n");
}

void Rename_File_Ch(FILE *ms, char name[20], char newname[20]) {
    BLOC_meta bloCmeta;
    FDmeta meta;
    int i, found = 0, blocmetacount, blocmetaindex;

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!found && fread(&bloCmeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, name) == 0) {
                found = 1;
                meta = bloCmeta.t[i];
                blocmetaindex = blocmetacount;
                break;
            }
        }
        if (!found) {
            blocmetacount++;
        }
    }

    if (!found) {
        printf("File not found.\n");
        return;
    }

    // Rename the file
    strcpy(bloCmeta.t[i].FDnom, newname);

    // Save the metadata
    fseek(ms, NbBloc * sizeof(int) + blocmetaindex * sizeof(BLOC_meta), SEEK_SET);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);
}

void supprime_fichier_chainee(FILE *ms, char nom[20]) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer;
    FDmeta meta;
    int i, trouv = 0, currentAddress, metaIndex = -1, blocmetaindex = -1;

    printf("Starting to search for file metadata...\n");

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    for (int blocmetacount = 0; blocmetacount < NbBlocmeta; blocmetacount++) {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Searching in metadata block %d...\n", blocmetacount);
        for (i = 0; i < bloCmeta.ne; i++) {
            printf("Checking file: %s\n", bloCmeta.t[i].FDnom);
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                trouv = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
                printf("File found: %s\n", nom);
                break;
            }
        }
        if (trouv) break;
    }

    if (!trouv) {
        printf("File not found.\n");
        return;
    }

    // Initialize variables
    currentAddress = meta.adresse;
    int k = 0;

    printf("Starting to delete file blocks...\n");

    // Set all file blocks to 0
    while (meta.taille > k && currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        printf("Deleting block %d...\n", currentAddress);

        // Update the allocation table
        update_Allocation_Table(ms, currentAddress, 0);
        printf("Allocation table updated for block %d (freed).\n", currentAddress);

        currentAddress = buffer.next; // Move to the next block
        k++;
    }

    printf("Updating metadata...\n");

    // Update metadata
    meta.nbEtudiant = 0; // number of students
    meta.taille = 0;     // number of blocks
    meta.adresse = -1;
    strcpy(meta.FDnom, " ");
    bloCmeta.t[metaIndex] = meta;

    // Save metadata
    fseek(ms, NbBloc * sizeof(int) + blocmetaindex * sizeof(BLOC_meta), SEEK_SET);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);

    printf("File deletion completed successfully.\n");
}

void add_student_to_unsorted_linked_file(FILE *ms, char fileName[20], Tetudiant student) {
    BLOC_ch buffer1, buffer2;
    BLOC_meta bloCmeta;
    FDmeta meta;
    int indexmeta = -1, blockNumber = 0, metaBlockIndex = 0, metaNumber = 0, found = 0;

    printf("Debug: Starting the function.\n");

    // Search for metadata matching the file name
    do {
        printf("Debug: Searching in block %d.\n", blockNumber);
        fseek(ms, blockNumber * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Read metadata block %d.\n", blockNumber);

        for (metaNumber = 0; metaNumber < bloCmeta.ne; ++metaNumber) {
            printf("Debug: Checking metadata entry %d.\n", metaNumber);
            if (strcmp(bloCmeta.t[metaNumber].FDnom, fileName) == 0) {
                printf("Debug: Found matching metadata for file '%s'.\n", fileName);
                found = 1;
                indexmeta = metaNumber;
                meta = bloCmeta.t[metaNumber];
                metaBlockIndex = blockNumber;
                break;
            }
        }
        blockNumber++;
    } while (!found && blockNumber < NbBlocmeta);

    if (!found) {
        printf("Debug: File '%s' does not exist.\n", fileName);
        return;
    }

    printf("Debug: File '%s' exists. Metadata found.\n", fileName);

    int p = meta.adresse;

    // Traverse to the last block
    while (p != -1) {
        printf("Debug: Traversing to block %d.\n", p);
        fseek(ms, p * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer1, sizeof(BLOC_ch), 1, ms);
        printf("Debug: Read block %d with %d entries.\n", p, buffer1.ne);
        if (buffer1.next == -1) break; // Stop if this is the last block
        p = buffer1.next;
    }

    if (buffer1.ne < FB) {
        // If there's space in the last block, add the student
        printf("Debug: Adding student to the current block.\n");
        buffer1.t[buffer1.ne] = student;
        buffer1.ne++;

        printf("Debug: Student added. ID: %d, Name: %s, Surname: %s, Section: %c\n",
               student.id, student.nom, student.prenom, student.sec);

        // Update MS
        fseek(ms, -sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&buffer1, sizeof(BLOC_ch), 1, ms);
        printf("Debug: Updated the block in MS.\n");

        // Update metadata
        bloCmeta.t[indexmeta].nbEtudiant++;
        fseek(ms, metaBlockIndex * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
        fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Updated metadata. Total students: %d.\n", bloCmeta.t[indexmeta].nbEtudiant);


    } else {
        // If the last block is full, allocate a new block and link it
        printf("Debug: Current block is full. Allocating a new block.\n");
        int newBlock = allouer ( ms ) ;

        //int newBlock = Manage_Storage_Space_Ch(ms, 1); // Function to allocate a new block
        if (newBlock == -1) {
            printf("Error: Unable to allocate new block. Disk may be full.\n");
            return;
        }
        buffer1.next = newBlock;

        // Update the previous block to point to the new block
        fseek(ms, p * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);

        fwrite(&buffer1, sizeof(BLOC_ch), 1, ms);
        printf("Debug: Updated the previous block to point to the new block.\n");
        printf("Debug: Allocated new block at position %d.\n", buffer1.next);
        update_Allocation_Table(ms, buffer1.next, 1);


        // Initialize the new block
        buffer2.ne = 1;
        buffer2.t[0] = student;
        buffer2.next = -1;

        printf("Debug: Student added to new block. ID: %d, Name: %s, Surname: %s, Section: %c\n",
               student.id, student.nom, student.prenom, student.sec);

        // Write new block to MS
        fseek(ms, newBlock * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fwrite(&buffer2, sizeof(BLOC_ch), 1, ms);
        printf("Debug: Wrote the new block to MS.\n");

        // Update metadata
        bloCmeta.t[indexmeta].nbEtudiant++;
        bloCmeta.t[indexmeta].taille++;
        fseek(ms, metaBlockIndex * sizeof(BLOC_meta) + NbBloc * sizeof(int), SEEK_SET);
        fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Updated metadata. Total blocks: %d.\n", bloCmeta.t[indexmeta].taille);


    }

    printf("Debug: Function completed.\n");
}

void add_student_to_sorted_linked_file(FILE *ms, char nom[20],Tetudiant x) {
    BLOC_meta bloCmeta;
    FDmeta meta;
    FDmeta final;
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    int i = 0, trouv = 0;
    int cpt = 0;

    // Debug: Starting metadata search
    printf("DEBUG: Starting search for metadata for file: %s\n", nom);

    // Search for metadata in the MS using the file name
    do {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("DEBUG: Reading metadata block #%d\n", cpt);
        for (i = 0; i < bloCmeta.ne; i++) {
            printf("DEBUG: Checking metadata entry: %s\n", bloCmeta.t[i].FDnom);
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                meta = bloCmeta.t[i];
                trouv = 1;
                printf("DEBUG: Metadata found - Address: %d, nbEtudiant: %d, taille: %d\n", meta.adresse,
                       meta.nbEtudiant, meta.taille);
                break;
            }
        }
        cpt++;
    } while (trouv == 0 && cpt <= 10);

    if (cpt > 10) {
        perror("ERROR: Disk is full, metadata not found");
        return;
    }

    // Move to the starting address of the linked list in the file
    BLOC_ch buffer;// Temporary buffer to hold a block of students
    final = meta;
    int l=0;

    meta.nbEtudiant++;
    meta.taille = ceil((double) meta.nbEtudiant / FB);


    int n = meta.nbEtudiant + 1;  // Ensure this value is correct
    if (n <= 0) {
        printf("ERROR: Invalid number of students (%d) in metadata\n", n);
        return;
    }
    Tetudiant *A = (Tetudiant *) malloc(n * sizeof(Tetudiant));
    if (A == NULL) {
        perror("ERROR: Memory allocation failed");
        printf("Attempted to allocate %lu bytes\n", n * sizeof(Tetudiant));
        return;
    }
    printf("DEBUG: Successfully allocated memory for %d students\n", n);

    int j = 0;
    int k = 0;
    printf("DEBUG: Reading blocks to populate student array...\n");

    // Read all blocks and copy their data into the temporary array A
    fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta) * sizeof(BLOC_meta) + (meta.adresse) * sizeof(BLOC_ch), SEEK_SET);
    printf("DEBUG: Moved to linked list address: %d\n", meta.adresse);
    while (final.taille > k) {
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        printf("DEBUG: Read block #%d with next = %d, ne = %d\n", k, buffer.next, buffer.ne);
        for (int i = 0; i < buffer.ne; ++i) {
            A[j] = buffer.t[i];
            printf("DEBUG: Student ID: %d added to array at index %d\n", A[j].id, j);
            j++;
        }

        fseek(ms, NbBloc * sizeof(int) + (NbBlocmeta) * sizeof(BLOC_meta) + (buffer.next) * sizeof(BLOC_ch), SEEK_SET);

        k++;
    }

    // Insert the new student
    printf("DEBUG: Inserting new student with ID: %d\n", x.id);
    int inserted = 0;
    i = 0;

// Check if the new student should be the first element
    for (i = meta.nbEtudiant - 1; (i >= 0 && A[i].id > x.id); i--) {
        A[i + 1] = A[i];  // Shift elements to the right
    }
    // Insert the value at the found position
    A[i + 1] = x;
    if (i + 1 >= n) {
        printf("ERROR: Array index out of bounds\n");
        free(A);
        return;
    }

    // Write updated data back into the file blocks
    printf("DEBUG: Writing updated blocks back to the file...\n");

    j = 0;
    k = 0;
    trouv=0;
    int size = meta.nbEtudiant;
    int temp = 0;
    trouv=0;

    while (k < final.taille) {
        fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + meta.adresse * sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);


        if (size > FB) {
            for (int i = 0; i < FB; ++i) {
                buffer.t[i] = A[j];
                printf("DEBUG: Writing student ID: %d to block %d at index %d\n", A[j].id, k, i);
                j++;
            }
            size -= FB;
            buffer.ne = FB;
        } else {
            for (int i = 0; i < size; ++i) {
                buffer.t[i] = A[j];
                printf("DEBUG: Writing student ID: %d to block %d at index %d\n", A[j].id, k, i);
                j++;
            }
            buffer.ne = size;
        }

        if(buffer.next!=-1) {
            fseek(ms, -1 * sizeof(BLOC_ch), SEEK_CUR);
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
            meta.adresse=buffer.next;
        }

        k++;
        printf("DEBUG: Allocated new block: %d at iteration %d\n", cpt, k);
        printf("DEBUG: Current buffer.next = %d\n", buffer.next);
        printf("DEBUG: Updated meta.adresse = %d\n", meta.adresse);
    }
    if(j!=meta.nbEtudiant){
        cpt= allouer(ms);
        if(cpt!=-1){
            buffer.next=cpt;
            fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + meta.adresse * sizeof(BLOC_ch), SEEK_SET);
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
            buffer.t[0]=A[j];
            buffer.ne=1;
            buffer.next=-1;
            fseek(ms, NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta) + cpt * sizeof(BLOC_ch), SEEK_SET);
            fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
            update_Allocation_Table(ms,cpt,1);
        }else{
            perror("DISK IS FULL");
        }
    }else{
        fseek(ms, -1 * sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
    }
    trouv=0;
    cpt=0;
    final.nbEtudiant++;
    final.taille=meta.taille;
    fseek(ms,NbBloc*sizeof(int),SEEK_SET);
    while (!trouv && cpt <= 10) {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Reading block meta #%d\n", cpt + 1);
        for (int i = 0; i < bloCmeta.ne; i++) {
            printf("Debug: Comparing %s with %s\n", bloCmeta.t[i].FDnom, nom);
            if (strcmp(bloCmeta.t[i].FDnom, nom) == 0) {
                bloCmeta.t[i]=final;  // Copy matching metadata
                trouv = 1;
                fseek(ms,-1* sizeof(BLOC_meta),SEEK_CUR);
                fwrite(&bloCmeta, sizeof(BLOC_meta),1,ms);
                printf("Debug: Found matching metadata! Address: %d, Taille: %d\n", final.adresse, final.taille);
                printf("updating the Metadata ");
            }
        }
        cpt++;
    }
    if(!trouv){
        perror("failed to update the metadata");
    }
    printf("DEBUG: Insertion completed successfully\n");

}

void defragmentation_fichier_chainee(FILE *ms, FILE *f, char nom[20]) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
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
    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant ) * sizeof(Tetudiant));
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    i=0;
    printf("Size: %d\n", meta.taille);
    // Traverse the blocks of the file
    while (i<meta.taille) {

        printf("Reading block at address: %d\n", currentAddress);
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        printf("Block read: %d students, Next: %d\n", buffer.ne, buffer.next);

        for (int k = 0; k< buffer.ne; k++) {
            printf("Student ID: %d\n", buffer.t[k].id);
            if (buffer.t[k].etat == 1) {
                arr[nbetudiant] = buffer.t[k];
                nbetudiant++;
            }
        }

        if (currentAddress != meta.adresse) {
            printf("Updating allocation table for block: %d\n", currentAddress);
            update_Allocation_Table(ms, currentAddress, 0);
        }
        i++;
        printf("%d", i);
        currentAddress = buffer.next;
    }

    printf("Total number of students after defragmentation: %d\n", nbetudiant);
    rewind(f);
    i = 0;
    while (i < nbetudiant) {
        int j = 0;
        BLOC_ch buffer;
        buffer.next = 0;
        buffer.ne = 0;
        while (i < nbetudiant && j < FB) {
            buffer.t[j] = arr[i];
            buffer.ne++;
            i++;
            j++;
        }
        blocCount++;

        fwrite(&buffer, sizeof(BLOC_ch), 1, f);
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
    chargement_fichier_chainee(ms, f, nom);
    printf("Defragmentation completed.\n");
    printf("Number of students updated: %d\n", nbetudiant);
    printf("Number of blocks updated: %d\n", blocCount);

    free(arr);
}

void Search_Linked_File(FILE *ms, char name[20], int id, int p[2], int sorted) {
    p[0] = -1; // Initialize the result to -1 by default (position)
    p[1] = -1; // Initialize the block to -1 by default (block number)
    BLOC_meta bloCmeta;
    FDmeta meta;
    int z = sizeof(int) * NbBloc;
    printf("Debug: Calculated z = %d\n", z);

    fseek(ms, NbBloc * sizeof(int), SEEK_SET);  // Move to start of metadata
    printf("Debug: fseek to metadata completed.\n");

    int found = 0;
    int count = 0;

    // Search for metadata in the MS using the file name
    while (!found && count <= 10) {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Reading block meta %d\n", count + 1);
        for (int i = 0; i < bloCmeta.ne; i++) {
            printf("Debug: Comparing %s with %s\n", bloCmeta.t[i].FDnom, name);
            if (strcmp(bloCmeta.t[i].FDnom, name) == 0) {
                meta = bloCmeta.t[i];  // Copy matching metadata
                found = 1;
                printf("Debug: Found matching metadata! Address: %d, Size: %d\n", meta.adresse, meta.taille);
                break;
            }
        }
        count++;
    }

    if (!found) {
        perror("File not found or disk metadata exhausted");
        return;
    }

    BLOC_ch buffer;
    // Move to the first block in the chain
    fseek(ms, z + (NbBlocmeta) * sizeof(BLOC_meta) + meta.adresse * sizeof(BLOC_ch), SEEK_SET);
    printf("Debug: fseek to first block at address %d completed.\n", meta.adresse);

    fread(&buffer, sizeof(BLOC_ch), 1, ms);
    printf("Debug: Read first block. Buffer details - Next: %d, NE: %d\n", buffer.next, buffer.ne);

    int block = 1; // Start block counting from 1

    if (sorted) {
        // Read the first block and search
        int mov = SortedSearch(buffer, id); // This function should return the index within the block
        printf("Debug: SortedSearch returned %d\n", mov);

        if (mov != -1) {
            printf("Found student at block %d, position %d\n", block, mov);
            p[0] = mov;   // Position in block
            p[1] = block; // Block number
            return;
        }

        // Continue searching in linked blocks
        while (buffer.next != -1) {
            block++;
            printf("Debug: Moving to next block. Address: %d\n", buffer.next);
            fseek(ms, z + NbBlocmeta * sizeof(BLOC_meta) + buffer.next * sizeof(BLOC_ch), SEEK_SET);
            fread(&buffer, sizeof(BLOC_ch), 1, ms);

            printf("Debug: Read next block %d. Buffer details - Next: %d, NE: %d\n", block, buffer.next, buffer.ne);
            mov = SortedSearch(buffer, id);
            printf("Debug: SortedSearch in block %d returned %d\n", block, mov);

            if (mov != -1) {
                printf("Found student at block %d, position %d\n", block, mov);
                p[0] = mov;   // Position in block
                p[1] = block; // Block number
                return;
            }
        }

        // If not found
        perror("ID not found in file");
    } else {
        // Unsorted search
        printf("The file exists.\n");
        found = 0;
        int block_number = 1; // Start block counting from 1
        int next = meta.adresse;

        while (block_number <= meta.taille && next != -1) {
            fseek(ms, z + (next) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
            fread(&buffer, sizeof(BLOC_ch), 1, ms);
            for (int i = 0; i < buffer.ne; i++) {
                if (buffer.t[i].id == id) {
                    printf("Found student at block %d, position %d\n", block_number, i);
                    p[0] = i;             // Position in block
                    p[1] = block_number;  // Block number
                    return;
                }
            }
            block_number++;
            next = buffer.next;
        }

        if (!found) {
            printf("ID not found in file\n");
        }
    }
}

void physical_deletion_from_linked_file(FILE *ms, FILE *f, char name[20], int id) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer, tempBuffer;
    FDmeta meta;
    int i, found = 0, currentAddress, metaIndex, previousAddress, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    printf("Starting physical deletion of the file %s for the student with ID: %d\n", name, id);

    // Locate the metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!found && fread(&bloCmeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            printf("Checking file: %s\n", bloCmeta.t[i].FDnom);
            if (strcmp(bloCmeta.t[i].FDnom, name) == 0) {
                found = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
                printf("File found: %s (Index: %d, BlocMetaIndex: %d)\n", name, metaIndex, blocmetaindex);
            }
        }
        blocmetacount++;
    }

    if (!found) {
        printf("File not found.\n");
        return;
    }

    printf("Number of students before deletion: %d\n", meta.nbEtudiant);

    // Create an array for the remaining students
    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant - 1) * sizeof(Tetudiant));
    currentAddress = meta.adresse;
    int nbetudiant = 0, blocCount = 0;
    i = 0;
    printf("Size %d\n", meta.taille);
    // Traverse the blocks of the file
    while (i < meta.taille) {

        printf("Reading block at address: %d\n", currentAddress);
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        printf("Block read: %d students, Next: %d\n", buffer.ne, buffer.next);

        for (int k = 0; k < buffer.ne; k++) {
            printf("Student ID: %d\n", buffer.t[k].id);
            if (buffer.t[k].id != id) {
                arr[nbetudiant] = buffer.t[k];
                nbetudiant++;
            } else {
                printf("Student with ID %d found and will be deleted.\n", buffer.t[k].id);
            }
        }

        if (currentAddress != meta.adresse) {
            printf("Updating allocation table for block: %d\n", currentAddress);
            update_Allocation_Table(ms, currentAddress, 0);
        }
        i++;
        printf("%d", i);
        currentAddress = buffer.next;
    }

    printf("Total number of students after deletion: %d\n", nbetudiant);

    // Reset the file
    // fclose(f);
    // f = freopen(NULL, "wb+", f);
    rewind(f);
    i = 0;
    while (i < nbetudiant) {
        int j = 0;
        BLOC_ch buffer;
        buffer.next = 0;
        buffer.ne = 0;
        while (i < nbetudiant && j < FB) {
            buffer.t[j] = arr[i];
            buffer.ne++;
            i++;
            j++;
        }
        blocCount++;
        fwrite(&buffer, sizeof(BLOC_ch), 1, f);
        printf("Block written: %d students\n", buffer.ne);
    }

    // Update metadata
    meta.nbEtudiant = nbetudiant;
    meta.taille = blocCount;
    bloCmeta.t[metaIndex] = meta;

    // Save the metadata
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    fseek(ms, blocmetaindex * sizeof(BLOC_meta), SEEK_CUR);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);

    printf("Metadata updated: Number of students = %d, Number of blocks = %d, address %d\n", meta.nbEtudiant, meta.taille, meta.adresse);

    // Load the file into secondary memory
    chargement_fichier_chainee(ms, f, name);
    printf("Physical deletion completed.\n");
    printf("Updated number of students: %d\n", nbetudiant);
    printf("Updated number of blocks: %d\n", blocCount);

    free(arr);
}

void logical_deletion_from_linked_file(FILE *ms, FILE *f, char name[20], int id) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer;
    FDmeta meta;
    int i, found = 0, currentAddress, metaIndex, previousAddress = -1;
    int blocmetacount = 0, blocmetaindex;

    printf("DEBUG: Starting logical_deletion_from_linked_file...\n");

    if (ms == NULL || f == NULL) {
        printf("DEBUG: File pointer is NULL. Exiting.\n");
        printf("Error: Unable to open file.\n");
        exit(1);
    }

    // Step 1: Locate metadata based on file name
    printf("DEBUG: Searching for metadata for file '%s'...\n", name);
    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Go to metadata section
    printf("DEBUG: Moved to metadata section (offset = %ld).\n", NbBloc * sizeof(int));

    while (!found && fread(&bloCmeta, sizeof(BLOC_meta), 1, ms)) {
        printf("DEBUG: Reading metadata block %d...\n", blocmetacount);
        for (i = 0; i < bloCmeta.ne; i++) {
            printf("DEBUG: Checking file '%s' in metadata index %d...\n", bloCmeta.t[i].FDnom, i);
            if (strcmp(bloCmeta.t[i].FDnom, name) == 0) {
                printf("DEBUG: File '%s' found in metadata.\n", name);
                found = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
                blocmetaindex = blocmetacount;
                break;
            }
        }
        blocmetacount++;
    }

    if (!found) {
        printf("DEBUG: File '%s' not found in metadata. Exiting function.\n", name);
        printf("File not found.\n");
        return;
    }

    // Step 2: Initialization of variables
    currentAddress = meta.adresse;
    found = 0; // Reset `found` for finding the student
    printf("DEBUG: File metadata located. Starting address = %d, number of students = %d\n", currentAddress, meta.nbEtudiant);
    printf("DEBUG: Reset 'found' to %d for student search.\n", found);

    // Step 3: Traverse the blocks in the linked list to find the student
    int blocCount = 0;
    while (currentAddress != -1) {
        printf("DEBUG: Reading block at address %d...\n", currentAddress);
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        printf("DEBUG: Moved to block address %d (offset = %ld).\n", currentAddress,
               NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta));
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        printf("DEBUG: Block read. Number of entries = %d, Next block = %d.\n", buffer.ne, buffer.next);

        for (i = 0; i < buffer.ne; i++) {
            printf("DEBUG: Checking student ID %d in block %d, position %d...\n", buffer.t[i].id, currentAddress, i);
            if (buffer.t[i].id == id) {
                // Logical deletion
                printf("DEBUG: Student with ID %d found. Performing logical deletion...\n", id);
                buffer.t[i].etat = 0;
                fseek(ms, -1 * sizeof(BLOC_ch), SEEK_CUR); // Update the block in memory
                printf("DEBUG: Writing updated block back to MS.\n");
                fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
                found = 1;
                break;
            }
        }

        if (found) {
            printf("DEBUG: Student logically deleted. Exiting loop.\n");
            break;
        }

        currentAddress = buffer.next; // Move to the next block
        printf("DEBUG: Moving to next block. Address = %d.\n", currentAddress);
        blocCount++;
    }

    if (!found) {
        printf("DEBUG: Student with ID %d not found. Exiting function.\n", id);
        printf("Student not found.\n");
        return;
    }

    // Step 4: Update metadata
    meta.nbEtudiant -= 1;
    printf("DEBUG: Updated metadata. New student count = %d.\n", meta.nbEtudiant);
    bloCmeta.t[metaIndex] = meta;

    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Go to metadata section
    fseek(ms, blocmetaindex * sizeof(BLOC_meta), SEEK_CUR); // Go to specific metadata block
    printf("DEBUG: Writing updated metadata to block index %d.\n", blocmetaindex);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);

    printf("DEBUG: Logical deletion process completed successfully.\n");
    printf("Logical deletion completed.\n");
    printf("Updated number of students: %d\n", meta.nbEtudiant);
}


