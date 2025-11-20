/**
 * @file sokoban.c
 * @brief Programme de jeu Sokoban implémenté en langage C
 * @author Ako Didion
 * @version 1.0
 * @date 09/11/2025
 *
 * Ce programme implémente le jeu classique Sokoban, permettant de charger,
 * jouer, recommencer et sauvegarder des parties.
 */

/* Fichiers inclus */
#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Déclaration des constantes symboliques */
#define TAILLE 12             // Taille de la grille du plateau de jeu (carrée 12x12)
#define LIGNE 100             // Nombre maximal de lignes pour le tableau de coordonnées
#define COLONNE 2             // Nombre de colonnes pour le tableau de coordonnées (x et y)
#define OUI 1                 // Constante pour l'option "oui"
#define TAILLE_NOM_FICHIER 20 // Taille maximale pour le nom du fichier

/* Déclaration des types */
typedef char t_Plateau[TAILLE][TAILLE]; // Type pour le plateau de jeu (matrice de caractères)
typedef int t_Cordoo[LIGNE][COLONNE];   // Type pour les coordonnées (tableau d'entiers)
typedef char t_tabDeplacement[1000];    // Type pour enregistrer les déplacements

/* Déclaration des constantes de jeu */
const char MUR = '#';
const char CAISSE = '$';
const char POINT = '.';
const char CAISSE_SUR_POINT = '*';
const char VIDE = ' ';
const char JOUEUR = '@';
const char JOUEUR_SUR_POINT = '+';
const char HAUT = 'z';
const char BAS = 's';
const char GAUCHE = 'q';
const char DROITE = 'd';
const char RECOMMENCER = 'r';
const char ABANDONNER = 'x';
const char ZOOM_IN = '+';
const char ZOOM_OUT = '-';
const char UNDO = 'u';

/* Déclaration des procédures et fonctions (Prototypes) */
void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, int zoom);
void afficher_entete(char fichier[], int nDeplacement);
void charger_coordo_plateau(t_Plateau plateau, t_Cordoo plateauCordoo);
void compter_point(t_Plateau plateau, int *compteurIndice);
void deplacement(t_Plateau plateauJeu, char touche, t_Cordoo plateauCordoo, int *nDeplacement);
void recomencer_partie(t_Plateau plateauJeu, char fichier[], int *nDeplacement, t_tabDeplacement tab);
bool gagner_partie(t_Plateau plateauJeu, t_Cordoo plateauCordoo, int nombrePoint);
void deplacer_joueur(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1);
void deplacer_caisse(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1, int y2, int x2);
void abbandonner_partie(t_Plateau plateauJeu, char fichier[], int nDeplacement);
void trouver_direction(t_Plateau plateauJeu, t_Cordoo positionJoueur, t_tabDeplacement tabDirection, char touche, int *nDeplacement);
void ZOOM_IN_OUT(char touche, int *zoom);
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]);
void memoriser_deplacement(t_tabDeplacement t, char touche, int indiceDeplacement, bool maj);
bool verifier_majuscule();
void revenir_deplacement(t_tabDeplacement t, t_Plateau p, t_Cordoo tCordoo);
void initialiser_plateau(t_tabDeplacement tab);

int kbhit();

/**
 * @brief Point d'entrée du programme principal du jeu Sokoban.
 * @return EXIT_SUCCESS: arrêt normal du programme
 *
 * Initialise le jeu, charge une partie, gère la boucle de jeu principale
 * et les interactions utilisateur (déplacements, abandon, recommencer).
 */
