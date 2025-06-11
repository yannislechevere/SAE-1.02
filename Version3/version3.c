/**
 * @file version3.c
 * @brief Automatisation d'un jeu snake codé en C lors de la SAE 1.02
 * @author Chauvel Arthur, Le Chevère Yannis
 * @version 3.0
 * @date 14/12/2024
 *
 * Cette version initialise le plateau de jeu avec les dimensions données en constantes,
 * elle initialise le serpent à la position donnée en constante,
 * elle permet également de manger des pommes dont les positions sont fixes.
 * La touche d'arrêt du jeu est fonctionnelle et la condition de victoire aussi.
 * Les conditions avec les bordures sont activées
 * et les collisions entre la tête et le corps du serpent sont activées aussi.
 * Des issues sont créées dans le plateau au milieu de chaque bordure,
 * si le serpent emprunte une de ces issues, il se téléporte à celle opposée.
 * Des pavés sont placés à des coordonnées bien précises, leur taille est donnée en constantes,
 * En cas de victoire, le nombre de déplacements unitaires réalisé par le serpent
 * et le temps CPU réalisé par le programme sont affichés.
 */


/* Fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>


// taille du serpent
#define TAILLE 10
// dimensions du plateau
#define LARGEUR_PLATEAU 80	
#define HAUTEUR_PLATEAU 40
// position initiale de la tête du serpent
#define X_INITIAL 40
#define Y_INITIAL 20
// nombre de pommes à manger pour gagner
#define NB_POMMES 10
// caractéristiques constantes des pavés (nombre et taille)
#define NB_PAVES 6
#define TAILLE_PAVES 5
// temporisation entre deux déplacements du serpent, 200 000 microseconces
#define TEMPORISATION 200000
// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE 'O'
// touches de direction ou d'arrêt du jeu
#define HAUT 1
#define BAS 2
#define GAUCHE 3
#define DROITE 4
#define CHEMIN_POMME 5
#define STOP 'a'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define VIDE ' '
#define POMME '6'
// coordonnées X et Y des issues
#define ISSUE_HAUT_X 40 // 80/2 colonne du milieu
#define ISSUE_HAUT_Y 0 // 1ère ligne 
#define ISSUE_BAS_X 40 // 80/2 colonne du milieu
#define ISSUE_BAS_Y 40 // derinère ligne 
#define ISSUE_GAUCHE_X 0 // 1ère colonne
#define ISSUE_GAUCHE_Y 20 // 40/2 ligne du milieu
#define ISSUE_DROITE_X 80 // dernière colonne
#define ISSUE_DROITE_Y 20 // 40/2 ligne du milieu

// définition des positions X et Y des pommes dans un tableau
// et des positions des coins supérieurs gauches des pavés dans un tableau
int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5}; // positions X des pommes
int lesPommesY[NB_POMMES] = { 8, 39, 2, 2, 5, 39, 33, 38, 35, 2}; // positions Y des pommes
int lesPavesX[NB_PAVES] = { 3, 74, 3, 74, 38, 38}; // position X du coin supérieur gauche du pavé 
int lesPavesY[NB_PAVES] = {3, 3, 34, 34, 21, 15 }; // position Y du coin supérieur droit du pavé

// définition d'un type pour le plateau
// Attention, pour que les indices du tableau 2D (qui commencent à 0) coincident
// avec les coordonées à l'écran (qui commencent à 1), on ajoute 1 aux dimensions
// et on neutralise la ligne 0 et la colonne 0 du tableau 2D (elles ne sont jamais
// utilisées)
typedef char tPlateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];

int nbPommes = 0; // compteur de pommes mangées

/* Déclaration des fonctions et procédures*/
void initPlateau(tPlateau plateau, int lesX[], int lesY[]);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau, int iPomme);
void placerPaves(tPlateau plateau);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void directionSerpent(int lesX[], int lesY[], tPlateau plateau, char *direction, int objectifX, int objectifY);
bool verifierCollisionProchainDeplacement(int lesX[], int lesY[], tPlateau plateau, char prochaineDirection);
int calculDistance(int lesX[], int lesY[], int pommeX, int pommeY);
void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *utiliserIssue);
void gotoxy(int x, int y);
int kbhit();
void disable_echo();
void enable_echo();


