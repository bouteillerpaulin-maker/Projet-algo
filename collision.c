#include "collision.h"

int rects_en_collision(float x1, float y1, float w1, float h1,
                       float x2, float y2, float w2, float h2) {
    return !(x1 + w1 < x2 || x1 > x2 + w2 ||
             y1 + h1 < y2 || y1 > y2 + h2);
}

void collisions_tirs_ennemis(Tir tirs[], int ntirs, Ennemi e[], int nennemis, int *score) {
    for (int i = 0; i < ntirs; i++) {
        if (!tirs[i].actif) continue;
        for (int j = 0; j < nennemis; j++) {
            if (!e[j].actif) continue;
            if (rects_en_collision(tirs[i].x, tirs[i].y, tirs[i].w, tirs[i].h,
                                   e[j].x, e[j].y, e[j].w, e[j].h)) {
                tirs[i].actif = 0;
                e[j].pv--;
                if (e[j].pv <= 0) {
                    e[j].actif = 0;
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
        if (!e[i].actif) continue;
        if (rects_en_collision(j->x, j->y, j->w, j->h,
                               e[i].x, e[i].y, e[i].w, e[i].h)) {
            j->vies--;
            if (j->vies <= 0)
                j->vivant = 0;
            e[i].actif = 0;
        }
    }
}
