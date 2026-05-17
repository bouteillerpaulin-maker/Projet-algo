#ifndef TOURELLE_H
#define TOURELLE_H

#include <allegro5/allegro.h>

#define MAX_TOURELLES       10
#define MAX_MISSILES        30
#define COOLDOWN_TOURELLE   180     // frames entre chaque tir
#define VITESSE_MISSILE     3.0f    // vitesse de base du missile
#define TRACKING_FORCE      0.06f   // force de guidage vers le joueur (0=droit, 1=téléportation)

// Sprites de la tourelle (6 frames directionnelles)
typedef struct {
    ALLEGRO_BITMAP *frames[6];
    // 0 = 3_1left, 1 = 3_2_1left, 2 = 3_2_2leftdown
    // 3 = 3_2_3down, 4 = 3_2uppertleft, 5 = 3_3up
} SpritesTourelle;

typedef struct {
    float x, y;      // position
    float vx, vy;    // vélocité courante
    int   actif;
    float w, h;
} Missile;

typedef struct {
    float x, y;
    float w, h;
    int   actif;
    int   pv;

    int   cooldown;       // compteur avant prochain tir
    float angle_vers_joueur; // angle en radians vers le joueur
    int   sprite_frame;   // frame selon l'angle
} Tourelle;

SpritesTourelle charger_sprites_tourelle(void);
void            liberer_sprites_tourelle(SpritesTourelle *s);

void init_tourelles(Tourelle t[], int taille);
void init_missiles(Missile m[], int taille);

void spawn_tourelle(Tourelle t[], int taille, float x, float y, int pv);

void maj_tourelles(Tourelle t[], int nb_t,
                   Missile m[], int nb_m,
                   float joueur_x, float joueur_y);
void maj_missiles(Missile m[], int taille,
                  float joueur_x, float joueur_y);

void dessiner_tourelles(Tourelle t[], int taille, SpritesTourelle *s);
void dessiner_missiles(Missile m[], int taille);

#endif