int main()
{
	// départ du calcul du temps CPU
	clock_t debut = clock();

	// Total des déplacements
	int deplacements = 0;

	// 2 tableaux contenant les positions des éléments qui constituent le serpent
	int lesX[TAILLE];
	int lesY[TAILLE];

	// représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
	char touche;

	// direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
	char direction;

	// le plateau de jeu
	tPlateau lePlateau;

	// boolléens vérifiant qu'il n'y ai pas de collisions,
	// le jeu ne s'arrête pas,
	// aucune pomme n'est mangée
	// et les issues n'ont pas à être untilisées
	bool collision = false;
	bool gagne = false;
	bool pommeMangee = false;
	bool utiliserIssue = false;

	// initialisation de la position du serpent : positionnement de la
	// tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche
	for (int i = 0 ; i < TAILLE ; i++)
	{
		lesX[i] = X_INITIAL - i;
		lesY[i] = Y_INITIAL;
	}

	// mise en place du plateau
	initPlateau(lePlateau, lesX, lesY);
	system("clear");
	dessinerPlateau(lePlateau);
	srand(time(NULL));
	ajouterPomme(lePlateau, nbPommes);

	// initialisation : le serpent se dirige vers la DROITE
	dessinerSerpent(lesX, lesY);
	disable_echo();
	direction = DROITE;

	// calcul la meilleur distance à l'initialisation
	int meilleurDistance = calculDistance(lesX, lesY, lesPommesX[nbPommes], lesPommesY[nbPommes]);

	// boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
	// si toutes les pommes sont mangées
	do
	{
		// choisis la direction en fonction de la meilleur distance
		if (meilleurDistance == HAUT) // se dirige vers le trou du haut puis quand il s'est téléporter avance vers la pomme
		{
			if (utiliserIssue)
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, lesPommesX[nbPommes], lesPommesY[nbPommes]);
			}
			else
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, ISSUE_HAUT_X, ISSUE_HAUT_Y);
			}
		}
		else if (meilleurDistance == BAS) // se dirige vers le trou du bas puis quand il s'est téléporter avance vers la pomme
		{
			if (utiliserIssue)
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, lesPommesX[nbPommes], lesPommesY[nbPommes]);
			}
			else
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, ISSUE_BAS_X, ISSUE_BAS_Y);
			}
		}
		else if (meilleurDistance == GAUCHE) // se dirige vers le trou de gauche puis quand il s'est téléporter avance vers la pomme
		{
			if (utiliserIssue)
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, lesPommesX[nbPommes], lesPommesY[nbPommes]);
			}
			else
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, ISSUE_GAUCHE_X, ISSUE_GAUCHE_Y);
			}
		}
		else if (meilleurDistance == DROITE) // se dirige vers le trou de droite puis quand il s'est téléporter avance vers la pomme
		{
			if (utiliserIssue)
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, lesPommesX[nbPommes], lesPommesY[nbPommes]);
			}
			else
			{
				directionSerpent(lesX, lesY, lePlateau, &direction, ISSUE_DROITE_X, ISSUE_DROITE_Y);
			}
		}
		else // sinon se dirige uniquement vers la pomme
		{
			directionSerpent(lesX, lesY, lePlateau, &direction, lesPommesX[nbPommes], lesPommesY[nbPommes]);
		}

		// deplacement du serpent à chaque fois et incrémentation du compteur de déplacements
		progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee, &utiliserIssue);
		deplacements++;

		// Ajoute une pomme au compteur de pomme quand elle est mangée et arrete le jeu si score atteint 10
		if (pommeMangee)
		{
			nbPommes++;
			if (nbPommes == NB_POMMES) {
				gagne = true;
			}
			if (!gagne)
			{
				ajouterPomme(lePlateau, nbPommes);
				// recalcul la meilleure position après l'apparition d'une nouvelle pomme
				meilleurDistance = calculDistance(lesX, lesY, lesPommesX[nbPommes], lesPommesY[nbPommes]); 
				pommeMangee = false;
				utiliserIssue = false;
			}
		}
		if (!gagne)
		{
			if (!collision)
			{
				usleep(TEMPORISATION);
				if (kbhit() == 1)
				{
					touche = getchar();
				}
			}
		}
	} while (touche != STOP && !collision && !gagne);
	enable_echo();
	gotoxy(1, HAUTEUR_PLATEAU + 1);

	clock_t fin = clock(); // fin du calcul du temps CPU
	double tpsCPU = ((fin - debut) * 1.0) / CLOCKS_PER_SEC;

	// afficher les performances du programme
	printf("Le temps CPU est de %.3f secondes.\n", tpsCPU);
	printf("Le serpent s'est déplacé %d fois.\n", deplacements);

	return EXIT_SUCCESS;
}


