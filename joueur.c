#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "joueur.h"
#include "config.h"

void init_joueur(Joueur *j) {
    j->w = 40;
    j->h = 20;
    j->x = 100;
    j->y = HAUTEUR_FENETRE / 2 - j->h / 2;
    j->vies = 3;
    j->vivant = 1;
}

void maj_joueur(Joueur *j, int haut, int bas, int gauche, int droite) {
    if (!j->vivant) return;

    if (haut)    j->y -= VITESSE_JOUEUR;
    if (bas)     j->y += VITESSE_JOUEUR;
    if (gauche)  j->x -= VITESSE_JOUEUR;
    if (droite)  j->x += VITESSE_JOUEUR;

    if (j->x < 0) j->x = 0;
    if (j->y < 0) j->y = 0;
    if (j->x + j->w > LARGEUR_FENETRE) j->x = LARGEUR_FENETRE - j->w;
    if (j->y + j->h > HAUTEUR_FENETRE) j->y = HAUTEUR_FENETRE - j->h;
}

void dessiner_joueur(Joueur *j) {
    if (!j->vivant) return;
    al_draw_filled_rectangle(j->x, j->y, j->x + j->w, j->y + j->h, al_map_rgb(0, 255, 0));
}
