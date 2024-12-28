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

typedef struct Position{
    int nbrbloc;
    int mov;
} Position;

// Function Prototypes
void Initialize_Disk_Ch(FILE *ms);
void Initialize_Disk_Co(FILE *ms);
void update_Allocation_Table(FILE *ms, int bloc_adress, int b);
void update_Allocation_Table_co(FILE *ms, int bloc_adress, int b);
void empty_MS_Ch(FILE *ms);
void empty_MS_Co(FILE *ms);
int Manage_Storage_Space_Ch(FILE *ms, int num_Etudiant);
int Manage_Storage_Space_Co(FILE *ms, int num_Etudiant);
void Compact_Disk_Co(FILE *ms);

void Creer_du_fichier_triee_chainee(FILE *ms, FILE *f, char nom[20], int nbEtudiant);
void creer_un_fichier_chainee_non_triee(FILE *ms, FILE *f, char nom[20], int nbEtudiant);
void chargement_fichier_chainee(FILE *ms, FILE *f, char nom[20]);
void suppression_physique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);
void suppression_logique_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);
void renomer_fichier_chainee(FILE *ms, char nom[20], char nouveaunom[20]);
void supprime_fichier_chainee(FILE *ms, char nom[20]);
void defragmentation_fichier_chainee(FILE *ms, FILE *f, char nom[20], int id);

void creer_un_fichier_co(FILE *ms, FILE *f, char FDnom[20], int taille, int internalmode);
void chargerFichier_co(FILE *ms, FILE *f, char fileName[20]);
void insertion_co(FILE *ms, char file_name[30]);
void supprime_fichier_contigue(FILE *ms, char nom[20]);
void defragmentation_co(int adr_premierbloc, int nbr_blocs, FILE *ms);
void Renommer_co(FILE *ms, char *oldName, char *newName);

void ajouter_etudiant_fichier_chainee_non_triee(FILE *ms, FILE *f, char nom[20]);
void insertion_dans_un_fichier_Triee(FILE *ms, FDmeta m, Tetudiant x);
Position Recherche_Sur_un_fichier_chainee_triee(int ID, FILE *ms, char nom[20]);
void recherche_fichier_chainee_non_triee(FILE *ms, char nom[20], int id, int p[2], FILE *f);
void Recherche_co(FILE *MS, int id_Tetudiant, int *num_block, int *deplacement);
void Suppression_Enregistrement_logique_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]);
void Suppression_Enregistrement_physic_co(FILE *MS, int ID_SUPP_Tetudiant, char file_name[30]);

void menuSystem(FILE *ms, FILE *f) {
    int mode = -1;  // Mode for selecting Linked or Contiguous
    int choice = 0; // User's choice for the operation

    // Ask the user to choose between Linked or Contiguous
    printf("Choose system mode:\n");
    printf("1. Linked List\n");
    printf("2. Contiguous List\n");
    printf("Enter your choice: ");
    scanf("%d", &mode);

    // Start the menu loop only after the mode has been selected
    if (mode == 1 || mode == 2) {
        do {
            if (mode == 1) {  // Linked Mode Menu
                printf("\n--- Linked List Operations ---\n");
                printf("1. Initialize Disk (Linked)\n");
                printf("2. Create a Sorted Linked File\n");
                printf("3. Create an Unsorted Linked File\n");
                printf("4. Load Linked File\n");
                printf("5. Physical Deletion of Student Record\n");
                printf("6. Logical Deletion of Student Record\n");
                printf("7. Rename Linked File\n");
                printf("8. Delete Linked File\n");
                printf("9. Defragment Linked File\n");
                printf("0. Exit\n");
            } else if (mode == 2) {  // Contiguous Mode Menu
                printf("\n--- Contiguous File Operations ---\n");
                printf("1. Initialize Disk (Contiguous)\n");
                printf("2. Create a Contiguous File\n");
                printf("3. Load Contiguous File\n");
                printf("4. Insert Record into Contiguous File\n");
                printf("5. Delete Contiguous File\n");
                printf("6. Defragment Contiguous File\n");
                printf("7. Rename Contiguous File\n");
                printf("0. Exit\n");
            }

            printf("Enter your choice: ");
            scanf("%d", &choice);  // Read user input for operation

            // Only call the appropriate functions when the user selects an action
            switch (choice) {
                case 1:
                    if (mode == 1) {
                        printf("Function Initialize_Disk_Ch is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function Initialize_Disk_Co is not implemented yet.\n");
                    }
                    break;

                case 2:
                    if (mode == 1) {
                        printf("Function Creer_du_fichier_triee_chainee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function creer_un_fichier_co is not implemented yet.\n");
                    }
                    break;

                case 3:
                    if (mode == 1) {
                        printf("Function creer_un_fichier_chainee_non_triee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function chargerFichier_co is not implemented yet.\n");
                    }
                    break;

                case 4:
                    if (mode == 1) {
                        printf("Function chargement_fichier_chainee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function insertion_co is not implemented yet.\n");
                    }
                    break;

                case 5:
                    if (mode == 1) {
                        printf("Function suppression_physique_fichier_chainee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function supprime_fichier_contigue is not implemented yet.\n");
                    }
                    break;

                case 6:
                    if (mode == 1) {
                        printf("Function suppression_logique_fichier_chainee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function defragmentation_co is not implemented yet.\n");
                    }
                    break;

                case 7:
                    if (mode == 1) {
                        printf("Function renomer_fichier_chainee is not implemented yet.\n");
                    } else if (mode == 2) {
                        printf("Function Renommer_co is not implemented yet.\n");
                    }
                    break;

                case 0:
                    printf("Exiting...\n");
                    break;

                default:
                    printf("Invalid choice, please try again.\n");
                    break;
            }
        } while (choice != 0);
    } else {
        printf("Invalid mode selection, please choose either 1 or 2.\n");
    }
}

int main() {
    FILE *ms = NULL;  // Disk simulation (empty file) pointer
    FILE *f = NULL;   // File operations pointer

    // Call the menu system
    menuSystem(ms, f);
    
    return 0;
}