int main()
{
    // Déclarations des variables locales au début de la fonction
    int nDeplacement = 0; // Compteur de déplacements
    int nombrePoint = 1;  // Nombre de points (cibles) à atteindre
    int zoom = 1;         // Niveau de zoom initial
    t_Plateau plateauJeu; // Plateau de jeu actuel
    t_Cordoo plateauCordoo;
    t_tabDeplacement tabDeplacement;  // Tableau de coordonnées pour le joueur et les points
    char fichier[TAILLE_NOM_FICHIER]; // Nom du fichier de la partie
    char touche = '\0';
    // Touche saisie par l'utilisateur
    printf("Saisie le nom du Jeu : \n");
    scanf("%s", fichier);                              // Lecture du nom du fichier
    charger_partie(plateauJeu, fichier);               // Chargement initial de la partie
    compter_point(plateauJeu, &nombrePoint);           // Compte le nombre de cibles
    charger_coordo_plateau(plateauJeu, plateauCordoo); // Charge les coordonnées initiales
    initialiser_plateau(tabDeplacement);

    // Boucle de jeu principale: continue tant que la partie n'est PAS gagnée et que l'utilisateur n'abandonne pas
    while (gagner_partie(plateauJeu, plateauCordoo, nombrePoint) && touche != ABANDONNER)
    {
        if (kbhit()) // Vérifie si une touche a été pressée
        {
            touche = getchar(); // Lit la touche

            if (touche == ABANDONNER)
            {
                abbandonner_partie(plateauJeu, fichier, nDeplacement); // Gestion de l'abandon
            }
            else if (touche == RECOMMENCER)
            {
                recomencer_partie(plateauJeu, fichier, &nDeplacement, tabDeplacement); // Gestion du redémarrage
                charger_coordo_plateau(plateauJeu, plateauCordoo);                     // Mise à jour des coordonnées
            }
            else if (touche == ZOOM_IN || touche == ZOOM_OUT) // Recommencer la partie
            {
                ZOOM_IN_OUT(touche, &zoom); // Gestion du zoom
            }
            else if (touche == UNDO)
            {
                // Fonctionnalité d'annulation non implémentée
                revenir_deplacement(tabDeplacement, plateauJeu, plateauCordoo);
            }
            else // Déplacement
            {
                trouver_direction(plateauJeu, plateauCordoo, tabDeplacement, touche, &nDeplacement); // Tente d'appliquer le déplacement
            }
            afficher_entete(fichier, nDeplacement);
            afficher_plateau(plateauJeu, zoom);
            for (int i = 0; i < 1000; i++)
            {
                printf("%c", tabDeplacement[i]); // Initialisation du tableau de déplacements
            }
        }
    }
    // Message de fin de jeu
    if (!gagner_partie(plateauJeu, plateauCordoo, nombrePoint)) // Si la partie est gagnée (la fonction renvoie 'false')
    {
        printf("Vous avez gagnée la partie ! \n");
    }
    else // Si la boucle s'est terminée par abandon
    {
        printf("Partie abandonnée, Au revoir ! \n");
    }
    return EXIT_SUCCESS; // Retourne le succès du programme
}

void initialiser_plateau(t_tabDeplacement tab)
{
    for (int i = 0; i < 1000; i++)
    {
        tab[i] = ' '; // Initialisation du tableau de déplacements
    }
}

void revenir_deplacement(t_tabDeplacement t, t_Plateau p, t_Cordoo positionJoueur)
{
    int i = 0;
    int y1 = 0, x1 = 0, y2 = 0, x2 = 0;
    char touche;
    while (t[i + 1] != ' ')
    {
        i++;
    }
    touche = t[i];
    printf("%d", touche);
    // Détermination des offsets en fonction de la touche
    if (touche == 'H' || touche == 'h') // 'z'
    {
        y1 = 1; // Touzsche saisie par l'u
        y2 = 2;
    }
    else if (touche == 'B' || touche == 'b') // 's'
    {
        y1 = -1;
        y2 = -2;
    }
    else if (touche == 'G' || touche == 'g') // 'q'
    {
        x1 = 1;
        x2 = 2;
    }
    else if (touche == 'D' || touche == 'd') // 'd'
    {
        x1 = -1;
        x2 = -2;
    }

    if ((x1 != 0 || y1 != 0))
    {
        deplacer_joueur(p, positionJoueur[0][0], positionJoueur[0][1], y1, x1);
        if (touche >= 65 && touche <= 90)
        {
            deplacer_caisse(p, positionJoueur[0][0] - y2, positionJoueur[0][1] - x2, y1, x1, y2, x2);
        }
        positionJoueur[0][0] += y1;
        positionJoueur[0][1] += x1;
        t[i] = ' ';
    }
}

void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[])
{
    FILE *f;

    f = fopen(fic, "w");
    fwrite(t, sizeof(char), nb, f);
    fclose(f);
}

bool verifier_majuscule()
{
    return true;
}

void memoriser_deplacement(t_tabDeplacement t, char touche, int indiceDeplacement, bool maj)
{
    int indice = 0;

    while (t[indice] != ' ')
    {
        indice++;
    }

    if (touche == HAUT)
    {
        t[indice] = 'h';
    }
    else if (touche == BAS)
    {
        t[indice] = 'b';
    }
    else if (touche == GAUCHE)
    {
        t[indice] = 'g';
    }
    else if (touche == DROITE)
    {
        t[indice] = 'd';
    }

    if (maj)
    {
        t[indice] = t[indice] - 32;
    }
}

