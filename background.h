#ifndef BACKGROUND_H
#define BACKGROUND_H

#define NB_ETOILES 150

typedef struct {
    float x, y;
    float vitesse;
    float luminosite; // 0.0 à 1.0
    int taille;       // 1 ou 2 pixels
} Etoile;

void init_background(void);
void maj_background(void);
void dessiner_background(void);

#endif