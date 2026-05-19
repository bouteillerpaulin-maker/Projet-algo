#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>

#include "boss.h"
#include "config.h"

#define BOSS_SPEED 2
#define DASH_SPEED 12

void init_boss(Boss *b) {

    b->w = 220;
    b->h = 180;

    b->x = LARGEUR_FENETRE + 300;
    b->y = HAUTEUR_FENETRE / 2;

    b->vx = 0;
    b->vy = 0;

    b->pv = 100;

    b->actif = 0;

    b->state = BOSS_IDLE;

    b->timer = 0;

    b->phase2 = 0;

    // =========================
    // CHARGEMENT DES SPRITES
    // =========================

b->idle[0] = al_load_bitmap("bossidle1.png");
b->idle[1] = al_load_bitmap("bossidle2.png");
b->idle[2] = al_load_bitmap("bossidle3.png");

b->loading[0] = al_load_bitmap("bossloading1.png");
b->loading[1] = al_load_bitmap("bossloading2.png");
b->loading[2] = al_load_bitmap("bossloading3.png");

b->charging[0] = al_load_bitmap("bosscharging1.png");
b->charging[1] = al_load_bitmap("bosscharging2.png");
b->charging[2] = al_load_bitmap("bosscharging3.png");

b->reseting[0] = al_load_bitmap("bossreseting1.png");
b->reseting[1] = al_load_bitmap("bossreseting2.png");
b->reseting[2] = al_load_bitmap("bossreseting3.png");
b->reseting[3] = al_load_bitmap("bossreseting4.png");

b->gun = al_load_bitmap("bossgun.png");

b->shooting = al_load_bitmap("bossshooting.png");
}
void activer_boss(Boss *b) {

    b->x = LARGEUR_FENETRE - 350;

    b->y = HAUTEUR_FENETRE / 2;

    b->pv = 100;

    b->actif = 1;

    b->state = BOSS_IDLE;

    b->timer = 0;
}

void maj_boss(Boss *b,
              float joueur_y,
              TirEnnemi tirs[],
              int max_tirs) {

    if (!b->actif)
        return;

    b->timer++;

    // =========================
    // PHASE 2
    // =========================

    if (b->pv <= 50)
        b->phase2 = 1;

    // =========================
    // IDLE
    // =========================

    if (b->state == BOSS_IDLE) {

        if (joueur_y > b->y)
            b->y += BOSS_SPEED;

        if (joueur_y < b->y)
            b->y -= BOSS_SPEED;

        if (b->timer > 120) {

            b->timer = 0;

            int attaque = rand() % 2;

            if (attaque == 0)
                b->state = BOSS_LOADING;

            else
                b->state = BOSS_SHOOTING;
        }
    }

    // =========================
    // LOADING
    // =========================

    else if (b->state == BOSS_LOADING) {

        if (b->timer > 60) {

            b->timer = 0;

            b->state = BOSS_CHARGING;

            b->vx = -DASH_SPEED;

            if (b->phase2)
                b->vx = -18;
        }
    }

    // =========================
    // CHARGING
    // =========================

    else if (b->state == BOSS_CHARGING) {

        b->x += b->vx;

        if (b->x < 50) {

            b->vx = DASH_SPEED;

            if (b->phase2)
                b->vx = 18;
        }

        if (b->x > LARGEUR_FENETRE - 350) {

            b->x = LARGEUR_FENETRE - 350;

            b->vx = 0;

            b->state = BOSS_RESETING;

            b->timer = 0;
        }
    }

    // =========================
    // SHOOTING
    // =========================

    else if (b->state == BOSS_SHOOTING) {

        if (b->timer % 20 == 0) {

            for (int i = 0; i < max_tirs; i++) {

                if (!tirs[i].actif) {

                    tirs[i].actif = 1;

                    tirs[i].x = b->x;
                    tirs[i].y = b->y + b->h / 2;

                    tirs[i].w = 20;
                    tirs[i].h = 10;


                    break;
                }
            }
        }

        if (b->timer > 120) {

            b->timer = 0;

            b->state = BOSS_RESETING;
        }
    }

    // =========================
    // RESET
    // =========================

    else if (b->state == BOSS_RESETING) {

        if (b->timer > 50) {

            b->timer = 0;

            b->state = BOSS_IDLE;
        }
    }
}

void dessiner_boss(Boss *b) {

    if (!b->actif)
        return;

    ALLEGRO_BITMAP *sprite = NULL;

    // =========================
    // IDLE
    // =========================

    if (b->state == BOSS_IDLE) {

        int anim = (b->timer / 15) % 3;

        sprite = b->idle[anim];
    }

    // =========================
    // LOADING
    // =========================

    else if (b->state == BOSS_LOADING) {

        int anim = (b->timer / 10) % 3;

        sprite = b->loading[anim];
    }

    // =========================
    // CHARGING
    // =========================

    else if (b->state == BOSS_CHARGING) {

        int anim = (b->timer / 5) % 3;

        sprite = b->charging[anim];
    }

    // =========================
    // SHOOTING
    // =========================

    else if (b->state == BOSS_SHOOTING) {

        sprite = b->shooting;
    }

    // =========================
    // RESET
    // =========================

    else if (b->state == BOSS_RESETING) {

        int anim = (b->timer / 10) % 4;

        sprite = b->reseting[anim];
    }

    if (sprite) {

        al_draw_bitmap(
            sprite,
            b->x,
            b->y,
            0
        );
    }

    // =========================
    // BARRE DE VIE
    // =========================

    al_draw_filled_rectangle(
        300,
        40,
        300 + b->pv * 6,
        70,
        al_map_rgb(255, 0, 0)
    );

    al_draw_rectangle(
        300,
        40,
        900,
        70,
        al_map_rgb(255,255,255),
        3
    );
}