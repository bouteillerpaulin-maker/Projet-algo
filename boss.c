#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>

#include "boss.h"
#include "config.h"

#define DASH_SPEED 14

// =========================
// INIT
// =========================

void init_boss(Boss *b) {

    b->spawn_timer = 0;

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

    b->shoot_burst = 0;

    // =========================
    // SPRITES
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

// =========================
// ACTIVER
// =========================

void activer_boss(Boss *b) {

    b->spawn_timer = 600; // 4 secondes à 60 FPS
    b->state = BOSS_IDLE;
    b->timer = 0;
    b->x = LARGEUR_FENETRE - 350;
    b->y = HAUTEUR_FENETRE / 2;

    b->pv = 100;
    b->actif = 1;

    b->state = BOSS_IDLE;

    b->timer = 0;
}

// =========================
// UPDATE IA
// =========================

void maj_boss(Boss *b,
              float joueur_y,
              TirEnnemi tirs[],
              int max_tirs) {
                if (!b->actif)
    return;

// =========================
// SPAWN DELAY (4 SECONDES)
// =========================

if (b->spawn_timer > 0) {
    b->spawn_timer--;
    return; // boss inactif
}
    if (!b->actif)
        return;

    b->timer++;

    // =========================
    // PHASE 2
    // =========================

    if (b->pv <= 50)
        b->phase2 = 1;

    // =========================
    // TRACKING Y (ADOUCI)
    // =========================

    float speed_y = b->phase2 ? 2.0f : 1.5f;

float diff = joueur_y - b->y;

    // zone morte plus large = moins de micro-ajustements
    if (diff > 60)
        b->y += speed_y;
    else if (diff < -60)
        b->y -= speed_y;

    // =========================
    // IDLE → CHOIX ACTION
    // =========================

    if (b->state == BOSS_IDLE) {

        int delay = b->phase2 ? 35 : 60;

        if (b->timer > delay) {

            b->timer = 0;

            int r = rand() % 100;

            if (r < 65) {
                b->state = BOSS_CHARGING;
                b->vx = b->phase2 ? -14 : -11;
            }
            else {
                b->state = BOSS_SHOOTING;
                b->shoot_burst = b->phase2 ? 6 : 4;
            }
        }
    }

    // =========================
    // CHARGING (RUSH + SHOOT)
    // =========================

    else if (b->state == BOSS_CHARGING) {

        b->x += b->vx;

        if (b->timer % 10 == 0) {

            for (int i = 0; i < max_tirs; i++) {

                if (!tirs[i].actif) {

                    tirs[i].actif = 1;
                    tirs[i].x = b->x;
                    tirs[i].y = b->y + (rand() % b->h);
                    tirs[i].w = 15;
                    tirs[i].h = 8;

                    break;
                }
            }
        }

        if (b->x < 50)
            b->vx = b->phase2 ? 17 : 14;

        if (b->x > LARGEUR_FENETRE - 350) {

            b->x = LARGEUR_FENETRE - 350;
            b->vx = 0;

            b->timer = 0;
            b->state = BOSS_IDLE;
        }
    }

    // =========================
    // SHOOT BURST
    // =========================

    else if (b->state == BOSS_SHOOTING) {

        if (b->shoot_burst > 0 && b->timer % 8 == 0) {

            for (int i = 0; i < max_tirs; i++) {

                if (!tirs[i].actif) {

                    tirs[i].actif = 1;
                    tirs[i].x = b->x;
                    tirs[i].y = b->y + (rand() % b->h);

                    b->shoot_burst--;
                    break;
                }
            }
        }

        if (b->shoot_burst <= 0) {

            b->timer = 0;
            b->state = BOSS_IDLE;
        }
    }

    // =========================
    // RESET
    // =========================

    else if (b->state == BOSS_RESETING) {

        if (b->timer > 40) {

            b->timer = 0;
            b->state = BOSS_IDLE;
        }
    }
}

// =========================
// DRAW
// =========================

void dessiner_boss(Boss *b) {

    if (!b->actif)
        return;

    ALLEGRO_BITMAP *sprite = NULL;

    if (b->state == BOSS_IDLE) {
        sprite = b->idle[(b->timer / 15) % 3];
    }
    else if (b->state == BOSS_CHARGING) {
        sprite = b->charging[(b->timer / 5) % 3];
    }
    else if (b->state == BOSS_SHOOTING) {
        sprite = b->shooting;
    }
    else if (b->state == BOSS_RESETING) {
        sprite = b->reseting[(b->timer / 10) % 4];
    }

    if (sprite) {
        al_draw_bitmap(sprite, b->x, b->y, 0);
    }

    // =========================
    // HP BAR
    // =========================

    al_draw_filled_rectangle(
        300, 40,
        300 + b->pv * 6, 70,
        al_map_rgb(255, 0, 0)
    );

    al_draw_rectangle(
        300, 40,
        900, 70,
        al_map_rgb(255,255,255),
        3
    );
}