void ZOOM_IN_OUT(char touche, int *zoom)
{
    if (touche == ZOOM_IN && *zoom < 3)
    {
        (*zoom)++;
    }
    else if (touche == ZOOM_OUT && *zoom > 1)
    {
        (*zoom)--;
    }
}

/**
 * @brief Procédure qui gère l'abandon de la partie.
 * @param plateauJeu de type t_Plateau, Entrée/Sortie : le plateau de jeu actuel.
 * @param fichier de type char[], Entrée: le nom du fichier de la partie (peut être modifié pour la sauvegarde).
 * @param nDeplacement de type int, Entrée : le nombre de déplacements effectués.
 *
 * Demande à l'utilisateur s'il souhaite sauvegarder la partie avant d'abandonner.
 */
void abbandonner_partie(t_Plateau plateauJeu, char fichier[], int nDeplacement)
{
    int sauvegarde; // Choix de sauvegarde

    printf("partie abandonée \n");
    printf("Veux tu sauvegarder la partie ? (oui = 1 , non = 0)\n");
    scanf("%d", &sauvegarde); // Lecture du choix

    if (sauvegarde) // Si l'utilisateur veut sauvegarder
    {
        printf("Le nom du fichier pour la sauvegarde ? (sans .sok) \n");
        scanf("%s", fichier);                    // Lecture du nouveau nom de fichier
        strcat(fichier, ".sok");                 // Ajout de l'extension
        enregistrer_partie(plateauJeu, fichier); // Enregistrement de la partie
        printf("Partie sauvegardée. Au revoir!\n");
    }
}

/**
 * @brief Fonction pour vérifier si la partie est gagnée.
 * @param plateauJeu de type t_Plateau, Entrée : le plateau de jeu actuel.
 * @param pointCordoo de type t_Cordoo, Entrée : les coordonnées des points (cibles).
 * @param nombrePoint de type int, Entrée : le nombre total de points (cibles) à atteindre.
 * @return bool : false si toutes les caisses sont sur les points, true sinon (partie non gagnée).
 *
 * La fonction renvoie 'true' tant qu'il reste au moins un point non recouvert par une caisse (*).
 * Elle renvoie 'false' seulement quand la partie est gagnée.
 */
bool gagner_partie(t_Plateau plateauJeu, t_Cordoo pointCordoo, int nombrePoint)
{
    bool gagne = false; // Le jeu n'est pas encore gagné par défaut

    // Parcours les coordonnées de tous les points (i = 1 car l'indice 0 est pour le joueur)
    for (int i = 1; i < nombrePoint; i++)
    {
        if (plateauJeu[pointCordoo[i][0]][pointCordoo[i][1]] != CAISSE_SUR_POINT)
        {
            gagne = true; // La condition de victoire n'est pas atteinte
        }
    }
    return gagne; // Retourne l'état de la partie
}

/**
 * @brief Procédure pour demander à l'utilisateur de recommencer la partie.
 * @param plateauJeu de type t_Plateau, Entrée/Sortie : le plateau de jeu.
 * @param fichier de type char[], Entrée : le nom du fichier de la partie initiale.
 * @param nDeplacement de type int*, Entrée/Sortie : pointeur sur le nombre de déplacements.
 *
 * Si l'utilisateur choisit de recommencer, recharge la partie
 * initiale et réinitialise le compteur de déplacements.
 */
void recomencer_partie(t_Plateau plateauJeu, char fichier[], int *nDeplacement, t_tabDeplacement tab)
{
    int recommencer; // Choix de recommencer

    printf("Veux tu recommencer la partie ? (oui = 1 , non = 0)\n");
    scanf("%d", &recommencer);

    if (recommencer) // Si l'utilisateur veut recommencer
    {
        charger_partie(plateauJeu, fichier); // Recharge la partie
        initialiser_plateau(tab);
        *nDeplacement = 0; // Réinitialise le compteur
    }
}

