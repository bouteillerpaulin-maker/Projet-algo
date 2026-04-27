#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "boss.h"
#include "config.h"

void init_boss(Boss *b) {
    b->w = 120;
    b->h = 80;
    b->x = LARGEUR_FENETRE + 200;
    b->y = HAUTEUR_FENETRE / 2 - b->h / 2;
    b->pv = 30;
    b->actif = 0;
    b->direction = 1;
}

void activer_boss(Boss *b) {
    b->x = LARGEUR_FENETRE - b->w - 50;
    b->y = HAUTEUR_FENETRE / 2 - b->h / 2;
    b->pv = 30;
    b->actif = 1;
}

void maj_boss(Boss *b) {
    if (!b->actif) return;

    b->y += b->direction * VITESSE_BOSS;
    if (b->y < 0 || b->y + b->h > HAUTEUR_FENETRE)
        b->direction *= -1;
}

void dessiner_boss(Boss *b) {
    if (!b->actif) return;
    al_draw_filled_rectangle(b->x, b->y, b->x + b->w, b->y + b->h, al_map_rgb(150, 0, 150));
}
