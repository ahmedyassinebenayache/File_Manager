#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Function Prototypes
void Initialize_Disk_Ch(FILE *ms);
void Initialize_Disk_Co(FILE *ms);
void Creer_du_fichier_triee_chainee(FILE *ms, FILE *f, char nom[20], int nbEtudiant);
void creer_un_fichier_chainee_non_triee(FILE *ms, FILE *f, char nom[20], int nbEtudiant);
void chargerFichier_co(FILE *ms, FILE *f, char fileName[20]);
void insertion_co(FILE *ms, char file_name[30]);
void supprime_fichier_contigue(FILE *ms, char nom[20]);
void menuSystem(FILE *ms, FILE *f);

// Function Definitions

void Initialize_Disk_Ch(FILE *ms) {
    // Code to initialize the disk for linked files
    printf("Disk Initialized for Linked Files.\n");
}

void Initialize_Disk_Co(FILE *ms) {
    // Code to initialize the disk for contiguous files
    printf("Disk Initialized for Contiguous Files.\n");
}

void Creer_du_fichier_triee_chainee(FILE *ms, FILE *f, char nom[20], int nbEtudiant) {
    // Code to create a sorted linked file
    printf("Sorted Linked File Created: %s\n", nom);
}

void creer_un_fichier_chainee_non_triee(FILE *ms, FILE *f, char nom[20], int nbEtudiant) {
    // Code to create an unsorted linked file
    printf("Unsorted Linked File Created: %s\n", nom);
}

void chargerFichier_co(FILE *ms, FILE *f, char fileName[20]) {
    // Code to load a contiguous file
    printf("Contiguous File Loaded: %s\n", fileName);
}

void insertion_co(FILE *ms, char file_name[30]) {
    // Code to insert a record into a contiguous file
    printf("Record Inserted into Contiguous File: %s\n", file_name);
}

void supprime_fichier_contigue(FILE *ms, char nom[20]) {
    // Code to delete a contiguous file
    printf("Contiguous File Deleted: %s\n", nom);
}

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
    do {
        if (mode == 1) {  // Linked Mode Menu
            printf("\n--- Linked List Operations ---\n");
            printf("1. Initialize Disk (Linked)\n");
            printf("2. Create a Sorted Linked File\n");
            printf("3. Create an Unsorted Linked File\n");
            printf("4. Load Linked File\n");
            printf("5. Delete Linked File\n");
            printf("0. Exit\n");
        } else if (mode == 2) {  // Contiguous Mode Menu
            printf("\n--- Contiguous File Operations ---\n");
            printf("1. Initialize Disk (Contiguous)\n");
            printf("2. Create a Contiguous File\n");
            printf("3. Load Contiguous File\n");
            printf("4. Insert Record into Contiguous File\n");
            printf("5. Delete Contiguous File\n");
            printf("0. Exit\n");
        } else {
            printf("Invalid mode selected. Exiting...\n");
            break;
        }

        printf("Enter your choice: ");
        scanf("%d", &choice);  // Read user input for operation

        // Only call the appropriate functions when the user selects an action
        switch (choice) {
            case 1:
                if (mode == 1) {
                    Initialize_Disk_Ch(ms);  // Initialize Disk for Linked
                } else if (mode == 2) {
                    Initialize_Disk_Co(ms);  // Initialize Disk for Contiguous
                }
                break;

            case 2:
                if (mode == 1) {
                    Creer_du_fichier_triee_chainee(ms, f, "example", 10);  // Create Sorted Linked File
                } else if (mode == 2) {
                    creer_un_fichier_chainee_non_triee(ms, f, "example", 10);  // Create Unsorted Linked File
                }
                break;

            case 3:
                if (mode == 1) {
                    chargerFichier_co(ms, f, "example");  // Load Linked File
                } else if (mode == 2) {
                    chargerFichier_co(ms, f, "example");  // Load Contiguous File
                }
                break;

            case 4:
                if (mode == 1) {
                    printf("Linked file operations not implemented yet.\n");
                } else if (mode == 2) {
                    insertion_co(ms, "example");  // Insert into Contiguous File
                }
                break;

            case 5:
                if (mode == 1) {
                    printf("Linked file deletion not implemented yet.\n");
                } else if (mode == 2) {
                    supprime_fichier_contigue(ms, "example");  // Delete Contiguous File
                }
                break;

            case 0:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);  // Continue until the user chooses to exit
}

int main() {
    FILE *ms = NULL;  // Simulated memory space (can be a file pointer if needed)
    FILE *f = NULL;   // Placeholder for file handling

    menuSystem(ms, f);  // Run the menu system

    return 0;
}