/**
 * @brief Procédure pour déterminer la direction de déplacement et appeler les fonctions de mouvement.
 * @param plateauJeu de type t_Plateau, Entrée/Sortie : le plateau de jeu.
 * @param positionJoueur de type t_Cordoo, Entrée/Sortie : les coordonnées du joueur (indice 0).
 * @param touche de type char, Entrée : la touche de direction saisie par l'utilisateur.
 * @param nDeplacement de type int*, Entrée/Sortie : pointeur sur le nombre de déplacements.
 *
 * Calcule les coordonnées de destination, vérifie les règles de collision (mur, caisse) et met à jour le plateau.
 */
void trouver_direction(t_Plateau plateauJeu, t_Cordoo positionJoueur, t_tabDeplacement tabDirection, char touche, int *nDeplacement)
{
    // Coordonnées actuelles du joueur
    int sokoX = positionJoueur[0][1];
    int sokoY = positionJoueur[0][0];

    bool maj = false;

    // Offsets de déplacement: (y1, x1) pour la case devant le joueur, (y2, x2) pour la case devant la caisse
    int y1 = 0, x1 = 0, y2 = 0, x2 = 0;

    // Détermination des offsets en fonction de la touche
    if (touche == HAUT) // 'z'
    {
        y1 = -1; // Touche saisie par l'u
        y2 = -2;
    }
    else if (touche == BAS) // 's'
    {
        y1 = 1;
        y2 = 2;
    }
    else if (touche == GAUCHE) // 'q'
    {
        x1 = -1;
        x2 = -2;
    }
    else if (touche == DROITE) // 'd'
    {
        x1 = 1;
        x2 = 2;
    }

    // Vérifie si la case de destination n'est pas un MUR et qu'une direction valide a été donnée
    if (plateauJeu[sokoY + y1][sokoX + x1] != MUR && (x1 + y1) != 0)
    {
        // CAS 1: Déplacement simple (VIDE ou POINT)
        if (plateauJeu[sokoY + y1][sokoX + x1] != CAISSE &&
            plateauJeu[sokoY + y1][sokoX + x1] != CAISSE_SUR_POINT)
        {
            deplacer_joueur(plateauJeu, sokoY, sokoX, y1, x1); // Déplacement du joueur
            positionJoueur[0][0] = sokoY + y1;                 // Mise à jour de la coordonnée Y
            positionJoueur[0][1] = sokoX + x1;                 // Mise à jour de la coordonnée X
            (*nDeplacement)++;                                 // Incrémentation du compteur
        }
        // CAS 2: Pousser une caisse (la case devant la caisse doit être VIDE ou POINT)
        else if (plateauJeu[sokoY + y2][sokoX + x2] == VIDE ||
                 plateauJeu[sokoY + y2][sokoX + x2] == POINT)
        {
            deplacer_caisse(plateauJeu, sokoY, sokoX, y1, x1, y2, x2);
            deplacer_joueur(plateauJeu, sokoY, sokoX, y1, x1); // Déplacement de la caisse et du joueur
            positionJoueur[0][0] = sokoY + y1;
            positionJoueur[0][1] = sokoX + x1;
            maj = verifier_majuscule();
            (*nDeplacement)++;
        }
        if ((positionJoueur[0][1] + positionJoueur[0][0]) != sokoX + sokoY)
        {
            memoriser_deplacement(tabDirection, touche, *nDeplacement, maj);
        }
    }
}

/**
 * @brief Procédure pour déplacer à la fois le joueur et la caisse.
 * @param plateauJeu de type t_Plateau, Entrée/Sortie : le plateau de jeu.
 * @param sokoY de type int, Entrée : coordonnée Y actuelle du joueur.
 * @param sokoX de type int, Entrée : coordonnée X actuelle du joueur.
 * @param y1 de type int, Entrée : offset Y pour la case devant le joueur.
 * @param x1 de type int, Entrée : offset X pour la case devant le joueur.
 * @param y2 de type int, Entrée : offset Y pour la case devant la caisse.
 * @param x2 de type int, Entrée : offset X pour la case devant la caisse.
 *
 * Met à jour les caractères sur le plateau pour simuler le déplacement de la caisse et la préparation du joueur.
 */
