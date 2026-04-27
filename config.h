#ifndef CONFIG_H
#define CONFIG_H

#define LARGEUR_FENETRE  1920
#define HAUTEUR_FENETRE  1000
#define FPS              60

#define VITESSE_JOUEUR   5
#define VITESSE_TIR      10
#define VITESSE_ENNEMI   3
#define VITESSE_BOSS     1

#define MAX_TIRS         100
#define MAX_ENNEMIS      100

#define NB_NIVEAUX       3

typedef enum {
    ETAT_MENU,
    ETAT_JEU,
    ETAT_GAME_OVER,
    ETAT_VICTOIRE,
    ETAT_QUIT
} EtatJeu;

#endif