/************************************************
		FONCTIONS ET PROCEDURES DU CODE 	    
*************************************************/
void initPlateau(tPlateau plateau, int lesX[], int lesY[])
{
	// initialisation du plateau avec des espaces
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		for (int j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
		{
			plateau[i][j] = VIDE;
		}
	}
	// Mise en place la bordure autour du plateau
	// première ligne
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][1] = BORDURE;
		plateau[LARGEUR_PLATEAU / 2][1] = VIDE; // trou du haut
	}
	// lignes intermédiaires
	for (int j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
	{
		plateau[1][j] = BORDURE;
		plateau[1][HAUTEUR_PLATEAU / 2] = VIDE; // trou de gauche
		plateau[LARGEUR_PLATEAU][j] = BORDURE;
		plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU / 2] = VIDE; // trou de droite
	}
	// dernière ligne
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][HAUTEUR_PLATEAU] = BORDURE;
		plateau[LARGEUR_PLATEAU / 2][HAUTEUR_PLATEAU] = VIDE; // trou du bas
	}
	//  place les pavés sur le plateau
    placerPaves(plateau);
}

void placerPaves(tPlateau plateau) {
    for (int i = 0; i < NB_PAVES; i++) {
        for (int x = lesPavesX[i]; x < lesPavesX[i] + TAILLE_PAVES; x++) {
            for (int y = lesPavesY[i]; y < lesPavesY[i] + TAILLE_PAVES; y++) {
                // Vérifier si x et y sont dans les limites du plateau
                if (x >= 1 && x <= LARGEUR_PLATEAU && y >= 1 && y <= HAUTEUR_PLATEAU) {
                    plateau[x][y] = BORDURE;
                }
            }
        }
    }
}

void dessinerPlateau(tPlateau plateau)
{
	int i, j;
	// affiche à l'écran le contenu du tableau 2D représentant le plateau
	for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
		{
			afficher(i, j, plateau[i][j]);
		}
	}
}

void ajouterPomme(tPlateau plateau, int iPomme)
{
	// génère la position de la pomme suivante en fonction du nombre de pommes mangées,
	// vérifie que ça correspond à une case vide
	// du plateau puis l'ajoute au plateau et l'affiche
	int xPomme, yPomme;
	do
	{
		xPomme = lesPommesX[iPomme];
		yPomme = lesPommesY[iPomme];
	} while (plateau[xPomme][yPomme] != VIDE);
	plateau[xPomme][yPomme] = POMME;
	afficher(xPomme, yPomme, POMME);
}

void afficher(int x, int y, char car)
{
	gotoxy(x, y);
	printf("%c", car);
	gotoxy(1, 1);
}

void effacer(int x, int y)
{
	gotoxy(x, y);
	printf("%c", VIDE);
	gotoxy(1, 1);
}