void deplacer_caisse(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1, int y2, int x2)
{

    int caseDevantCaisse = plateauJeu[sokoY + y2][sokoX + x2]; // Caractère de la case devant la caisse
    int caseDevantJoueur = plateauJeu[sokoY + y1][sokoX + x1]; // Caractère de la caisse

    // Mise à jour de la NOUVELLE position de la caisse
    if (caseDevantCaisse == VIDE)
    {
        plateauJeu[sokoY + y2][sokoX + x2] = CAISSE;
    }
    else if (caseDevantCaisse == POINT)
    {
        plateauJeu[sokoY + y2][sokoX + x2] = CAISSE_SUR_POINT;
    }

    // Mise à jour de l'ANCIENNE position de la caisse (qui devient la destination du joueur)
    if (caseDevantJoueur == CAISSE_SUR_POINT)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = POINT; // Le '*' redevient un '.'
    }
    else // (caseDevantJoueur == CAISSE)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = VIDE; // Le '$' redevient un ' '
    }
    // Déplace le joueur sur l'ancienne position de la caisse
}

/**
 * @brief Procédure pour déplacer le joueur sur une case vide ou un point.
 * @param plateauJeu de type t_Plateau, Entrée/Sortie : le plateau de jeu.
 * @param sokoY de type int, Entrée : coordonnée Y actuelle du joueur.
 * @param sokoX de type int, Entrée : coordonnée X actuelle du joueur.
 * @param y1 de type int, Entrée : offset Y pour la case de destination.
 * @param x1 de type int, Entrée : offset X pour la case de destination.
 *
 * Gère la mise à jour des symboles JOUEUR, JOUEUR_SUR_POINT, VIDE, et POINT sur le plateau.
 */
void deplacer_joueur(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1)
{

    int caseDevantJoueur = plateauJeu[sokoY + y1][sokoX + x1]; // Caractère de la case de destination
    int caseJoueur = plateauJeu[sokoY][sokoX];                 // Caractère de la case actuelle du joueur

    // CAS 1: Déplacement sur une case VIDE
    if (caseDevantJoueur == VIDE)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = JOUEUR; // Le joueur va sur une case VIDE, devient '@'
    }
    // CAS 2: Déplacement sur une case POINT
    else if (caseDevantJoueur == POINT)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = JOUEUR_SUR_POINT; // Le joueur va sur un POINT, devient '+'
    }

    if (caseJoueur == JOUEUR_SUR_POINT)
    {
        plateauJeu[sokoY][sokoX] = POINT; // L'ancienne position '+' redevient un '.'
    }

    else if (caseJoueur == JOUEUR)
    {
        plateauJeu[sokoY][sokoX] = VIDE; // L'ancienne position '@' redevient un ' '
    }
}

/**
 * @brief Procédure pour compter le nombre de points (cibles) sur le plateau.
 * @param plateau de type t_Plateau, Entrée : le plateau de jeu.
 * @param compteurIndice de type int*, Entrée/Sortie : pointeur sur le compteur de points.
 *
 * Parcours le plateau pour trouver les symboles POINT et CAISSE_SUR_POINT et incrémente le compteur.
 */
void compter_point(t_Plateau plateau, int *compteurIndice)
{
    // Boucles pour parcourir la matrice TAILLE x TAILLE
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            // Un point ou une caisse sur un point sont considérés comme des cibles
            if (plateau[i][j] == POINT || plateau[i][j] == CAISSE_SUR_POINT)
            {
                (*compteurIndice)++; // Incrémentation du compteur de cibles
            }
        }
    }
    printf("%d", *compteurIndice); // Affichage du nombre de points pour information
}

/**
 * @brief Procédure pour charger les coordonnées initiales du joueur et des points.
 * @param plateau de type t_Plateau, Entrée : le plateau de jeu.
 * @param plateauCordoo de type t_Cordoo, Entrée/Sortie : tableau des coordonnées à remplir.
 *
 * Stocke la position du joueur à l'indice 0 et les positions des points à partir de l'indice 1.
 */
void charger_coordo_plateau(t_Plateau plateau, t_Cordoo plateauCordoo)
{
    int indicePoint = 1;  // Indice de départ pour les coordonnées des points
    int indiceJoueur = 0; // Indice pour les coordonnées du joueur

    // Parcours de la matrice TAILLE x TAILLE
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            // Enregistrement des coordonnées des POINTS
            if (plateau[i][j] == POINT)
            {
                plateauCordoo[indicePoint][0] = i; // Ligne (Y)
                plateauCordoo[indicePoint][1] = j; // Colonne (X)
                indicePoint++;
            }
            // Enregistrement des coordonnées des CAISSE_SUR_POINT
            else if (plateau[i][j] == CAISSE_SUR_POINT)
            {
                plateauCordoo[indicePoint][0] = i;
                plateauCordoo[indicePoint][1] = j;
                indicePoint++;
            }
            // Enregistrement des coordonnées du JOUEUR
            else if (plateau[i][j] == JOUEUR || plateau[i][j] == JOUEUR_SUR_POINT)
            {
                plateauCordoo[indiceJoueur][0] = i;
                plateauCordoo[indiceJoueur][1] = j;
            }
        }
    }
}

