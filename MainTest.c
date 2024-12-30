// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Constants
#define FB 5                // Number of students per block
#define NbBloc 50           // Total number of blocks
#define NbBlocmeta 10       // Number of metadata blocks
#define Nbmeta 50           // Number of metadata records
#define MAX_NAME_LEN 20

// Structures
typedef struct Tetudiant {
    int etat;               // Record state
    int id;                 // Student ID
    char nom[20];           // First name
    char prenom[20];        // Last name
    char sec[20];           // Section
} Tetudiant;

typedef struct FDmeta {
    char FDnom[20];         // File name
    int taille;             // File size
    int nbEtudiant;         // Number of students
    int adresse;            // Address in memory
    int modeglobal;         // 0: contiguous, 1: linked (chainee)
    int modeinterne;        // 0: unsorted, 1: sorted
} FDmeta;

typedef struct BLOC_ch {
    Tetudiant t[FB];        // Array of student records
    int ne;                 // Number of entries
    int next;               // Pointer to the next block
} BLOC_ch;

typedef struct BLOC_co {
    Tetudiant t[FB];        // Array of student records
    int ne;                 // Number of entries
} BLOC_co;

typedef struct BLOC_meta {
    FDmeta t[FB];           // Array of file metadata
    int ne;                 // Number of entries
} BLOC_meta;

typedef struct BLOC_meta_ch {
    FDmeta t[FB];           // Array of file metadata for linked files
    int ne;                 // Number of entries
    int next;               // Pointer to the next block
} BLOC_meta_ch;

typedef struct Position {
    int nbrbloc;
    int mov;
} Position;

// Disk and Memory Management Functions
void Initialize_Disk_Ch(FILE *ms);
void Initialize_Disk_Co(FILE *ms);
void update_Allocation_Table(FILE *ms, int block_address, int value);
void update_Allocation_Table_co(FILE *ms, int block_address, int value);
void empty_MS_Ch(FILE *ms);
void empty_MS_Co(FILE *ms);
int Manage_Storage_Space_Ch(FILE *ms, int student_number);
int Manage_Storage_Space_Co(FILE *ms, int student_number);
void Compact_Disk_Co(FILE *ms);

// File Operations for Chained Allocation
void create_linked_file(FILE *ms, FILE *f, int nbEtudiant, char nom[20], int sorted);
void load_linked_file(FILE *ms, FILE *f, char filename[20]);
void Search_Linked_File(FILE *ms, char name[20], int id, int p[2], int sorted);
void Rename_File_Ch(FILE *ms, char filename[20], char new_filename[20]);
void Delete_File_Ch(FILE *ms, char filename[20]);
void Defragment_File_Ch(FILE *ms, FILE *f, char filename[20]);
void add_student_to_unsorted_linked_file(FILE *ms, FILE *f, char fileName[20], Tetudiant student);

// File Operations for Contiguous Allocation
void Create_File_Co(FILE *ms, FILE *f, char filename[20], int size, int internal_mode);
void Load_File_Co(FILE *ms, FILE *f, char filename[20]);
void Insert_Co(FILE *ms, char filename[30]);
void Delete_File_Co(FILE *ms, char filename[20]);
void Defragment_Co(int first_block_address, int number_of_blocks, FILE *ms);
void Rename_File_Co(FILE *ms, char *old_name, char *new_name);

// Student Record Operations
void Add_Student_Unsorted_File_Ch(FILE *ms, FILE *f, char filename[20]);
void Insert_In_Sorted_File(FILE *ms, FDmeta meta, Tetudiant student);
Position Search_Sorted_File_Ch(int id, FILE *ms, char filename[20]);
void Search_Unsorted_File_Ch(FILE *ms, char filename[20], int id, int p[2], FILE *f);
void Search_Co(FILE *ms, int student_id, int *block_number, int *offset);
void Logical_Delete_Record_Co(FILE *ms, int student_id, char filename[30]);
void Physical_Delete_Record_Co(FILE *ms, int student_id, char filename[30]);

// Display Function
void display_file_contents(FILE *f);

// Function implementations
void display_main_menu() {
    printf("Choose an option:\n");
    printf("1. Linked Storage\n");
    printf("2. Contiguous Storage\n");
    printf("3. Exit\n");
}

