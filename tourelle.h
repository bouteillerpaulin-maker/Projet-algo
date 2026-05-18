#ifndef TOURELLE_H
#define TOURELLE_H

#include <allegro5/allegro.h>
#include "tir.h"

#define MAX_TOURELLES       4       // max 4 a l'ecran (etait 10)
#define MAX_MISSILES        8       // peu de missiles en vol (etait 30)
#define COOLDOWN_TOURELLE   360     // tir toutes les 6s a 60fps (etait 180)
#define VITESSE_MISSILE     3    // missile lent et lisible (etait 3.0)
#define TRACKING_FORCE      0.06f   // guidage tres doux (etait 0.06)

typedef struct {
    ALLEGRO_BITMAP *frames[6];
} SpritesTourelle;

typedef struct {
    float x, y;
    float vx, vy;
    int   actif;
    float w, h;
    int   age;          // frames depuis le lancement
} Missile;

#define MISSILE_DUREE_GUIDAGE  300   // guidage actif pendant 300 frames, ensuite vol droit

typedef struct {
    float x, y;
    float w, h;
    int   actif;
    int   pv;
    int   cooldown;
    float angle_vers_joueur;
    int   sprite_frame;
} Tourelle;

SpritesTourelle charger_sprites_tourelle(void);
void            liberer_sprites_tourelle(SpritesTourelle *s);

void init_tourelles(Tourelle t[], int taille);
void init_missiles(Missile m[], int taille);
void spawn_tourelle(Tourelle t[], int taille, float x, float y, int pv);

void maj_tourelles(Tourelle t[], int nb_t,
                   Missile m[], int nb_m,
                   Tir tirs_joueur[], int nb_tirs_joueur,
                   float joueur_x, float joueur_y,
                   int *score);

void maj_missiles(Missile m[], int taille,
                  float joueur_x, float joueur_y);

void dessiner_tourelles(Tourelle t[], int taille, SpritesTourelle *s);
void dessiner_missiles(Missile m[], int taille);

#endif