/**
 * @brief Procédure pour afficher le plateau de jeu.
 * @param plateau_jeu de type t_Plateau, Entrée : le plateau de jeu à afficher.
 *
 * Affiche le plateau, en remplaçant les symboles ('*' et '+')
 * par les symboles visibles ('$' et '@') pour l'utilisateur.
 */
void afficher_plateau(t_Plateau plateau_jeu, int zoom)
{ // Facteur de zoom (1 = normal, 2 = double taille)
    for (int i = 0; i < TAILLE; i++)
    {
        for (int ligne = 0; ligne < zoom; ligne++) // double la hauteur
        {
            printf("\t\t"); // indentation

            for (int j = 0; j < TAILLE; j++)
                if (plateau_jeu[i][j] == JOUEUR_SUR_POINT)
                {
                    for (int k = 0; k < zoom; k++)
                    {
                        printf("%c", JOUEUR);
                    }
                }
                else if (plateau_jeu[i][j] == CAISSE_SUR_POINT)
                {
                    for (int k = 0; k < zoom; k++)
                    {
                        printf("%c", CAISSE);
                    }
                }
                else
                {
                    for (int k = 0; k < zoom; k++)
                    {
                        printf("%c", plateau_jeu[i][j]);
                    }
                }
            printf("\n");
        }
    }
}
/**
 * @brief Procédure pour afficher l'en-tête de la partie et les commandes.
 * @param fichier de type char[], Entrée : le nom du fichier de la partie.
 * @param nDeplacement de type int, Entrée : le nombre de déplacements effectués.
 *
 * Nettoie l'écran et affiche les informations de la partie et les commandes de jeu.
 */
void afficher_entete(char fichier[], int nDeplacement)
{
    system("clear"); // Nettoyage de l'écran

    printf("\tNom de la partie %s\n", fichier);
    printf("\n");
    // Affichage des commandes
    printf("\t z pour avancer\n");
    printf("\t q pour aller a gauche\n");
    printf("\t s pour reculer\n");
    printf("\t d pour aller a droite\n");
    printf("\t x pour abandonner\n");
    printf("\t r pour recommencer\n");
    printf("\n");
    // Affichage du compteur de déplacements
    printf("\tNombre de déplacements \n\t    effectuées : %d", nDeplacement);
    printf("\n");
    printf("\n");
}

/**
 * @brief Procédure pour charger le plateau de jeu à partir d'un fichier.
 * @param plateau de type t_Plateau, Entrée/Sortie : le plateau de jeu à remplir.
 * @param fichier de type char[], Entrée : le nom du fichier à charger (doit exister).
 *
 * Ouvre le fichier en mode lecture et lit son contenu caractère par caractère pour remplir la matrice du plateau.
 */
void charger_partie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne;
    f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    }
    else
    {
        for (int ligne = 0; ligne < TAILLE; ligne++)
        {
            for (int colonne = 0; colonne < TAILLE; colonne++)
            {
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

/**
 * @brief Procédure pour enregistrer l'état actuel du plateau de jeu dans un fichier.
 * @param plateau de type t_Plateau, Entrée : le plateau de jeu à sauvegarder.
 * @param fichier de type char[], Entrée : le nom du fichier de destination.
 *
 * Ouvre le fichier en mode écriture, écrit chaque caractère du plateau suivi d'un saut de ligne pour chaque ligne.
 */
void enregistrer_partie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne = '\n';

    f = fopen(fichier, "w");
    for (int ligne = 0; ligne < TAILLE; ligne++)
    {
        for (int colonne = 0; colonne < TAILLE; colonne++)
        {
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}

/**
 * @brief Fonction pour vérifier si une touche a été pressée (mode non-bloquant).
 * @return int : 1 si un caractère est présent dans le buffer d'entrée, 0 sinon.
 *
 * Configure le terminal en mode non-bloquant pour lire l'entrée utilisateur
 * sans attendre la pression de la touche Entrée.
 */
int kbhit()
{
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}
