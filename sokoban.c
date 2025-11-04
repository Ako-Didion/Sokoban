#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAILLE 12
typedef char t_Plateau[TAILLE][TAILLE];

void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, t_Plateau plateau_jeu);
void afficher_entete(char fichier[]);
void coordo_plateau(t_Plateau plateau, int tabCordoo[][2]);
void element_plateau(t_Plateau plateau, int *compteurIndice);
void deplacement(t_Plateau plateauJeu, char direction, int tabCoordo[][2]);
void reactualisation_plateau(t_Plateau plateauJeu, int tabCoordo[][2], int cpt);
void recomencer_partie(t_Plateau plateauJeu, t_Plateau plateauOriginal, int tabCoordo[][2]);
bool gagner(t_Plateau plateauJeu, int tabCoordo[][2], int nombrePoint);
int kbhit();

int nombreDeplacement = 0;

int main()
{
    t_Plateau plateauOriginal;
    t_Plateau plateauJeu;
    char fichier[11];
    int cpt = 0;
    printf("Saisie le nom du Jeu : \n");
    scanf("%s", fichier);
    charger_partie(plateauOriginal, fichier);
    charger_partie(plateauJeu, fichier);
    element_plateau(plateauOriginal, &cpt);
    int tabCordoo[cpt][2];
    coordo_plateau(plateauOriginal, tabCordoo);
    while (!gagner(plateauJeu, tabCordoo, cpt))
    {

        if (kbhit())
        {
            char touche = getchar();
            if (touche == 'x')
            {
                int sauvegarde;
                printf("partie abandonée \n");
                printf("Veux tu sauvegarder la partie dans l'etat des choses ? (oui = 0 , non = 1)\n");
                scanf("%d", &sauvegarde);
                if (sauvegarde == 0)
                {
                    printf("Le nom du fichier pour la sauvegarde ? \n");
                    scanf("%s", fichier);
                    strcat(fichier, ".sok");
                    enregistrer_partie(plateauJeu, fichier);
                    printf("Partie sauvegardée. Au revoir!\n");
                }
                return 0;
            }
            else if (touche == 'r')
            {
                recomencer_partie(plateauJeu, plateauOriginal, tabCordoo);
                afficher_entete(fichier);
                afficher_plateau(plateauOriginal, plateauJeu);
            }
            else
            {
                deplacement(plateauJeu, touche, tabCordoo);
                afficher_entete(fichier);
                reactualisation_plateau(plateauJeu, tabCordoo, cpt);
                afficher_plateau(plateauOriginal, plateauJeu);
            }
        }
    }
    printf("Vous avez gagner ! \n");
    return 0;
}

bool gagner(t_Plateau plateauJeu, int tabCoordo[][2], int nombrePoint)
{
    for (int i = 1; i < nombrePoint; i++)
    {
        if (plateauJeu[tabCoordo[i][0]][tabCoordo[i][1]] != '$')
        {
            return false;
        }
    }
    return true;
}

void recomencer_partie(t_Plateau plateauJeu, t_Plateau plateauOriginal, int tabCoordo[][2])
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            plateauJeu[i][j] = plateauOriginal[i][j];
        }
    }
    coordo_plateau(plateauJeu, tabCoordo);
    nombreDeplacement = 0;
}

void deplacement(t_Plateau plateauJeu, char direction, int tabCoordo[][2])
{
    int joueurX = tabCoordo[0][0];
    int joueurY = tabCoordo[0][1];
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (direction == 'z')
    {
        x1 = -1;
        x2 = -2;
    }
    else if (direction == 's')
    {
        x1 = 1;
        x2 = 2;
    }
    else if (direction == 'q')
    {
        y1 = -1;
        y2 = -2;
    }
    else if (direction == 'd')
    {
        y1 = 1;
        y2 = 2;
    }

    if (plateauJeu[joueurX + x1][joueurY + y1] != '#')
    {
        if (plateauJeu[joueurX + x1][joueurY + y1] == ' ')
        {
            plateauJeu[joueurX + x1][joueurY + y1] = '@';
            plateauJeu[joueurX][joueurY] = ' ';
            
        }
        else if (plateauJeu[joueurX + x1][joueurY + y1] == '$')
        {
            if (plateauJeu[joueurX + x2][joueurY + y2] == '.')
            {
                plateauJeu[joueurX + x2][joueurY + y2] = '*';
                plateauJeu[joueurX + x1][joueurY + y1] = '@';
                plateauJeu[joueurX][joueurY] = ' ';
            }
            
        }
            
    }

        tabCoordo[0][0] += x1;
        tabCoordo[0][1] += y1;
        nombreDeplacement++;
}


void reactualisation_plateau(t_Plateau plateauJeu, int tabCoordo[][2], int cpt)
{
    for (int i = 1; i < cpt; i++)
    {
        if (plateauJeu[tabCoordo[i][0]][tabCoordo[i][1]] == ' ')
        {
            plateauJeu[tabCoordo[i][0]][tabCoordo[i][1]] = '.';
        }
    }
}

void element_plateau(t_Plateau plateau, int *compteurIndice)
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau[i][j] == '.' || plateau[i][j] == '@')
            {
                (*compteurIndice)++;
            }
        }
    }
}

void coordo_plateau(t_Plateau plateau, int tabCordoo[][2])
{
    int indexPoint = 1;
    int indexJoueur = 0;
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau[i][j] == '.')
            {
                tabCordoo[indexPoint][0] = i;
                tabCordoo[indexPoint][1] = j;
                indexPoint++;
            }
            else if (plateau[i][j] == '@')
            {
                tabCordoo[indexJoueur][0] = i;
                tabCordoo[indexJoueur][1] = j;
            }
        }
    }
}

void afficher_plateau(t_Plateau plateau_ori, t_Plateau plateau_jeu)
{
    for (int i = 0; i < TAILLE; i++)
    {
        printf("\t\t");
        for (int j = 0; j < TAILLE; j++)
            {
                printf("%c", plateau_jeu[i][j]);
            }

        printf("\n");
    }
}

void afficher_entete(char fichier[])
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
    printf("\tNombre de déplacements \n\t        effectué : %d", nombreDeplacement);
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


