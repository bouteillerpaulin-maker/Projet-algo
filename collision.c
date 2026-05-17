#include "collision.h"
#include "joueur.h"

int rects_en_collision(float x1, float y1, float w1, float h1,
                       float x2, float y2, float w2, float h2) {
    return !(x1 + w1 < x2 || x1 > x2 + w2 ||
             y1 + h1 < y2 || y1 > y2 + h2);
}

void collisions_tirs_ennemis(Tir tirs[], int ntirs, Ennemi e[], int nennemis, int *score) {
    for (int i = 0; i < ntirs; i++) {
        if (!tirs[i].actif) continue;
        for (int j = 0; j < nennemis; j++) {
            if (!e[j].actif || e[j].en_explosion) continue;
            if (rects_en_collision(tirs[i].x, tirs[i].y, tirs[i].w, tirs[i].h,
                                   e[j].x,    e[j].y,    e[j].w,    e[j].h)) {
                tirs[i].actif = 0;
                e[j].pv--;
                if (e[j].pv <= 0) {
                    e[j].en_explosion    = 1;
                    e[j].explosion_frame = 0;
                    e[j].explosion_timer = 0;
                    if (score) (*score)++;
                }
                break;
            }
        }
    }
}

void collisions_joueur_ennemis(Joueur *j, Ennemi e[], int nennemis) {
    if (!j->vivant) return;
    for (int i = 0; i < nennemis; i++) {
        if (!e[i].actif || e[i].en_explosion) continue;
        if (rects_en_collision(j->x, j->y, j->w, j->h,
                               e[i].x, e[i].y, e[i].w, e[i].h)) {
            joueur_prend_degat(j);
            e[i].en_explosion    = 1;
            e[i].explosion_frame = 0;
            e[i].explosion_timer = 0;
        }
    }
}