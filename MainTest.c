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
void Load_Linked_File_Ch(FILE *ms, FILE *f, char filename[20]);
void Delete_linked_struct(FILE *ms, FILE *f, char nom[20], int id, int physical);
void Search_In_Sorted_Linked_File(int ID, FILE *ms, char name[20], int *P[2]);
void Rename_File_Ch(FILE *ms, char filename[20], char new_filename[20]);
void Delete_File_Ch(FILE *ms, char filename[20]);
void Defragment_File_Ch(FILE *ms, FILE *f, char filename[20], int id);

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
    printf("3. Delete linked struct (physical or logical)\n"); // Merged option
    printf("4. Rename a linked file\n");
    printf("5. Defragment a linked file\n");
    printf("6. Add a student to an unsorted linked file\n");
    printf("7. Insert into a sorted linked file\n");
    printf("8. Search in a sorted linked file\n");
    printf("9. Search in an unsorted linked file\n");
    printf("10. Return to main menu\n");
}

void display_contiguous_actions() {
    printf("Choose an action for contiguous storage:\n");
    printf("1. Create a contiguous file\n");
    printf("2. Load a contiguous file\n");
    printf("3. Insert into a contiguous file\n");
    printf("4. Delete a contiguous file\n");
    printf("5. Defragment a contiguous file\n");
    printf("6. Rename a contiguous file\n");
    printf("7. Logically delete a record in a contiguous file\n");
    printf("8. Physically delete a record in a contiguous file\n");
    printf("9. Search in a contiguous file\n");
    printf("10. Return to main menu\n");
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
                    if (linked_action == 10) break; // Return to main menu
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

                            load_linked_file(ms, f, nom); // Call the modified function

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 3: {
                            printf("Deleting a linked file...\n");
                            int delete_type;
                            printf("Choose delete type (1 for physical, 2 for logical): ");
                            scanf("%d", &delete_type);

                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            int id;
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID: ");
                            scanf("%d", &id);

                            if (delete_type == 1) {
                                printf("Physically deleting a linked file...\n");
                                Delete_linked_struct(ms, f, nom, id, 1); // Call the physical delete function
                            } else if (delete_type == 2) {
                                printf("Logically deleting a linked file...\n");
                                Delete_linked_struct(ms, f, nom, id, 0); // Call the logical delete function
                            } else {
                                printf("Invalid delete type!\n");
                            }

                            // Close the files after deletion
                            fclose(ms);
                            fclose(f);
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

                            Rename_File_Ch(ms, nom, nouveaunom); // Call the rename function

                            fclose(ms);
                            break;
                        }
                        case 5:
                            printf("Defragmenting a linked file...\n");
                            // defragmentation_fichier_chainee(...);
                            break;
                        case 6:
                            printf("Adding a student to an unsorted linked file...\n");
                            // ajouter_etudiant_fichier_chainee_non_triee(...);
                            break;
                        case 7:
                            printf("Inserting into a sorted linked file...\n");
                            // insertion_dans_un_fichier_Triee(...);
                            break;
                        case 8: {
                            printf("Searching in a sorted linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            int id;
                            char nom[20];
                            int *P[2];
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID to search: ");
                            scanf("%d", &id);

                            Search_In_Sorted_Linked_File(id, ms, nom, P);

                            fclose(ms);
                            break;
                        }
                        case 9:
                            printf("Searching in an unsorted linked file...\n");
                            // recherche_fichier_chainee_non_triee(...);
                            break;
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
                    if (contiguous_action == 10) break; // Return to main menu
                    switch (contiguous_action) {
                        case 1:
                            printf("Creating a contiguous file...\n");
                            // creer_un_fichier_co(...);
                            break;
                        case 2:
                            printf("Loading a contiguous file...\n");
                            // chargerFichier_co(...);
                            break;
                        case 3:
                            printf("Inserting into a contiguous file...\n");
                            // insertion_co(...);
                            break;
                        case 4:
                            printf("Deleting a contiguous file...\n");
                            // supprime_fichier_contigue(...);
                            break;
                        case 5:
                            printf("Defragmenting a contiguous file...\n");
                            // defragmentation_co(...);
                            break;
                        case 6:
                            printf("Renaming a contiguous file...\n");
                            // Renommer_co(...);
                            break;
                        case 7:
                            printf("Logically deleting a record in a contiguous file...\n");
                            // Suppression_Enregistrement_logique_co(...);
                            break;
                        case 8:
                            printf("Physically deleting a record in a contiguous file...\n");
                            // Suppression_Enregistrement_physic_co(...);
                            break;
                        case 9:
                            printf("Searching in a contiguous file...\n");
                            // Recherche_co(...);
                            break;
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

void Delete_linked_struct(FILE *ms, FILE *f, char name[20], int id, int physical) {
    BLOC_meta blocMeta;
    BLOC_ch buffer;
    FDmeta meta;
    int i, found = 0, currentAddress, metaIndex, blocMetaCount, blocMetaIndex;

    // Locate metadata based on the file name
    fseek(ms, NbBloc * sizeof(int), SEEK_SET);
    blocMetaCount = 0;
    while (!found && fread(&blocMeta, sizeof(BLOC_meta), 1, ms)) {
        for (i = 0; i < blocMeta.ne; i++) {
            if (strcmp(blocMeta.t[i].FDnom, name) == 0) {
                found = 1;
                meta = blocMeta.t[i];
                metaIndex = i;
                blocMetaIndex = blocMetaCount;
            }
        }
        blocMetaCount++;
    }

    if (!found) {
        printf("File not found.\n");
        return;
    }

    // Initialize variables
    currentAddress = meta.adresse;
    int numStudents = 0, blocCount = 0;
    found = 0;

    if (physical) {
        // Physical deletion
        Tetudiant *arr = (Tetudiant *) malloc(meta.nbEtudiant * sizeof(Tetudiant));

        while (currentAddress != -1) {
            fseek(ms, NbBloc * sizeof(int) + currentAddress * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch), SEEK_SET);
            fread(&buffer, sizeof(BLOC_ch), 1, ms);
            for (i = 0; i < buffer.ne; i++) {
                if (buffer.t[i].id != id) {
                    // Keep students who do not match the chosen student
                    arr[numStudents++] = buffer.t[i];
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
        f = fopen(name, "wb+");

        // Add students block by block to the file
        for (i = 0; i < numStudents; i += FB) {
            BLOC_ch buffer = { .ne = 0, .next = -1 };
            for (int j = 0; j < FB && i + j < numStudents; j++) {
                buffer.t[j] = arr[i + j];
                buffer.ne++;
            }
            blocCount++;
            fwrite(&buffer, sizeof(buffer), 1, f);
        }

        // Update metadata
        meta.nbEtudiant = numStudents; // Number of students
        meta.taille = blocCount;       // Number of blocks
        blocMeta.t[metaIndex] = meta;

        // Save metadata
        fseek(ms, NbBloc * sizeof(int), SEEK_SET);
        fseek(ms, blocMetaIndex * sizeof(BLOC_meta), SEEK_CUR);
        fwrite(&blocMeta, sizeof(BLOC_meta), 1, ms);
        // Load the file in memory
        load_linked_file(ms, f, name);
        printf("Physical deletion completed.\n");
        printf("Updated number of students: %d\n", numStudents);
        printf("Updated number of blocks: %d\n", blocCount);
        free(arr);
    } else {
        // Logical deletion
        while (!found && currentAddress != -1) {
            fseek(ms, NbBloc * sizeof(int) + currentAddress * sizeof(BLOC_ch) + NbBlocmeta * sizeof(BLOC_ch), SEEK_SET);
            fread(&buffer, sizeof(BLOC_ch), 1, ms);
            for (i = 0; i < buffer.ne; i++) {
                if (buffer.t[i].id == id) {
                    // Logical deletion
                    buffer.t[i].etat = 0;
                    found = 1;
                    fseek(ms, -1 * sizeof(BLOC_ch), SEEK_CUR); // Update the block in memory
                    fwrite(&buffer, sizeof(BLOC_ch), 1, ms);
                    break;
                }
            }
            currentAddress = buffer.next; // Move to the next block
        }

        // Update metadata
        if (found) {
            meta.nbEtudiant -= 1; // Number of students
            blocMeta.t[metaIndex] = meta;
            // Save metadata
            fseek(ms, NbBloc * sizeof(int), SEEK_SET);
            fseek(ms, blocMetaIndex * sizeof(BLOC_meta), SEEK_CUR);
            fwrite(&blocMeta, sizeof(BLOC_meta), 1, ms);
            printf("Logical deletion completed.\n");
            printf("Updated number of students: %d\n", meta.nbEtudiant);
        } else {
            printf("Student with ID %d not found.\n", id);
        }
    }

    // Print the file contents after deletion
    printf("File contents after deletion:\n");
    display_file_contents(f);
}

void Search_In_Sorted_Linked_File(int ID, FILE *ms, char name[20], int *P[2]) {
    BLOC_meta bloCmeta;
    FDmeta meta;
    int z = sizeof(int) * NbBloc;
    printf("Debug: Calculated z = %d\n", z);

    fseek(ms, NbBloc * sizeof(int), SEEK_SET);  // Move to start of metadata
    printf("Debug: fseek to metadata completed.\n");

    int found = 0;
    int count = 0;
    int block = 0;
    int mov = 0;

    // Search for metadata in the MS using the file name
    while (!found && count < 10) {
        fread(&bloCmeta, sizeof(BLOC_meta), 1, ms);
        printf("Debug: Reading block meta #%d\n", count + 1);
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
    Position x = {0, 0};  // Initialize position structure

    // Move to the first block in the chain
    fseek(ms, z + (NbBlocmeta) * sizeof(BLOC_meta) + meta.adresse * sizeof(BLOC_ch), SEEK_SET);
    printf("Debug: fseek to first block at address %d completed.\n", meta.adresse);

    fread(&buffer, sizeof(BLOC_ch), 1, ms);
    printf("Debug: Read first block. Buffer details - Next: %d, NE: %d\n", buffer.next, buffer.ne);

    // Read the first block and search
    mov = SortedSearch(buffer, ID);
    printf("Debug: SortedSearch returned %d\n", x.mov);

    if (x.mov != -1) {
        printf("Block address: %d, Movement in block: %d\n", 0, mov);
        P[0] = &mov;
        P[1] = &block;
        return;
    }

    // Continue searching in linked blocks
    int i = 1;
    while (i < meta.taille) {
        if (buffer.next == -1) {
            printf("Debug: Reached end of block chain.\n");
            break;  // No more blocks
        }

        printf("Debug: Moving to next block. Address: %d\n", buffer.next);
        fseek(ms, z + NbBlocmeta * sizeof(BLOC_meta) + buffer.next * sizeof(BLOC_ch), SEEK_SET);
        fread(&buffer, sizeof(BLOC_ch), 1, ms);

        printf("Debug: Read next block #%d. Buffer details - Next: %d, NE: %d\n", i, buffer.next, buffer.ne);
        mov = SortedSearch(buffer, ID);
        printf("Debug: SortedSearch in block #%d returned %d\n", i, x.mov);

        if (mov != -1) {
            block = i;
            printf("Found ID in block #%d. Movement in block: %d\n", i, x.mov);
            P[0] = &mov;
            P[1] = &block;
            return;
        }

        i++;
    }

    // If not found
    perror("ID not found in file");
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
