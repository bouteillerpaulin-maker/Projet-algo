#ifndef JOUEUR_H
#define JOUEUR_H

typedef struct {
    float x, y;
    float w, h;
    int vies;
    int vivant;
} Joueur;

void init_joueur(Joueur *j);
void maj_joueur(Joueur *j, int haut, int bas, int gauche, int droite);
void dessiner_joueur(Joueur *j);

#endif
