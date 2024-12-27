#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chaînée.h"
#include "contiguë.h"

// Prototypes are already declared in the header files

void afficherMenu();
void choisirMode(int *mode);

int main() {
    int choix, mode = -1;  // 'mode' will hold the system type, -1 indicates uninitialized

    // Choose between "chaînée" or "contiguë"
    choisirMode(&mode);

    do {
        afficherMenu();  // Afficher le menu à l'utilisateur
        printf("Entrez votre choix: ");
        scanf("%d", &choix);  // Lire le choix de l'utilisateur

        switch (choix) {
            case 1:
                initialiserMemoire();  // Réinitialiser la mémoire
                break;
            case 2:
                if (mode == 1) {  // Chaînée
                    creerFichier();  // Fonction pour créer et charger un fichier chaînée
                } else if (mode == 2) {  // Contiguë
                    creerFichierContigue();  // Fonction pour créer et charger un fichier contiguë
                }
                break;
            case 3:
                afficherEtatMemoire();  // Afficher l'état de la mémoire (blocs libres/occupés)
                break;
            case 4:
                afficherMetadonneesFichier();  // Afficher les métadonnées des fichiers
                break;
            case 5:
                rechercherEnregistrementParID();  // Rechercher un fichier par son ID
                break;
            case 6:
                insererEnregistrement();  // Insérer un enregistrement dans un fichier
                break;
            case 7:
                supprimerEnregistrement();  // Supprimer un enregistrement d'un fichier
                break;
            case 8:
                defragmenterFichier();  // Défragmenter un fichier
                break;
            case 9:
                renommerFichier();  // Renommer un fichier
                break;
            case 10:
                supprimerFichier();  // Supprimer un fichier
                break;
            case 11:
                compacterMemoire();  // Compacter l'espace libre en mémoire
                break;
            case 12:
                viderMemoire();  // Vider toute la mémoire
                break;
            case 0:
                printf("Fermeture du programme...\n");
                break;
            default:
                printf("Choix invalide, veuillez réessayer.\n");
        }
    } while (choix != 0);  // Continuer jusqu'à ce que l'utilisateur choisisse de quitter

    return 0;
}

// Affiche le menu principal de l'application
void afficherMenu() {
    printf("\n=== Système de Gestion de Fichiers ===\n");
    printf("1. Initialiser la Mémoire\n");
    printf("2. Créer un Fichier\n");
    printf("3. Afficher l'État de la Mémoire\n");
    printf("4. Afficher les Métadonnées des Fichiers\n");
    printf("5. Rechercher un Enregistrement par ID\n");
    printf("6. Insérer un Nouvel Enregistrement\n");
    printf("7. Supprimer un Enregistrement\n");
    printf("8. Défragmenter un Fichier\n");
    printf("9. Renommer un Fichier\n");
    printf("10. Supprimer un Fichier\n");
    printf("11. Compacter la Mémoire\n");
    printf("12. Vider la Mémoire\n");
    printf("0. Quitter\n");
}

// Fonction pour choisir entre chaînée ou contiguë
void choisirMode(int *mode) {
    printf("Choisissez le système de gestion de fichiers:\n");
    printf("1. Chaînée\n");
    printf("2. Contiguë\n");
    printf("Entrez votre choix (1 ou 2): ");
    scanf("%d", mode);

    if (*mode == 1) {
        printf("Système Chaînée sélectionné.\n");
    } else if (*mode == 2) {
        printf("Système Contiguë sélectionné.\n");
    } else {
        printf("Choix invalide. Par défaut, Chaînée sélectionné.\n");
        *mode = 1;  // Défaut sur Chaînée
    }
}
