#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "tourelle.h"
#include "tir.h"
#include "config.h"
#include "background.h"

SpritesTourelle charger_sprites_tourelle(void) {
    SpritesTourelle s;
    s.frames[0] = al_load_bitmap("3.1left.png");
    s.frames[1] = al_load_bitmap("3.2.1left.png");
    s.frames[2] = al_load_bitmap("3.2.2leftdown.png");
    s.frames[3] = al_load_bitmap("3.2.3down.png");
    s.frames[4] = al_load_bitmap("3.2uppertleft.png");
    s.frames[5] = al_load_bitmap("3.3up.png");

    if (!s.frames[0]) printf("ERREUR : 3.1left.png\n");
    if (!s.frames[1]) printf("ERREUR : 3.2.1left.png\n");
    if (!s.frames[2]) printf("ERREUR : 3.2.2leftdown.png\n");
    if (!s.frames[3]) printf("ERREUR : 3.2.3down.png\n");
    if (!s.frames[4]) printf("ERREUR : 3.2uppertleft.png\n");
    if (!s.frames[5]) printf("ERREUR : 3.3up.png\n");

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
        t[i].actif             = 0;
        t[i].w                 = 160;
        t[i].h                 = 160;
        t[i].pv                = 2;
        t[i].cooldown          = COOLDOWN_TOURELLE;
        t[i].angle_vers_joueur = 0;
        t[i].sprite_frame      = 0;
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
    (void)y; // ignoree : on force toujours la tourelle au sol
    for (int i = 0; i < taille; i++) {
        if (!t[i].actif) {
            t[i].actif        = 1;
            t[i].x            = x;
            t[i].y            = HAUTEUR_FENETRE - 160.0f; // collee au bas de l'ecran
            t[i].w            = 160;
            t[i].h            = 160;
            t[i].pv           = pv;
            t[i].cooldown     = COOLDOWN_TOURELLE + rand() % 120;
            t[i].sprite_frame = 0;
            break;
        }
    }
}

// ----------------------------------------------------------------
// Angle → frame sprite
// Tourelles au sol → joueur toujours au-dessus (angle ~270° en Allegro)
// ----------------------------------------------------------------
static int frame_depuis_angle(float angle) {
    float deg = angle * 180.0f / 3.14159f;
    if (deg < 0) deg += 360.0f;

    if (deg >= 150.0f && deg < 210.0f) return 0; // gauche
    if (deg >= 90.0f  && deg < 150.0f) return 1; // haut-gauche (= vers le haut ecran)
    if (deg >= 30.0f  && deg < 90.0f)  return 2; // haut-droite
    if (deg >= 330.0f || deg < 30.0f)  return 3; // droite
    if (deg >= 270.0f && deg < 330.0f) return 4; // bas-droite
    if (deg >= 210.0f && deg < 270.0f) return 5; // bas (rare depuis le sol)
    return 0;
}

// ----------------------------------------------------------------
// Mise a jour tourelles — destructibles par les tirs joueur
// ----------------------------------------------------------------

void maj_tourelles(Tourelle t[], int nb_t,
                   Missile m[], int nb_m,
                   Tir tirs_joueur[], int nb_tirs_joueur,
                   float joueur_x, float joueur_y,
                   int *score) {
    for (int i = 0; i < nb_t; i++) {
        if (!t[i].actif) continue;

        // Calée sur la vitesse du sol (VITESSE_SOL dans background.h)
        t[i].x -= VITESSE_SOL;
        // Toujours collee au sol
        t[i].y = HAUTEUR_FENETRE - 160.0f;

        if (t[i].x + t[i].w < 0) {
            t[i].actif = 0;
            continue;
        }

        // --- Collision avec les tirs du joueur ---
        for (int j = 0; j < nb_tirs_joueur; j++) {
            if (!tirs_joueur[j].actif) continue;
            if (tirs_joueur[j].x < t[i].x + t[i].w &&
                tirs_joueur[j].x + tirs_joueur[j].w > t[i].x &&
                tirs_joueur[j].y < t[i].y + t[i].h &&
                tirs_joueur[j].y + tirs_joueur[j].h > t[i].y) {
                tirs_joueur[j].actif = 0;
                t[i].pv--;
                if (t[i].pv <= 0) {
                    t[i].actif = 0;
                    if (score) *score += 50;
                    break;
                }
            }
        }
        if (!t[i].actif) continue;

        // Angle vers le joueur
        float dx = joueur_x - (t[i].x + t[i].w / 2.0f);
        float dy = joueur_y - (t[i].y + t[i].h / 2.0f);
        t[i].angle_vers_joueur = atan2f(dy, dx);
        t[i].sprite_frame = frame_depuis_angle(t[i].angle_vers_joueur);

        // Tir
        if (t[i].cooldown > 0) {
            t[i].cooldown--;
        } else {
            for (int j = 0; j < nb_m; j++) {
                if (!m[j].actif) {
                    m[j].actif = 1;
                    m[j].age  = 0;
                    m[j].x    = t[i].x + t[i].w / 2.0f;
                    m[j].y    = t[i].y + t[i].h / 2.0f;
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
// Missiles a tete chercheuse douce
// ----------------------------------------------------------------

void maj_missiles(Missile m[], int taille,
                  float joueur_x, float joueur_y) {
    for (int i = 0; i < taille; i++) {
        if (!m[i].actif) continue;

        m[i].age++;

        // Guidage actif seulement pendant les premieres frames
        if (m[i].age < MISSILE_DUREE_GUIDAGE) {
            float dx   = joueur_x - m[i].x;
            float dy   = joueur_y - m[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 5.0f) {
                float cvx = (dx / dist) * VITESSE_MISSILE;
                float cvy = (dy / dist) * VITESSE_MISSILE;
                m[i].vx += (cvx - m[i].vx) * TRACKING_FORCE;
                m[i].vy += (cvy - m[i].vy) * TRACKING_FORCE;
            }
        }
        // Apres MISSILE_DUREE_GUIDAGE : vol rectiligne, plus de correction

        m[i].x += m[i].vx;
        m[i].y += m[i].vy;

        if (m[i].x < -50 || m[i].x > LARGEUR_FENETRE + 50 ||
            m[i].y < -50 || m[i].y > HAUTEUR_FENETRE + 50)
            m[i].actif = 0;
    }
}

// ----------------------------------------------------------------
// Dessin — avec barre de vie si pv >= 2
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

        float angle = atan2f(m[i].vy, m[i].vx);
        float nx    = cosf(angle);
        float ny    = sinf(angle);

        al_draw_filled_rectangle(m[i].x - nx * 8, m[i].y - ny * 8,
                                 m[i].x + nx * 8, m[i].y + ny * 8,
                                 al_map_rgb(255, 160, 0));
        al_draw_filled_circle(m[i].x + nx * 8, m[i].y + ny * 8,
                              4, al_map_rgb(255, 80, 0));
        al_draw_filled_circle(m[i].x - nx * 10, m[i].y - ny * 10,
                              3, al_map_rgba(255, 200, 100, 120));
    }
}