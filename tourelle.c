#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "tourelle.h"
#include "config.h"

// ----------------------------------------------------------------
// Chargement des sprites
// ----------------------------------------------------------------

SpritesTourelle charger_sprites_tourelle(void) {
    SpritesTourelle s;
    s.frames[0] = al_load_bitmap("3_1left.png");
    s.frames[1] = al_load_bitmap("3_2_1left.png");
    s.frames[2] = al_load_bitmap("3_2_2leftdown.png");
    s.frames[3] = al_load_bitmap("3_2_3down.png");
    s.frames[4] = al_load_bitmap("3_2uppertleft.png");
    s.frames[5] = al_load_bitmap("3_3up.png");

    if (!s.frames[0]) printf("ERREUR : 3_1left.png\n");
    if (!s.frames[1]) printf("ERREUR : 3_2_1left.png\n");
    if (!s.frames[2]) printf("ERREUR : 3_2_2leftdown.png\n");
    if (!s.frames[3]) printf("ERREUR : 3_2_3down.png\n");
    if (!s.frames[4]) printf("ERREUR : 3_2uppertleft.png\n");
    if (!s.frames[5]) printf("ERREUR : 3_3up.png\n");

    return s;
}

void liberer_sprites_tourelle(SpritesTourelle *s) {
    for (int i = 0; i < 6; i++)
        if (s->frames[i]) al_destroy_bitmap(s->frames[i]);
}

// ----------------------------------------------------------------
// Init
// ----------------------------------------------------------------

void init_tourelles(Tourelle t[], int taille) {
    for (int i = 0; i < taille; i++) {
        t[i].actif        = 0;
        t[i].w            = 80;
        t[i].h            = 80;
        t[i].pv           = 3;
        t[i].cooldown     = COOLDOWN_TOURELLE;
        t[i].angle_vers_joueur = 0;
        t[i].sprite_frame = 0;
    }
}

void init_missiles(Missile m[], int taille) {
    for (int i = 0; i < taille; i++) {
        m[i].actif = 0;
        m[i].w     = 12;
        m[i].h     = 6;
    }
}

void spawn_tourelle(Tourelle t[], int taille, float x, float y, int pv) {
    for (int i = 0; i < taille; i++) {
        if (!t[i].actif) {
            t[i].actif    = 1;
            t[i].x        = x;
            t[i].y        = y;
            t[i].w        = 80;
            t[i].h        = 80;
            t[i].pv       = pv;
            t[i].cooldown = COOLDOWN_TOURELLE + rand() % 60;
            t[i].sprite_frame = 0;
            break;
        }
    }
}

// ----------------------------------------------------------------
// Choisit la frame selon l'angle vers le joueur
// ----------------------------------------------------------------
static int frame_depuis_angle(float angle) {
    // angle en radians, 0 = droite, PI = gauche, -PI/2 = haut
    float deg = angle * 180.0f / 3.14159f;
    if (deg < 0) deg += 360.0f;

    // 6 frames : gauche, haut-gauche, haut-gauche-bas, bas, haut-gauche (2), haut
    if (deg >= 150.0f && deg < 210.0f) return 0; // gauche
    if (deg >= 210.0f && deg < 270.0f) return 1; // bas-gauche
    if (deg >= 270.0f && deg < 330.0f) return 2; // bas-gauche 2
    if (deg >= 330.0f || deg < 30.0f)  return 3; // bas / droite
    if (deg >= 30.0f  && deg < 90.0f)  return 4; // haut-gauche
    if (deg >= 90.0f  && deg < 150.0f) return 5; // haut
    return 0;
}

// ----------------------------------------------------------------
// Mise à jour tourelles
// ----------------------------------------------------------------

