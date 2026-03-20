#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "ennemi.h"
#include "config.h"

void init_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++) {
        e[i].actif = 0;
        e[i].w = 30;
        e[i].h = 20;
        e[i].pv = 1;
    }
}

void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) {
            e[i].actif = 1;
            e[i].x = x;
            e[i].y = y;
            e[i].pv = pv;
            break;
        }
    }
}

void maj_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (e[i].actif) {
            e[i].x -= VITESSE_ENNEMI;
            if (e[i].x + e[i].w < 0)
                e[i].actif = 0;
        }
    }
}

void dessiner_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (e[i].actif) {
            al_draw_filled_rectangle(e[i].x, e[i].y,
                                     e[i].x + e[i].w,
                                     e[i].y + e[i].h,
                                     al_map_rgb(255, 0, 0));
        }
    }
}
