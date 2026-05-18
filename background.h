#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <allegro5/allegro.h>

#define NB_ETOILES  150
#define HAUTEUR_SOL  80      // hauteur bande de sol en pixels
#define VITESSE_SOL  2.0f    // vitesse scroll sol — partagee avec tourelle.c

typedef struct {
    float x, y;
    float vitesse;
    float luminosite;
    int taille;
} Etoile;

void init_background(void);
void maj_background(void);
void dessiner_background(void);

#endif // BACKGROUND_H