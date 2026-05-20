#ifndef ENNEMI_H
#define ENNEMI_H
#include "tir.h"

#include <allegro5/allegro.h>

#define MAX_TIRS_ENNEMIS    20
#define VITESSE_TIR_ENNEMI  3.5f    // tirs plus rapides 
#define COOLDOWN_TIR_ENNEMI 100     // légèrement plus fréquent 
#define VITESSE_CHARGE_MAX  10.5f    // kamikazes plus menaçants 
#define DISTANCE_CHARGE     380.0f  // déclenchement charge plus tôt 

// Types de comportement
typedef enum {
    COMPORT_SERPENT   = 0,  // sinusoïde en serpent 
    COMPORT_VAGUE     = 1,  // tout le groupe monte/descend ensemble
    COMPORT_TOURBILLON= 2,  // rotation autour d'un centre
    COMPORT_KAMIKAZE  = 3,  // fonce en accélérant vers le joueur
} ComportEnnemi;

typedef struct {
    ALLEGRO_BITMAP *haut[2];
    ALLEGRO_BITMAP *bas[2];
    ALLEGRO_BITMAP *droite[2];
    ALLEGRO_BITMAP *gauche;
    ALLEGRO_BITMAP *explosion[3];
} SpritesEnnemi;

typedef struct {
    float x, y;
    float w, h;
    int   actif;
    int   pv;

    // Mouvement
    float temps;
    float phase;
    float amp;
    float freq;
    float y_base;
    float x_offset;

    // Centre de rotation (tourbillon)
    float cx, cy;
    float angle;
    float rayon;

    // Charge / kamikaze
    int   en_charge;
    float vx, vy;
    float accel;        // accélération kamikaze

    // Esquive tirs
    float esquive_vy;   // vélocité d'esquive verticale

    // Comportement du groupe
    ComportEnnemi comport;

    // Tir
    int cooldown_tir;

    // Animation
    int   anim_frame;
    int   anim_timer;
    float prev_y;

    // Explosion
    int en_explosion;
    int explosion_frame;
    int explosion_timer;
} Ennemi;

typedef struct {
    float x, y;
    float w, h;
    int   actif;
} TirEnnemi;

SpritesEnnemi charger_sprites_ennemi(void);
void          liberer_sprites_ennemi(SpritesEnnemi *s);

void init_ennemis(Ennemi e[], int taille);
void spawn_groupe(Ennemi e[], int taille, float x, float y_centre, int nb, int pv);
void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv);
void maj_ennemis(Ennemi e[], int taille, float joueur_x, float joueur_y,
                 Tir tirs[], int nb_tirs,
                 TirEnnemi tirs_ennemis[], int nb_tirs_ennemis);
void dessiner_ennemis(Ennemi e[], int taille, SpritesEnnemi *s);

void init_tirs_ennemis(TirEnnemi t[], int taille);
void maj_tirs_ennemis(TirEnnemi t[], int taille);
void dessiner_tirs_ennemis(TirEnnemi t[], int taille);

#endif