void display_linked_actions() {
    printf("Choose an action for linked storage:\n");
    printf("1. Create a linked file\n");
    printf("2. Load a linked file\n");
    printf("3. Delete linked struct\n");
    printf("4. Rename a linked file\n");
    printf("5. Defragment a linked file\n");
    printf("6. Insert into a linked file\n");
    printf("7. Search in a linked file\n");
    printf("8. Return to main menu\n");
}

void display_contiguous_actions() {
    printf("Choose an action for contiguous storage:\n");
    printf("1. Create a contiguous file\n");
    printf("2. Load a contiguous file\n");
    printf("3. Delete a contiguous file\n");
    printf("4. Rename a contiguous file\n");
    printf("5. Defragment a contiguous file\n");
    printf("6. Insert into a contiguous file\n");
    printf("7. Search in a contiguous file\n");
    printf("8. Return to main menu\n");
}

int main() {
    int choice;
    int exit_program = 0;

    while (!exit_program) {
        display_main_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                FILE *ms = fopen("linked_storage.dat", "wb+");
                if (ms == NULL) {
                    printf("Error opening file.\n");
                    return 1;
                }
                printf("Initializing disk for linked storage...\n");
                Initialize_Disk_Ch(ms);
                printf("Disk initialized for linked storage.\n");
                fclose(ms);

                int linked_action;
                while (1) {
                    display_linked_actions();
                    scanf("%d", &linked_action);
                    if (linked_action == 8) break; // Return to main menu
                    switch (linked_action) {
                        case 1: {
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "wb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            int nbEtudiant;
                            char nom[20];
                            int sorted;

                            printf("Enter the number of students: ");
                            scanf("%d", &nbEtudiant);
                            getchar(); // Clear the buffer

                            printf("Enter the file name: ");
                            fgets(nom, sizeof(nom), stdin);
                            nom[strcspn(nom, "\n")] = '\0'; // Remove the newline

                            printf("Choose file type (0 for unsorted, 1 for sorted): ");
                            scanf("%d", &sorted);

                            create_linked_file(ms, f, nbEtudiant, nom, sorted);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 2: {
                            printf("Loading a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to load: ");
                            scanf("%s", nom);

                            load_linked_file(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 3: {
                            // Handle deletion of linked structure
                            break;
                        }
                        case 4: {
                            printf("Renaming a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            char nouveaunom[20];
                            printf("Enter the current file name: ");
                            scanf("%s", nom);

                            printf("Enter the new file name: ");
                            scanf("%s", nouveaunom);

                            Rename_File_Ch(ms, nom, nouveaunom);

                            fclose(ms);
                            break;
                        }
                        case 5: {
                            printf("Defragmenting a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            Defragment_File_Ch(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 6: {
                            printf("Inserting into a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            int sorted;
                            printf("Enter the file name: ");
                            scanf("%s", nom);
                            // Student Infroamtions
                            Tetudiant student;
                            // Prompt user for new student information
                            printf("Enter the new student's information:\n");
                            printf("ID: ");
                            scanf("%d", &student.id);
                            printf("Name: ");
                            scanf(" %[^\n]", student.nom);
                            printf("Surname: ");
                            scanf(" %[^\n]", student.prenom);
                            printf("Section: ");
                            scanf(" %[^\n]", student.sec);

                            add_student_to_unsorted_linked_file(ms, f, nom, student);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 7: {
                            printf("Searching in a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            int id;
                            char nom[20];
                            int sorted;
                            int pos[2];
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID to search: ");
                            scanf("%d", &id);

                            printf("Is the file sorted? (1 for yes, 0 for no): ");
                            scanf("%d", &sorted);

                            Search_Linked_File(ms, nom, id, pos, sorted);

                            fclose(ms);
                            break;
                        }
                        default:
                            printf("Invalid action!\n");
                    }
                }
                break;
            }
            case 2: {
                FILE *ms = fopen("contiguous_storage.dat", "wb+");
                if (ms == NULL) {
                    printf("Error opening file.\n");
                    return 1;
                }
                printf("Initializing disk for contiguous storage...\n");
                Initialize_Disk_Co(ms);
                printf("Disk initialized for contiguous storage.\n");
                fclose(ms);

                int contiguous_action;
                while (1) {
                    display_contiguous_actions();
                    scanf("%d", &contiguous_action);
                    if (contiguous_action == 8) break; // Return to main menu
                    switch (contiguous_action) {
                        case 1: {
                            // Handle creation of contiguous file
                            break;
                        }
                        case 2: {
                            // Handle loading of contiguous file
                            break;
                        }
                        case 3: {
                            // Handle deletion of contiguous file
                            break;
                        }
                        case 4: {
                            // Handle renaming of contiguous file
                            break;
                        }
                        case 5: {
                            // Handle defragmentation of contiguous file
                            break;
                        }
                        case 6: {
                            // Handle insertion into a contiguous file
                            break;
                        }
                        case 7: {
                            // Handle searching in a contiguous file
                            break;
                        }
                        default:
                            printf("Invalid action!\n");
                    }
                }
                break;
            }
            case 3:
                exit_program = 1;
                break;
            default:
                printf("Invalid choice!\n");
        }
    }

    return 0;
}

void Initialize_Disk_Ch(FILE *ms){

    int Allocation_Table[NbBloc];

    for(int i=0; i<NbBloc; i++){
        Allocation_Table[i] = 0;  //marquer tous les blocs comme libres
    }
    fseek(ms,0,SEEK_SET);

    fwrite(Allocation_Table,sizeof(int),NbBloc,ms); //écrire la table d'allocation dans le premier bloc

    FDmeta meta;             //valeurs par défaut pour les blocs métadonnées
    BLOC_meta meta_bloc;
    strcpy(meta.FDnom," ");
    meta.taille = 0;
    meta.nbEtudiant = 0;
    meta.adresse = -1;
    meta.modeglobal = 0;
    meta.modeinterne = 0;
    BLOC_ch buffer;
    buffer.ne=0;
    buffer.next=-1;
    fseek(ms,NbBloc * sizeof(int) ,SEEK_SET);  //déplacer le curseur vers le bloc suivant
    //valeurs par défaut pour les blocs métadonnées
    meta_bloc.ne = 0;
    for(int j=0; j<NbBlocmeta; j++){

        for(int i= 0; i<FB; i++){

            meta_bloc.t[i] = meta;
        }
        fwrite(&meta_bloc ,sizeof(BLOC_meta), 1,ms); //écrire les blocs métadonnées dans MS
    }

}

void Initialize_Disk_Co(FILE *ms){

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

int allouer(FILE *ms) {
    int k = -1, table[NbBloc];
    fseek(ms, 0, SEEK_SET);
    fread(table, sizeof(int), NbBloc, ms);
    for (int i = 0; i < NbBloc; i++) {
        if (table[i] == 0) { // Find a free block
            table[i] = 1; // Mark as allocated
            k = i;
            fseek(ms, 0, SEEK_SET);
            fwrite(table, sizeof(int), NbBloc, ms); // Update the allocation table
            break;
        }
    }
    return k; // Return the allocated block address
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

void create_linked_file(FILE *ms, FILE *f, int nbEtudiant, char nom[20], int sorted) {
    // Debugging: Start of the function
    printf("Starting create_linked_file\n");

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
    Tetudiant *T = (Tetudiant *)malloc(nbEtudiant * sizeof(Tetudiant));
    for (int i = 1; i <= nbEtudiant; i++) { // Start count from 1
        printf("ID: ");
        scanf("%d", &T[i - 1].id); // Adjust index to start from 0
        getchar(); // Clear the buffer after reading an integer

        printf("Name: ");
        fgets(T[i - 1].nom, sizeof(T[i - 1].nom), stdin);
        T[i - 1].nom[strcspn(T[i - 1].nom, "\n")] = '\0'; // Remove the newline

        printf("Surname: ");
        fgets(T[i - 1].prenom, sizeof(T[i - 1].prenom), stdin);
        T[i - 1].prenom[strcspn(T[i - 1].prenom, "\n")] = '\0'; // Remove the newline

        printf("Section: ");
        fgets(T[i - 1].sec, sizeof(T[i - 1].sec), stdin);
        T[i - 1].sec[strcspn(T[i - 1].sec, "\n")] = '\0'; // Remove the newline

        T[i - 1].etat = 1;

        printf("Student %d: ID=%d, Name=%s, Surname=%s, Section=%s\n", i, T[i - 1].id, T[i - 1].nom, T[i - 1].prenom, T[i - 1].sec);
    }

    if (sorted) {
        // Sort students by ID using bubble sort
        for (int i = 1; i <= nbEtudiant; i++) {
            for (int j = i + 1; j <= nbEtudiant; ++j) {
                if (T[i - 1].id > T[j - 1].id) {
                    Tetudiant temp = T[j - 1];
                    T[j - 1] = T[i - 1];
                    T[i - 1] = temp;
                }
            }
        }
    }
    printf("Students sorted by ID\n");
    for (int i = 1; i <= nbEtudiant; i++) {
        printf("Sorted Student %d: ID=%d, Name=%s, Surname=%s, Section=%s\n", i, T[i - 1].id, T[i - 1].nom, T[i - 1].prenom, T[i - 1].sec);
    }

    int size = m.nbEtudiant;
    BLOC_ch buffer;
    int j = 0;
    int current_address = m.adresse;

    while (size > 0) {
        buffer.ne = (size > FB) ? FB : size;
        for (int i = 0; i < buffer.ne; i++) {
            buffer.t[i] = T[j];
            j++;
        }
        size -= buffer.ne;

        if (size > 0) {
            buffer.next = allouer(ms);
            update_Allocation_Table(ms, buffer.next, 1);
        } else {
            buffer.next = -1;
        }

        fseek(ms, current_address * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fwrite(&buffer, sizeof(BLOC_ch), 1, ms);

        printf("Written buffer with %d students, remaining size=%d, next address=%d\n", buffer.ne, size, buffer.next);

        current_address = buffer.next;
    }

    free(T);

    // Display the contents of the file
    display_file_contents(f);
}

void display_file_contents(FILE *f) {
    Tetudiant student;

    // Rewind to the beginning of the file
    rewind(f);

    printf("Contents of the file:\n");
    while (fread(&student, sizeof(Tetudiant), 1, f) == 1) {
        printf("ID: %d, Name: %s, Surname: %s, Section: %s, Status: %d\n",
               student.id, student.nom, student.prenom, student.sec, student.etat);
    }
}

void load_linked_file(FILE *ms, FILE *f, char filename[20]) {
    fseek(ms, NbBloc * sizeof(int), SEEK_SET); // Move file pointer to the start of metadata
    BLOC_meta blockMeta;
    BLOC_ch buffer;
    FDmeta meta;
    int i = 0, found = 0;

    // Search for the metadata in the memory storage (MS) using the filename
    do {
        fread(&blockMeta, sizeof(BLOC_meta), 1, ms);
        for (i = 0; i < blockMeta.ne; i++) {
            if (strcmp(blockMeta.t[i].FDnom, filename) == 0) {
                found = 1;
                meta = blockMeta.t[i];
                break;
            }
        }
    } while (!found && !feof(ms));

    if (!found) {
        printf("The file does not exist.\n");
        return;
    }

    printf("The file exists. Loading data...\n");
    int current_address = meta.adresse;
    int block_count = 1;

    while (current_address != -1) {
        fseek(ms, current_address * sizeof(BLOC_ch) + NbBloc * sizeof(int) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);

        printf("Block %d:\n", block_count);
        for (int j = 0; j < buffer.ne; j++) {
            printf("  ID: %d, Name: %s, Surname: %s, Section: %s, Status: %d\n",
                   buffer.t[j].id, buffer.t[j].nom, buffer.t[j].prenom, buffer.t[j].sec, buffer.t[j].etat);
        }
        printf("  Next = %d\n", buffer.next);

        current_address = buffer.next;
        block_count++;
    }

    printf("File loading completed.\n");
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

void Defragment_File_Ch(FILE *ms, FILE *f, char filename[20]) {
    BLOC_meta bloCmeta;
    BLOC_ch buffer;
    FDmeta meta;
    int i, found = 0, currentAddress, metaIndex, blocmetacount, blocmetaindex;
    int allocationTable[NbBloc];

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocmetacount = 0;
    while (!found && fread(&bloCmeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < bloCmeta.ne; i++) {
            if (strcmp(bloCmeta.t[i].FDnom, filename) == 0) {
                found = 1;
                meta = bloCmeta.t[i];
                metaIndex = i;
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

    // Save students in an array and remove them from the file, then recreate and reload the file to physically remove the chosen student
    Tetudiant *arr = (Tetudiant *) malloc((meta.nbEtudiant) * sizeof(Tetudiant));

    // Initialize variables
    currentAddress = meta.adresse;
    int num_students = 0, blocCount = 0;

    // num_students is the index of students in the array and will eventually be the total number of students
    while (currentAddress != -1) {
        fseek(ms, NbBloc * sizeof(int) + (currentAddress) * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_meta), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);
        for (i = 0; i < buffer.ne; i++) {
            if (buffer.t[i].etat != 0) {
                // Keep students who are not logically deleted
                arr[num_students] = buffer.t[i];
                num_students++;
            }
        }
        if (currentAddress != meta.adresse) {
            // Update the allocation table
            update_Allocation_Table(ms, currentAddress, 0);
        }
        currentAddress = buffer.next; // Move to the next block
    }

    // Reset the file
    fclose(f);
    f = fopen(filename, "wb+");
    if (f == NULL) {
        printf("Error opening file.\n");
        free(arr);
        return;
    }

    rewind(f);
    for (i = 0; i < num_students;) {
        BLOC_ch buffer = { .ne = 0, .next = -1 };
        for (int j = 0; j < FB && i < num_students; j++, i++) {
            buffer.t[j] = arr[i];
            buffer.ne++;
        }
        blocCount++;
        fwrite(&buffer, sizeof(buffer), 1, f);
    }

    // Update metadata
    meta.nbEtudiant = num_students; // Number of students
    meta.taille = blocCount;        // Number of blocks
    bloCmeta.t[metaIndex] = meta;

    // Save metadata
    fseek(ms, NbBloc * sizeof(int) + blocmetaindex * sizeof(BLOC_meta), SEEK_SET);
    fwrite(&bloCmeta, sizeof(BLOC_meta), 1, ms);

    // Load the file into memory
    load_linked_file(ms, f, filename);
    printf("Defragmentation completed!\n");
    printf("Updated number of students: %d\n", num_students);
    printf("Updated number of blocks: %d\n", blocCount);

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

void add_student_to_unsorted_linked_file(FILE *ms, FILE *f, char fileName[20], Tetudiant student) {
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

        printf("Debug: Student added. ID: %d, Name: %s, Surname: %s, Section: %s\n",
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

        // Update F
        fseek(f, p * sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer1, sizeof(BLOC_ch), 1, f);
        printf("Debug: Updated the block in file F.\n");
    } else {
        // If the last block is full, allocate a new block and link it
        printf("Debug: Current block is full. Allocating a new block.\n");
        int newBlock = Manage_Storage_Space_Ch(ms, 1); // Function to allocate a new block
        if (newBlock == -1) {
            printf("Error: Unable to allocate new block. Disk may be full.\n");
            return;
        }
        buffer1.next = newBlock;
        printf("Debug: Allocated new block at position %d.\n", buffer1.next);
        update_Allocation_Table(ms, buffer1.next, 1);

        // Update the previous block to point to the new block
        fseek(ms, -sizeof(BLOC_ch), SEEK_CUR);
        fwrite(&buffer1, sizeof(BLOC_ch), 1, ms);
        printf("Debug: Updated the previous block to point to the new block.\n");

        // Initialize the new block
        buffer2.ne = 1;
        buffer2.t[0] = student;
        buffer2.next = -1;

        printf("Debug: Student added to new block. ID: %d, Name: %s, Surname: %s, Section: %s\n",
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

        // Update F
        fseek(f, newBlock * sizeof(BLOC_ch), SEEK_SET);
        fwrite(&buffer2, sizeof(BLOC_ch), 1, f);
        printf("Debug: Wrote the new block to file F.\n");
    }

    printf("Debug: Function completed.\n");
}