void maj_tourelles(Tourelle t[], int nb_t,
                   Missile m[], int nb_m,
                   float joueur_x, float joueur_y) {
    for (int i = 0; i < nb_t; i++) {
        if (!t[i].actif) continue;

        // Avance vers la gauche doucement (la tourelle défile comme le sol)
        t[i].x -= 2.0f;
        if (t[i].x + t[i].w < 0) {
            t[i].actif = 0;
            continue;
        }

        // Calcule l'angle vers le joueur
        float dx = joueur_x - (t[i].x + t[i].w / 2.0f);
        float dy = joueur_y - (t[i].y + t[i].h / 2.0f);
        t[i].angle_vers_joueur = atan2f(dy, dx);
        t[i].sprite_frame = frame_depuis_angle(t[i].angle_vers_joueur);

        // Tir
        if (t[i].cooldown > 0) {
            t[i].cooldown--;
        } else {
            // Cherche un slot missile libre
            for (int j = 0; j < nb_m; j++) {
                if (!m[j].actif) {
                    m[j].actif = 1;
                    m[j].x = t[i].x + t[i].w / 2.0f;
                    m[j].y = t[i].y + t[i].h / 2.0f;
                    // Vitesse initiale vers le joueur
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist > 0.01f) {
                        m[j].vx = (dx / dist) * VITESSE_MISSILE;
                        m[j].vy = (dy / dist) * VITESSE_MISSILE;
                    }
                    break;
                }
            }
            t[i].cooldown = COOLDOWN_TOURELLE;
        }
    }
}

// ----------------------------------------------------------------
// Mise à jour missiles (tête chercheuse)
// ----------------------------------------------------------------

void maj_missiles(Missile m[], int taille,
                  float joueur_x, float joueur_y) {
    for (int i = 0; i < taille; i++) {
        if (!m[i].actif) continue;

        // Calcule la direction vers le joueur
        float dx   = joueur_x - m[i].x;
        float dy   = joueur_y - m[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 5.0f) {
            // Vitesse cible vers le joueur
            float cible_vx = (dx / dist) * VITESSE_MISSILE;
            float cible_vy = (dy / dist) * VITESSE_MISSILE;
            // Interpolation douce → tête chercheuse progressive
            m[i].vx += (cible_vx - m[i].vx) * TRACKING_FORCE;
            m[i].vy += (cible_vy - m[i].vy) * TRACKING_FORCE;
        }

        m[i].x += m[i].vx;
        m[i].y += m[i].vy;

        // Sortie écran
        if (m[i].x < -50 || m[i].x > LARGEUR_FENETRE + 50 ||
            m[i].y < -50 || m[i].y > HAUTEUR_FENETRE + 50) {
            m[i].actif = 0;
        }
    }
}

// ----------------------------------------------------------------
// Dessin
// ----------------------------------------------------------------

void dessiner_tourelles(Tourelle t[], int taille, SpritesTourelle *s) {
    for (int i = 0; i < taille; i++) {
        if (!t[i].actif) continue;

        ALLEGRO_BITMAP *sprite = s->frames[t[i].sprite_frame];
        if (!sprite) continue;

        float bw = al_get_bitmap_width(sprite);
        float bh = al_get_bitmap_height(sprite);
        al_draw_scaled_bitmap(sprite, 0, 0, bw, bh,
                              t[i].x, t[i].y, t[i].w, t[i].h, 0);
    }
}

void dessiner_missiles(Missile m[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (!m[i].actif) continue;

        // Missile jaune-orange avec petit cercle à la tête
        float angle = atan2f(m[i].vy, m[i].vx);
        float nx    = cosf(angle);
        float ny    = sinf(angle);

        // Corps du missile
        al_draw_filled_rectangle(m[i].x - nx * 8, m[i].y - ny * 8,
                                 m[i].x + nx * 8, m[i].y + ny * 8,
                                 al_map_rgb(255, 160, 0));
        // Tête
        al_draw_filled_circle(m[i].x + nx * 8, m[i].y + ny * 8,
                              4, al_map_rgb(255, 80, 0));
        // Trainée
        al_draw_filled_circle(m[i].x - nx * 10, m[i].y - ny * 10,
                              3, al_map_rgba(255, 200, 100, 120));
    }
}