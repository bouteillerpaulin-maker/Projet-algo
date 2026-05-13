#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "tir.h"
#include "config.h"

void init_tirs(Tir tirs[], int taille) {
    for (int i = 0; i < taille; i++) {
        tirs[i].actif = 0;
        tirs[i].w = 10;
        tirs[i].h = 4;
    }
}

void tirer(Tir tirs[], int taille, float x, float y) {
    for (int i = 0; i < taille; i++) {
        if (!tirs[i].actif) {
            tirs[i].actif = 1;
            tirs[i].x = x;
            tirs[i].y = y;
            break;
        }
    }
}

void maj_tirs(Tir tirs[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (tirs[i].actif) {
            tirs[i].x += VITESSE_TIR;
            if (tirs[i].x > LARGEUR_FENETRE)
                tirs[i].actif = 0;
        }
    }
}

void dessiner_tirs(Tir tirs[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (tirs[i].actif) {
            al_draw_filled_rectangle(tirs[i].x, tirs[i].y,
                                     tirs[i].x + tirs[i].w,
                                     tirs[i].y + tirs[i].h,
                                     al_map_rgb(255, 255, 0));
        }
    }
}
