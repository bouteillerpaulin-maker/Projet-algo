#ifndef COLLISION_H
#define COLLISION_H

#include "joueur.h"
#include "tir.h"
#include "ennemi.h"

int rects_en_collision(float x1, float y1, float w1, float h1,
                       float x2, float y2, float w2, float h2);

void collisions_tirs_ennemis(Tir tirs[], int ntirs, Ennemi e[], int nennemis, int *score);
void collisions_joueur_ennemis(Joueur *j, Ennemi e[], int nennemis);

#endif
