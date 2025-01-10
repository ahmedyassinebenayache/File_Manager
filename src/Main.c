// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Main prototypes functions
void display_main_menu() {
    printf("Choose an option:\n");
    printf("1. Linked Storage\n");
    printf("2. Contiguous Storage\n");
    printf("3. < Exit >\n");
}

void display_linked_actions() {
    printf("Choose an action for linked storage:\n");
    printf("1. Initialize linked storage\n");
    printf("2. Empty linked storage\n");
    printf("3. Create linked file (Sorted & Unsorted)\n");
    printf("4. Load linked file\n");
    printf("5. Display linked file\n");
    printf("6. Rename linked file\n");
    printf("7. Delete linked file\n");
    printf("8. Add to linked list (Sorted & Unsorted)\n");
    printf("9. Defragment a linked file\n");
    printf("10. Search in linked file (Sorted & Unsorted)\n");
    printf("11. Delete record from linked file (Physical & Logical)\n");
    printf("12. < Return >\n");
}

void display_contiguous_actions() {
    printf("Choose an action for contiguous storage:\n");
    printf("1. Initialize contiguous storage\n");
    printf("2. Empty contiguous storage\n");
    printf("3. Create contiguous file (Sorted & Unsorted)\n");
    printf("4. Compact a contiguous file\n");
    printf("5. Load contiguous file\n");
    printf("6. Display contiguous file\n");
    printf("7. Rename contiguous file\n");
    printf("8. Delete contiguous file\n");
    printf("9. Add to contiguous list (Sorted & Unsorted)\n");
    printf("10. Defragment a contiguous file\n");
    printf("11. Search in contiguous file (Sorted & Unsorted)\n");
    printf("12. Delete record from contiguous file (Physical & Logical)\n");
    printf("13. < Return >\n");
}

