#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE 12
#define LIGNE 100
#define COLONNE 2
#define OUI 1
#define TAILLE_NOM_FICHIER 20
#define INDICE1 1
#define INDICE2 2

typedef char t_Plateau[TAILLE][TAILLE];
typedef int t_Cordoo[LIGNE][COLONNE];

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

void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau);
void afficher_entete(char fichier[], int nDeplacement);
void charger_coordo_plateau(t_Plateau plateau, t_Cordoo plateauCordoo);
void compter_point(t_Plateau plateau, int *compteurIndice);
void recomencer_partie(t_Plateau plateauJeu, char fichier[], int *nDeplacement);
bool gagner_partie(t_Plateau plateauJeu, t_Cordoo plateauCordoo, int nombrePoint);
void deplacer_joueur(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1);
void deplacer_caisse(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1, int y2, int x2);
void abbandonner_partie(t_Plateau plateauJeu, char fichier[], int nDeplacement);
void trouver_direction(t_Plateau plateauJeu, t_Cordoo plateauCordoo, char touche, int *nDeplacement);
int kbhit();

int main()
{
    int nDeplacement = 0;
    int nombrePoint = 1;
    t_Plateau plateauJeu;
    t_Cordoo plateauCordoo;
    char fichier[TAILLE_NOM_FICHIER];
    char touche = '\0';
    printf("Saisie le nom du Jeu : \n");
    scanf("%s", fichier);
    charger_partie(plateauJeu, fichier);
    compter_point(plateauJeu, &nombrePoint);
    charger_coordo_plateau(plateauJeu, plateauCordoo);
    while (gagner_partie(plateauJeu, plateauCordoo, nombrePoint) && touche != ABANDONNER)
    {
        if (kbhit())
        {
            touche = getchar();
            if (touche == ABANDONNER)
            {
                abbandonner_partie(plateauJeu, fichier, nDeplacement);
            }

            else if (touche == RECOMMENCER)
            {
                recomencer_partie(plateauJeu, fichier, &nDeplacement);
                charger_coordo_plateau(plateauJeu, plateauCordoo);
                afficher_entete(fichier, nDeplacement);
                afficher_plateau(plateauJeu);
            }

            else
            {
                trouver_direction(plateauJeu, plateauCordoo, touche, &nDeplacement);
                afficher_entete(fichier, nDeplacement);
                afficher_plateau(plateauJeu);
            }
        }
    }
    if (!gagner_partie(plateauJeu, plateauCordoo, nombrePoint))
    {
        printf("Vous avez gagnée la partie ! \n");
    }
    else
    {
        printf("Partie abandonnée, Au revoir ! \n");
    }
    return EXIT_SUCCESS;
}

void abbandonner_partie(t_Plateau plateauJeu, char fichier[], int nDeplacement)
{
    int sauvegarde;
    printf("partie abandonée \n");
    printf("Veux tu sauvegarder la partie ? (oui = 1 , non = 0)\n");
    scanf("%d", &sauvegarde);
    if (sauvegarde)
    {
        printf("Le nom du fichier pour la sauvegarde ? (sans .sok) \n");
        scanf("%s", fichier);
        strcat(fichier, ".sok");
        enregistrer_partie(plateauJeu, fichier);
        printf("Partie sauvegardée. Au revoir!\n");
    }
}

bool gagner_partie(t_Plateau plateauJeu, t_Cordoo pointCordoo, int nombrePoint)
{
    bool gagne = false;
    for (int i = 1; i < nombrePoint; i++)
    {
        if (plateauJeu[pointCordoo[i][0]][pointCordoo[i][1]] != CAISSE_SUR_POINT)
        {
            gagne = true;
        }
    }
    return gagne;
}

void recomencer_partie(t_Plateau plateauJeu, char fichier[], int *nDeplacement)
{
    int recommencer;
    printf("Veux tu recommencer la partie ? (oui = 1 , non = 0)\n");
    scanf("%d", &recommencer);
    if (recommencer)
    {
        charger_partie(plateauJeu, fichier);
        *nDeplacement = 0;
    }
}

