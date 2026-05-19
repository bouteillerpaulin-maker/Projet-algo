#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro.h>
#include "ennemi.h"

typedef enum {
    BOSS_IDLE,
    BOSS_CHARGING,
    BOSS_SHOOTING,
    BOSS_RESETING
} BossState;

typedef struct {
    
    int spawn_timer;
    // =========================
    // POSITION / PHYSIQUE
    // =========================
    float x, y;
    float vx, vy;

    int w, h;

    // =========================
    // STATS
    // =========================
    int pv;
    int actif;
    int phase2;

    // =========================
    // IA / COMBAT
    // =========================
    BossState state;

    int timer;
    int shoot_burst;

    // =========================
    // SPRITES
    // =========================
    ALLEGRO_BITMAP *idle[3];
    ALLEGRO_BITMAP *loading[3];
    ALLEGRO_BITMAP *charging[3];
    ALLEGRO_BITMAP *reseting[4];

    ALLEGRO_BITMAP *gun;
    ALLEGRO_BITMAP *shooting;

} Boss;

// =========================
// FONCTIONS
// =========================
void init_boss(Boss *b);

void activer_boss(Boss *b);

void maj_boss(Boss *b,
              float joueur_y,
              TirEnnemi tirs[],
              int max_tirs);

void dessiner_boss(Boss *b);

#endif