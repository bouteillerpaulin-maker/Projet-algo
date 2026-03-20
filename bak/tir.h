#ifndef TIR_H
#define TIR_H

typedef struct {
    float x, y;
    float w, h;
    int actif;
} Tir;

void init_tirs(Tir tirs[], int taille);
void tirer(Tir tirs[], int taille, float x, float y);
void maj_tirs(Tir tirs[], int taille);
void dessiner_tirs(Tir tirs[], int taille);

#endif