void trouver_direction(t_Plateau plateauJeu, t_Cordoo positionJoueur, char touche, int *nDeplacement)
{
    int sokoX = positionJoueur[0][1];
    int sokoY = positionJoueur[0][0];
    int y1 = 0, x1 = 0, y2 = 0, x2 = 0;
    if (touche == HAUT)
    {
        y1 = -1;
        y2 = -2;
    }
    else if (touche == BAS)
    {
        y1 = 1;
        y2 = 2;
    }
    else if (touche == GAUCHE)
    {
        x1 = -1;
        x2 = -2;
    }
    else if (touche == DROITE)
    {
        x1 = 1;
        x2 = 2;
    }

    if (plateauJeu[sokoY + y1][sokoX + x1] != MUR && (x1 + y1) != 0)
    {
        if (plateauJeu[sokoY + y1][sokoX + x1] != CAISSE &&
            plateauJeu[sokoY + y1][sokoX + x1] != CAISSE_SUR_POINT)
        {
            deplacer_joueur(plateauJeu, sokoY, sokoX, y1, x1);
            positionJoueur[0][0] = sokoY + y1;
            positionJoueur[0][1] = sokoX + x1;
            (*nDeplacement)++;
        }
        else if (plateauJeu[sokoY + y2][sokoX + x2] == VIDE ||
                 plateauJeu[sokoY + y2][sokoX + x2] == POINT)
        {
            deplacer_caisse(plateauJeu, sokoY, sokoX, y1, x1, y2, x2);
            positionJoueur[0][0] = sokoY + y1;
            positionJoueur[0][1] = sokoX + x1;
            (*nDeplacement)++;
        }
    }
}

void deplacer_caisse(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1, int y2, int x2)
{
    int caseDevantCaisse = plateauJeu[sokoY + y2][sokoX + x2];
    int caseDevantJoueur = plateauJeu[sokoY + y1][sokoX + x1];

    if (caseDevantCaisse == VIDE)
    {
        plateauJeu[sokoY + y2][sokoX + x2] = CAISSE;
    }
    else if (caseDevantCaisse == POINT)
    {
        plateauJeu[sokoY + y2][sokoX + x2] = CAISSE_SUR_POINT;
    }

    if (caseDevantJoueur == CAISSE_SUR_POINT)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = POINT;
    }
    else
    {
        plateauJeu[sokoY + y1][sokoX + x1] = VIDE;
    }
    deplacer_joueur(plateauJeu, sokoY, sokoX, y1, x1);
}

void deplacer_joueur(t_Plateau plateauJeu, int sokoY, int sokoX, int y1, int x1)
{
    int caseDevantJoueur = plateauJeu[sokoY + y1][sokoX + x1];
    int caseJoueur = plateauJeu[sokoY][sokoX];

    if (caseDevantJoueur == VIDE)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = JOUEUR;
        if (caseJoueur == JOUEUR_SUR_POINT)
        {
            plateauJeu[sokoY][sokoX] = POINT;
        }

        else
        {
            plateauJeu[sokoY][sokoX] = VIDE;
        }
    }

    else if (caseDevantJoueur == POINT)
    {
        plateauJeu[sokoY + y1][sokoX + x1] = JOUEUR_SUR_POINT;
        if (caseJoueur == JOUEUR_SUR_POINT)
        {
            plateauJeu[sokoY][sokoX] = POINT;
        }

        else if (caseJoueur == JOUEUR)
        {
            plateauJeu[sokoY][sokoX] = VIDE;
        }
    }
}

void compter_point(t_Plateau plateau, int *compteurIndice)
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau[i][j] == POINT || plateau[i][j] == CAISSE_SUR_POINT)
            {
                (*compteurIndice)++;
            }
        }
    }
    printf("%d", *compteurIndice);
}

void charger_coordo_plateau(t_Plateau plateau, t_Cordoo plateauCordoo)
{
    int indicePoint = 1;
    int indiceJoueur = 0;
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau[i][j] == POINT)
            {
                plateauCordoo[indicePoint][0] = i;
                plateauCordoo[indicePoint][1] = j;
                indicePoint++;
            }
            else if (plateau[i][j] == CAISSE_SUR_POINT)
            {
                plateauCordoo[indicePoint][0] = i;
                plateauCordoo[indicePoint][1] = j;
                indicePoint++;
            }
            else if (plateau[i][j] == JOUEUR || plateau[i][j] == JOUEUR_SUR_POINT)
            {
                plateauCordoo[indiceJoueur][0] = i;
                plateauCordoo[indiceJoueur][1] = j;
            }
        }
    }
}

void afficher_plateau(t_Plateau plateau_jeu)
{
    for (int i = 0; i < TAILLE; i++)
    {
        printf("\t\t");
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau_jeu[i][j] == JOUEUR_SUR_POINT)
            {
                printf("%c", JOUEUR);
            }
            else if (plateau_jeu[i][j] == CAISSE_SUR_POINT)
            {
                printf("%c", CAISSE);
            }
            else
            {
                printf("%c", plateau_jeu[i][j]);
            }
        }

        printf("\n");
    }
}

void afficher_entete(char fichier[], int nDeplacement)
{
    system("clear");
    printf("\tNom de la partie %s\n", fichier);
    printf("\n");
    printf("\t z pour avancer\n");
    printf("\t q pour aller a gauche\n");
    printf("\t s pour reculer\n");
    printf("\t d pour aller a droite\n");
    printf("\t x pour abandonner\n");
    printf("\t r pour recommencer\n");
    printf("\n");
    printf("\tNombre de déplacements \n\t        effectué : %d", nDeplacement);
    printf("\n");
}

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

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere présent
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