// Main function
int main() {
    int choice;
    int exit_program = 0;

    while (!exit_program) {
        display_main_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {  // Linked Storage
                int linked_action;
                while (1) {
                    display_linked_actions();
                    scanf("%d", &linked_action);
                    if (linked_action == 12) break; // < Return >
                    switch (linked_action) {
                        case 1: {
                            FILE *ms = fopen("linked_storage.dat", "wb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }
                            printf("Initializing disk for linked storage...\n");
                            Initialize_Disk_Ch(ms);
                            printf("Disk initialized for linked storage.\n");
                            fclose(ms);
                            break;
                        }
                        case 2: {
                            FILE *ms = fopen("linked_storage.dat", "wb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }
                            printf("Emptying linked storage...\n");
                            empty_MS_Ch(ms);
                            printf("Linked storage emptied.\n");
                            fclose(ms);
                            break;
                        }
                        case 3: {
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening linked storage file.\n");
                                break;
                            }

                            FILE *f = fopen("students.dat", "wb+");
                            if (f == NULL) {
                                printf("Error opening students file.\n");
                                fclose(ms);
                                break;
                            }

                            int nbEtudiant;
                            char nom[20];
                            int sorted;

                            printf("Enter the number of students: ");
                            if (scanf("%d", &nbEtudiant) != 1) {
                                printf("Invalid input.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }
                            getchar(); // Clear the buffer

                            printf("Enter the file name: ");
                            if (fgets(nom, sizeof(nom), stdin) == NULL) {
                                printf("Error reading file name.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }
                            nom[strcspn(nom, "\n")] = '\0'; // Remove the newline

                            printf("Choose file type (0 for unsorted, 1 for sorted): ");
                            if (scanf("%d", &sorted) != 1 || (sorted != 0 && sorted != 1)) {
                                printf("Invalid input.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }

                            Creer_du_fichierchainee(ms, f, nbEtudiant, nom, sorted);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 4: {
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

                            chargement_fichier_chainee(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 5: {
                            printf("Displaying a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to display: ");
                            scanf("%s", nom);

                            Displayfile(ms, nom);

                            fclose(ms);
                            break;
                        }
                        case 6: {
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
                        case 7: {
                            printf("Deleting a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to delete: ");
                            scanf("%s", nom);

                            supprime_fichier_chainee(ms, nom);

                            fclose(ms);
                            break;
                        }
                        case 8: {
                            printf("Inserting into a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            int sorted;
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Do you want to insert in sorted or unsorted order? (0 for unsorted, 1 for sorted): ");
                            scanf("%d", &sorted);

                            // Student Information
                            Tetudiant student;
                            student.etat = 1;
                            // Prompt user for new student information
                            printf("Enter the new student's information:\n");
                            printf("ID: ");
                            scanf("%d", &student.id);
                            printf("Name: ");
                            scanf("%s", student.nom);
                            printf("Surname: ");
                            scanf("%s", student.prenom);
                            printf("Section: ");
                            scanf(" %c", &student.sec);

                            if (sorted) {
                                add_student_to_sorted_linked_file(ms, nom, student);
                            } else {
                                add_student_to_unsorted_linked_file(ms, nom, student);
                            }

                            fclose(ms);
                            break;
                        }
                        case 9: {
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

                            defragmentation_fichier_chainee(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 10: {
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
                        case 11: {
                            printf("Deleting a record from a linked file...\n");
                            FILE *ms = fopen("linked_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            int id, delete_type;
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID to delete: ");
                            scanf("%d", &id);

                            printf("Choose deletion type (0 for logical, 1 for physical): ");
                            scanf("%d", &delete_type);

                            if (delete_type == 0) {
                                logical_deletion_from_linked_file(ms, f, nom, id);
                            } else if (delete_type == 1) {
                                physical_deletion_from_linked_file(ms, f, nom, id);
                            } else {
                                printf("Invalid deletion type!\n");
                            }

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        default:
                            printf("Invalid action!\n");
                    }
                }
                break;
            }
            case 2: {  // Contiguous Storage
                int contiguous_action;
                while (1) {
                    display_contiguous_actions();
                    scanf("%d", &contiguous_action);
                    if (contiguous_action == 13) break; // < Return >
                    switch (contiguous_action) {
                        case 1: {
                            FILE *ms = fopen("contiguous_storage.dat", "wb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }
                            printf("Initializing disk for contiguous storage...\n");
                            Initialize_Disk_Co(ms);
                            printf("Disk initialized for contiguous storage.\n");
                            fclose(ms);
                            break;
                        }
                        case 2: {
                            FILE *ms = fopen("contiguous_storage.dat", "wb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }
                            printf("Emptying contiguous storage...\n");
                            empty_MS_Ch(ms); // Assuming same function can be used for contiguous storage
                            printf("Contiguous storage emptied.\n");
                            fclose(ms);
                            break;
                        }
                        case 3: {
                            printf("Creating a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening contiguous storage file.\n");
                                break;
                            }

                            FILE *f = fopen("students.dat", "wb+");
                            if (f == NULL) {
                                printf("Error opening students file.\n");
                                fclose(ms);
                                break;
                            }

                            int nbEtudiant;
                            char nom[20];
                            int sorted;

                            printf("Enter the number of students: ");
                            if (scanf("%d", &nbEtudiant) != 1) {
                                printf("Invalid input.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }
                            getchar(); // Clear the buffer

                            printf("Enter the file name: ");
                            if (fgets(nom, sizeof(nom), stdin) == NULL) {
                                printf("Error reading file name.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }
                            nom[strcspn(nom, "\n")] = '\0'; // Remove the newline

                            printf("Choose file type (0 for unsorted, 1 for sorted): ");
                            if (scanf("%d", &sorted) != 1 || (sorted != 0 && sorted != 1)) {
                                printf("Invalid input.\n");
                                fclose(ms);
                                fclose(f);
                                break;
                            }

                            creer_un_fichier_co(ms, f, nom, nbEtudiant, sorted);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 4: {
                            printf("Compacting a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                return 1;
                            }

                            compactdisk_co(ms);

                            fclose(ms);
                            break;
                        }
                        case 5: {
                            printf("Loading a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to load: ");
                            scanf("%s", nom);

                            chargerFichier_co(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 6: {
                            printf("Displaying a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to display: ");
                            scanf("%s", nom);

                            Display_fichier_co(ms, nom);

                            fclose(ms);
                            break;
                        }
                        case 7: {
                            printf("Renaming a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
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
                        case 8: {
                            printf("Deleting a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name to delete: ");
                            scanf("%s", nom);

                            supprime_fichier_contigue(ms, nom);

                            fclose(ms);
                            break;
                        }
                        case 9: {
                            printf("Inserting into a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            // Assuming insertion_co is a function that handles the insertion process
                            insertion_co(ms, nom);

                            fclose(ms);
                            break;
                        }
                        case 10: {
                            printf("Defragmenting a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                if (ms != NULL) fclose(ms);
                                if (f != NULL) fclose(f);
                                break;
                            }

                            char nom[20];
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            defragmentation_co(ms, f, nom);

                            fclose(ms);
                            fclose(f);
                            break;
                        }
                        case 11: {
                            printf("Searching in a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            if (ms == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            int id;
                            char nom[20];
                            Position Pos;

                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID to search: ");
                            scanf("%d", &id);

                            Recherche_co(ms, id, nom, &Pos.nbrbloc, &Pos.mov);

                            fclose(ms);
                            break;
                        }
                        case 12: {
                            printf("Deleting a record from a contiguous file...\n");
                            FILE *ms = fopen("contiguous_storage.dat", "rb+");
                            FILE *f = fopen("students.dat", "rb+");
                            if (ms == NULL || f == NULL) {
                                printf("Error opening file.\n");
                                break;
                            }

                            char nom[20];
                            int id, delete_type;
                            printf("Enter the file name: ");
                            scanf("%s", nom);

                            printf("Enter the student ID to delete: ");
                            scanf("%d", &id);

                            printf("Choose deletion type (0 for logical, 1 for physical): ");
                            scanf("%d", &delete_type);

                            if (delete_type == 0) {
                                Suppression_Enregistrement_logique_co(ms, id, nom);
                            } else if (delete_type == 1) {
                                Suppression_Enregistrement_physic_co(ms, id, nom);
                            } else {
                                printf("Invalid deletion type!\n");
                            }

                            fclose(ms);
                            fclose(f);
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