void dessinerSerpent(int lesX[], int lesY[])
{
	int i;
	// affiche les anneaux puis la tête
	for(i = 1 ; i < TAILLE ; i++)
	{
		afficher(lesX[i], lesY[i], CORPS);
	}
	afficher(lesX[0], lesY[0], TETE);
}

void directionSerpent(int lesX[], int lesY[], tPlateau plateau, char *direction, int x, int y)
{
	// Calcul des directions possibles
	int differenceX = x - lesX[0]; // Différence en X
	int differenceY = y - lesY[0]; // Différence en Y

	// Essayer de se déplacer dans la direction verticale
	if (differenceY != 0)
	{
		*direction = (differenceY > 0) ? BAS : HAUT;
		if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
		{
			// Si collision, essayer la direction horizontale
			*direction = (differenceX > 0) ? DROITE : GAUCHE;
			if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
			{
				// Si collision, essayer l'autre direction horizontale
				*direction = (differenceX > 0) ? GAUCHE : DROITE;
				if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
				{
					// Si collision, essayer l'autre direction verticale
					*direction = (differenceY > 0) ? HAUT : BAS;
				}
			}
		}
	}
	
	// Si pas de déplacement horizontal possible, essayer horizontale
	else if (differenceX != 0)
	{
		*direction = (differenceX > 0) ? DROITE : GAUCHE;
		if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
		{
			// Si collision, essayer la direction verticale
			*direction = (differenceY > 0) ? BAS : HAUT;
			if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
			{
				// Si collision, essayer l'autre direction verticale
				*direction = (differenceY > 0) ? HAUT : BAS;
				if (verifierCollisionProchainDeplacement(lesX, lesY, plateau, *direction))
				{
					// Si collision, essayer l'autre direction horizontale
					*direction = (differenceX > 0) ? GAUCHE : DROITE;
				}
			}
		}
	}
}

int calculDistance(int lesX[], int lesY[], int pommeX, int pommeY)
{
	// définition des variables
	int passageTrouGauche, passageTrouDroit, passageTrouHaut, passageTrouBas, passageDirect;

	// calcul la distance pour chaque chemin du serpent vers la pomme
	passageTrouGauche = abs(lesX[0] - ISSUE_GAUCHE_X) + abs(lesY[0] - ISSUE_GAUCHE_Y) + 
		abs(pommeX - ISSUE_DROITE_X) + abs(pommeY - ISSUE_DROITE_Y);
	passageTrouDroit = abs(lesX[0] - ISSUE_DROITE_X) + abs(lesY[0] - ISSUE_DROITE_Y) + 
		abs(pommeX - ISSUE_GAUCHE_X) + abs(pommeY - ISSUE_GAUCHE_Y);
	passageTrouHaut = abs(lesX[0] - ISSUE_HAUT_X) + abs(lesY[0] - ISSUE_HAUT_Y) + 
		abs(pommeX - ISSUE_BAS_X) + abs(pommeY - ISSUE_BAS_Y);
	passageTrouBas = abs(lesX[0] - ISSUE_BAS_X) + abs(lesY[0] - ISSUE_BAS_Y) + 
		abs(pommeX - ISSUE_HAUT_X) + abs(pommeY - ISSUE_HAUT_Y);
	passageDirect = abs(lesX[0] - pommeX) + abs(lesY[0] - pommeY);

	// compare les résultats pour obtenir le meilleur chemin
	// chemin direct vers la pomme sans passer dans un trou
	if ( (passageDirect <= passageTrouHaut) && (passageDirect <= passageTrouBas) &&
		(passageDirect <= passageTrouGauche) && (passageDirect <= passageTrouDroit) )
	{
		return CHEMIN_POMME;
	}
	// chemin vers la pomme en passant par le trou du haut
	else if ( (passageTrouHaut <= passageTrouBas) && (passageTrouHaut <= passageTrouGauche) && 
		(passageTrouHaut <= passageTrouDroit) ) 
	{
		return HAUT;
	}
	// chemin vers la pomme en passant par le trou du bas
	else if ( (passageTrouBas <= passageTrouGauche) && (passageTrouBas <= passageTrouDroit) ) 
	{
		return BAS;
	}
	// chemin vers la pomme en passant par le trou de gauche
	else if (passageTrouGauche <= passageTrouDroit) 
	{
		return GAUCHE;
	}
	else // chemin vers la pomme en passant par le trou de droite
	{
		return DROITE;
	}
}

