#ifndef ENNEMI_H
#define ENNEMI_H

#include <allegro5/allegro.h>

#define MAX_TIRS_ENNEMIS    64
#define VITESSE_TIR_ENNEMI  2.5f
#define COOLDOWN_TIR_ENNEMI 150
#define VITESSE_CHARGE_MAX  2.0f
#define DISTANCE_CHARGE     280.0f

// Sprites directionnels
typedef struct {
    ALLEGRO_BITMAP *haut[2];    // ennehaut1.png, ennehaut2.png
    ALLEGRO_BITMAP *bas[2];     // ennebas1.png,  ennebas2.png
    ALLEGRO_BITMAP *droite[2];  // ennedr1.png,   ennedr2.png
    ALLEGRO_BITMAP *gauche;     // ennegh2.png (une seule frame)
} SpritesEnnemi;

typedef struct {
    float x, y;
    float w, h;
    int   actif;
    int   pv;

    // Mouvement sinusoidal permanent
    float phase;    // phase individuelle (chaque ennemi oscille différemment)
    float amp;      // amplitude verticale
    float freq;     // fréquence d'oscillation
    float y_base;   // centre de l'oscillation

    // Charge vers le joueur
    int   en_charge;
    float vx, vy;

    // Tir
    int cooldown_tir;

    // Animation sprite
    int   anim_frame;  // 0 ou 1
    int   anim_timer;  // compteur changement de frame
    float prev_y;      // pour détecter direction verticale
} Ennemi;

typedef struct {
    float x, y;
    float w, h;
    int   actif;
} TirEnnemi;

SpritesEnnemi charger_sprites_ennemi(void);
void          liberer_sprites_ennemi(SpritesEnnemi *s);

void init_ennemis(Ennemi e[], int taille);
void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv);
void maj_ennemis(Ennemi e[], int taille, float joueur_x, float joueur_y,
                 TirEnnemi tirs[], int nb_tirs);
void dessiner_ennemis(Ennemi e[], int taille, SpritesEnnemi *s);

void init_tirs_ennemis(TirEnnemi t[], int taille);
void maj_tirs_ennemis(TirEnnemi t[], int taille);
void dessiner_tirs_ennemis(TirEnnemi t[], int taille);

#endif