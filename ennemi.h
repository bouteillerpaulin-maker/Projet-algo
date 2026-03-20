#ifndef ENNEMI_H
#define ENNEMI_H

typedef struct {
    float x, y;
    float w, h;
    int actif;
    int pv;
} Ennemi;

void init_ennemis(Ennemi e[], int taille);
void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv);
void maj_ennemis(Ennemi e[], int taille);
void dessiner_ennemis(Ennemi e[], int taille);

#endif
