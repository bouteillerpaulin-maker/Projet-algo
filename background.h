#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <allegro5/allegro.h>

#define NB_ETOILES 150

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
