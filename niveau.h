#ifndef NIVEAU_H
#define NIVEAU_H

#include "ennemi.h"
#include "tourelle.h"

typedef struct {
    int id;
    int temps;
    int fini;
    int boss_phase;
} Niveau;

void init_niveau(Niveau *n, int id);
void script_niveau(Niveau *n, Ennemi e[], int nennemis,
                   Tourelle t[], int ntourelles, int frame);
int  niveau_termine(Niveau *n, Ennemi e[], int nennemis, int boss_vivant);

#endif