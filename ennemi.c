#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "ennemi.h"
#include "config.h"

void init_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++) {
        e[i].actif = 0;
        e[i].w = 80;
        e[i].h = 80;
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

void dessiner_ennemis(Ennemi e[], int taille, ALLEGRO_BITMAP *sprite) {
    if (!sprite) {
        return;
    }

    float bmp_w = al_get_bitmap_width(sprite);
    float bmp_h = al_get_bitmap_height(sprite);

    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) continue;

        al_draw_scaled_bitmap(sprite,
                              0, 0, bmp_w, bmp_h,
                              e[i].x, e[i].y,
                              e[i].w, e[i].h,
                              0);
    }
}