bool verifierCollisionProchainDeplacement(int lesX[], int lesY[], tPlateau plateau, char prochaineDirection)
{
	int nouvelleX = lesX[0]; // projeter des coordonnées en X
	int nouvelleY = lesY[0]; // projeter des coordonnées en Y

	// Calcul de la nouvelle position en fonction de la direction donnée
	switch (prochaineDirection)
	{
	case HAUT	:
		nouvelleY--;
		break;
	case BAS	:
		nouvelleY++;
		break;
	case GAUCHE	:	
		nouvelleX--;
		break;
	case DROITE	:
		nouvelleX++;
		break;
	}

	// Vérification des collisions avec les bords du tableau
	if (plateau[nouvelleX][nouvelleY] == BORDURE)
	{
		return true; // Collision avec une bordure
	}

	// Vérification des collisions avec le corps du serpent
	for (int i = 0 ; i < TAILLE ; i++)
	{
		if ( (lesX[i] == nouvelleX) && (lesY[i] == nouvelleY) )
		{
			return true; // Collision avec le corps du serpent
		}
	}

	return false; // Pas de collision
}

void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *utiliserIssue)
{
	// efface le dernier élément avant d'actualiser la position de tous les
	// élémentds du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure
	effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

	for (int i = TAILLE - 1 ; i > 0 ; i--)
	{
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}
	// faire progresser la tete dans la nouvelle direction
	switch (direction)
	{
	case HAUT:
		lesY[0] = lesY[0] - 1;
		break;
	case BAS:
		lesY[0] = lesY[0] + 1;
		break;
	case DROITE:
		lesX[0] = lesX[0] + 1;
		break;
	case GAUCHE:
		lesX[0] = lesX[0] - 1;
		break;
	}

	// Faire des trous dans les bordures
	for (int i = 1 ; i < TAILLE ; i++)
	{
		if (lesX[0] <= 0)
		{
			lesX[0] = LARGEUR_PLATEAU; // faire apparaitre à gauche
			*utiliserIssue = true;		   // quand le serpent traverse le trou
		}
		else if (lesX[0] > LARGEUR_PLATEAU)
		{
			lesX[0] = 1;		// faire apparaitre à droite
			*utiliserIssue = true; // quand le serpent traverse le trou
		}
		else if (lesY[0] <= 0)
		{
			lesY[0] = HAUTEUR_PLATEAU; // faire apparaitre en haut
			*utiliserIssue = true;		   // quand le serpent traverse le trou
		}
		else if (lesY[0] > HAUTEUR_PLATEAU)
		{
			lesY[0] = 1;		// faire apparaitre en bas
			*utiliserIssue = true; // quand le serpent traverse le trou
		}
	}

	*pomme = false;
	// détection d'une "collision" avec une pomme
	if (plateau[lesX[0]][lesY[0]] == POMME)
	{
		*pomme = true;
		// la pomme disparait du plateau
		plateau[lesX[0]][lesY[0]] = VIDE;
	}
	// détection d'une collision avec la bordure
	else if (plateau[lesX[0]][lesY[0]] == BORDURE)
	{
		*collision = true;
	}
	dessinerSerpent(lesX, lesY);
}



/************************************************
				 FONCTIONS UTILITAIRES 			
*************************************************/
void gotoxy(int x, int y)
{ 
    printf("\033[%d;%df", y, x);
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
 
	if(ch != EOF)
	{
		ungetc(ch, stdin);
		unCaractere = 1;
	} 
	return unCaractere;
}

// Fonction pour désactiver l'echo
void disable_echo()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) 
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Désactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour réactiver l'echo
void enable_echo()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Réactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
