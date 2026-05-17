#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "ennemi.h"
#include "config.h"

// ----------------------------------------------------------------
// Chargement des sprites
// ----------------------------------------------------------------

SpritesEnnemi charger_sprites_ennemi(void) {
    SpritesEnnemi s;
    s.haut[0]      = al_load_bitmap("ennehaut1.png");
    s.haut[1]      = al_load_bitmap("ennehaut2.png");
    s.bas[0]       = al_load_bitmap("ennebas1.png");
    s.bas[1]       = al_load_bitmap("ennebas2.png");
    s.droite[0]    = al_load_bitmap("ennedr1.png");
    s.droite[1]    = al_load_bitmap("ennedr2.png");
    s.gauche       = al_load_bitmap("ennegh2.png");
    s.explosion[0] = al_load_bitmap("3explosion.png");
    s.explosion[1] = al_load_bitmap("4explosion.png");
    s.explosion[2] = al_load_bitmap("5explosion.png");

    if (!s.haut[0])      printf("ERREUR : ennehaut1.png\n");
    if (!s.haut[1])      printf("ERREUR : ennehaut2.png\n");
    if (!s.bas[0])       printf("ERREUR : ennebas1.png\n");
    if (!s.bas[1])       printf("ERREUR : ennebas2.png\n");
    if (!s.droite[0])    printf("ERREUR : ennedr1.png\n");
    if (!s.droite[1])    printf("ERREUR : ennedr2.png\n");
    if (!s.gauche)       printf("ERREUR : ennegh2.png\n");
    if (!s.explosion[0]) printf("ERREUR : 3explosion.png\n");
    if (!s.explosion[1]) printf("ERREUR : 4explosion.png\n");
    if (!s.explosion[2]) printf("ERREUR : 5explosion.png\n");

    return s;
}

void liberer_sprites_ennemi(SpritesEnnemi *s) {
    for (int i = 0; i < 2; i++) {
        if (s->haut[i])   al_destroy_bitmap(s->haut[i]);
        if (s->bas[i])    al_destroy_bitmap(s->bas[i]);
        if (s->droite[i]) al_destroy_bitmap(s->droite[i]);
    }
    if (s->gauche) al_destroy_bitmap(s->gauche);
    for (int i = 0; i < 3; i++)
        if (s->explosion[i]) al_destroy_bitmap(s->explosion[i]);
}

// ----------------------------------------------------------------
// Tirs ennemis
// ----------------------------------------------------------------

void init_tirs_ennemis(TirEnnemi t[], int taille) {
    for (int i = 0; i < taille; i++) {
        t[i].actif = 0;
        t[i].w     = 10;
        t[i].h     = 5;
    }
}

void maj_tirs_ennemis(TirEnnemi t[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (!t[i].actif) continue;
        t[i].x -= VITESSE_TIR_ENNEMI;
        if (t[i].x + t[i].w < 0)
            t[i].actif = 0;
    }
}

void dessiner_tirs_ennemis(TirEnnemi t[], int taille) {
    for (int i = 0; i < taille; i++) {
        if (!t[i].actif) continue;
        al_draw_filled_rectangle(t[i].x, t[i].y,
                                 t[i].x + t[i].w,
                                 t[i].y + t[i].h,
                                 al_map_rgb(255, 80, 0));
    }
}

static void tirer_ennemi(TirEnnemi tirs[], int nb_tirs, float x, float y) {
    for (int i = 0; i < nb_tirs; i++) {
        if (!tirs[i].actif) {
            tirs[i].actif = 1;
            tirs[i].x     = x;
            tirs[i].y     = y + (float)((rand() % 11) - 5);
            break;
        }
    }
}

// ----------------------------------------------------------------
// Init
// ----------------------------------------------------------------

static void reset_ennemi(Ennemi *e) {
    e->actif           = 0;
    e->w               = 96;
    e->h               = 96;
    e->pv              = 1;
    e->temps           = 0;
    e->phase           = 0;
    e->amp             = 0;
    e->freq            = 0;
    e->y_base          = 0;
    e->x_offset        = 0;
    e->cx              = 0;
    e->cy              = 0;
    e->angle           = 0;
    e->rayon           = 0;
    e->en_charge       = 0;
    e->vx              = 0;
    e->vy              = 0;
    e->accel           = 0;
    e->esquive_vy      = 0;
    e->comport         = COMPORT_SERPENT;
    e->cooldown_tir    = 0;
    e->anim_frame      = 0;
    e->anim_timer      = 0;
    e->prev_y          = 0;
    e->en_explosion    = 0;
    e->explosion_frame = 0;
    e->explosion_timer = 0;
}

void init_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++)
        reset_ennemi(&e[i]);
}

void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv) {
    spawn_groupe(e, taille, x, y, 1, pv);
}

void spawn_groupe(Ennemi e[], int taille, float x, float y_centre, int nb, int pv) {
    float amp        = 50.0f + (float)(rand() % 60);
    float freq       = 0.020f + (float)(rand() % 15) / 1000.0f;
    float phase0     = (float)(rand() % 628) / 100.0f;
    float espacement = 110.0f;

    // Choisit un comportement aléatoire pour ce groupe
    ComportEnnemi comport = (ComportEnnemi)(rand() % 4);

    // Centre du tourbillon : avance vers la gauche
    float cx = x;
    float cy = y_centre;
    float rayon_base = 60.0f + (float)(rand() % 40);

    int spawnes = 0;
    for (int i = 0; i < taille && spawnes < nb; i++) {
        if (e[i].actif || e[i].en_explosion) continue;

        reset_ennemi(&e[i]);

        e[i].actif        = 1;
        e[i].w            = 96;
        e[i].h            = 96;
        e[i].pv           = pv;
        e[i].comport      = comport;
        e[i].y_base       = y_centre;
        e[i].amp          = amp;
        e[i].freq         = freq;
        e[i].cooldown_tir = COOLDOWN_TIR_ENNEMI + (rand() % 60);

        // Phase décalée pour effet serpent
        e[i].phase    = phase0 + (float)spawnes * 0.5f;
        e[i].temps    = 0.0f;
        e[i].x_offset = (float)spawnes * espacement;
        e[i].prev_y   = y_centre;

        if (comport == COMPORT_TOURBILLON) {
            // Chaque membre est placé à angle régulier autour du centre
            e[i].cx    = cx;
            e[i].cy    = cy;
            e[i].rayon = rayon_base;
            e[i].angle = phase0 + (float)spawnes * (2.0f * 3.14159f / (float)(nb > 1 ? nb : 1));
            e[i].x     = cx + cosf(e[i].angle) * e[i].rayon;
            e[i].y     = cy + sinf(e[i].angle) * e[i].rayon;
        } else if (comport == COMPORT_KAMIKAZE) {
            e[i].x     = x + e[i].x_offset;
            e[i].y     = y_centre;
            e[i].accel = 0.05f + (float)(rand() % 5) / 100.0f;
            e[i].vx    = -VITESSE_ENNEMI;
            e[i].vy    = 0;
        } else {
            e[i].x = x + e[i].x_offset;
            e[i].y = y_centre;
        }

        spawnes++;
    }
}

// ----------------------------------------------------------------
// Mise à jour
// ----------------------------------------------------------------

void maj_ennemis(Ennemi e[], int taille, float joueur_x, float joueur_y,
                 Tir tirs[], int nb_tirs,
                 TirEnnemi tirs_ennemis[], int nb_tirs_ennemis) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) continue;

        // --- EXPLOSION ---
        if (e[i].en_explosion) {
            e[i].explosion_timer++;
            if (e[i].explosion_timer >= 8) {
                e[i].explosion_frame++;
                e[i].explosion_timer = 0;
                if (e[i].explosion_frame >= 3) {
                    e[i].actif        = 0;
                    e[i].en_explosion = 0;
                }
            }
            continue;
        }

        e[i].prev_y = e[i].y;
        e[i].temps += 1.0f;

        float dx   = joueur_x - e[i].x;
        float dy   = joueur_y - e[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        // --- ESQUIVE DES TIRS ---
        // Vérifie si un tir joueur est proche → dévie verticalement
        float esquive = 0.0f;
        for (int t = 0; t < nb_tirs; t++) {
            if (!tirs[t].actif) continue;
            float tdx = tirs[t].x - e[i].x;
            float tdy = tirs[t].y - (e[i].y + e[i].h / 2.0f);
            float tdist = sqrtf(tdx * tdx + tdy * tdy);
            if (tdist < 120.0f && tdx > -20.0f) {
                // Tir proche et venant vers l'ennemi → esquive
                esquive = (tdy > 0) ? -2.0f : 2.0f;
                break;
            }
        }
        // Interpolation douce de l'esquive
        e[i].esquive_vy += (esquive - e[i].esquive_vy) * 0.1f;

        // --- COMPORTEMENT ---
        if (e[i].comport == COMPORT_KAMIKAZE) {
            // Fonce vers le joueur en accélérant
            if (dist > 5.0f) {
                float cible_vx = (dx / dist) * (VITESSE_CHARGE_MAX + e[i].accel * e[i].temps);
                float cible_vy = (dy / dist) * (VITESSE_CHARGE_MAX + e[i].accel * e[i].temps);
                e[i].vx += (cible_vx - e[i].vx) * 0.05f;
                e[i].vy += (cible_vy - e[i].vy) * 0.05f;
            }
            // Vitesse max limitée pour pas que ça soit injouable
            float speed = sqrtf(e[i].vx * e[i].vx + e[i].vy * e[i].vy);
            if (speed > 6.0f) {
                e[i].vx = (e[i].vx / speed) * 6.0f;
                e[i].vy = (e[i].vy / speed) * 6.0f;
            }
            e[i].x += e[i].vx;
            e[i].y += e[i].vy + e[i].esquive_vy;

        } else if (e[i].comport == COMPORT_TOURBILLON) {
            // Rotation autour d'un centre qui avance vers la gauche
            e[i].cx   -= VITESSE_ENNEMI;
            e[i].angle += 0.04f;
            e[i].x = e[i].cx + cosf(e[i].angle) * e[i].rayon;
            e[i].y = e[i].cy + sinf(e[i].angle) * e[i].rayon + e[i].esquive_vy;

        } else if (e[i].comport == COMPORT_VAGUE) {
            // Tout le groupe ondule ensemble (même phase)
            e[i].x -= VITESSE_ENNEMI;
            float t     = e[i].temps * e[i].freq;
            float new_y = e[i].y_base + sinf(t) * e[i].amp + e[i].x_offset * 0.1f;
            new_y += e[i].esquive_vy;
            if (new_y < 0)                        new_y = 0;
            if (new_y + e[i].h > HAUTEUR_FENETRE) new_y = HAUTEUR_FENETRE - e[i].h;
            e[i].y = new_y;

        } else {
            // COMPORT_SERPENT : sinusoïde avec phase décalée par membre
            // + charge si proche
            if (!e[i].en_charge && dist < DISTANCE_CHARGE) {
                e[i].en_charge = 1;
                if (dist > 0.01f) {
                    e[i].vx = (dx / dist) * VITESSE_CHARGE_MAX;
                    e[i].vy = (dy / dist) * VITESSE_CHARGE_MAX;
                }
            }

            if (e[i].en_charge) {
                if (dist > 5.0f) {
                    float cible_vx = (dx / dist) * VITESSE_CHARGE_MAX;
                    float cible_vy = (dy / dist) * VITESSE_CHARGE_MAX;
                    e[i].vx += (cible_vx - e[i].vx) * 0.03f;
                    e[i].vy += (cible_vy - e[i].vy) * 0.03f;
                }
                e[i].x += e[i].vx;
                e[i].y += e[i].vy + e[i].esquive_vy;
            } else {
                e[i].x -= VITESSE_ENNEMI;
                float t     = e[i].temps * e[i].freq + e[i].phase;
                float new_y = e[i].y_base + sinf(t) * e[i].amp;
                new_y += e[i].esquive_vy;
                if (new_y < 0)                        new_y = 0;
                if (new_y + e[i].h > HAUTEUR_FENETRE) new_y = HAUTEUR_FENETRE - e[i].h;
                e[i].y = new_y;
            }
        }

        // Sortie écran
        if (e[i].x + e[i].w < 0 || e[i].x > LARGEUR_FENETRE + 300 ||
            e[i].y + e[i].h < 0 || e[i].y > HAUTEUR_FENETRE + 100) {
            e[i].actif = 0;
            continue;
        }

        // Animation
        e[i].anim_timer++;
        if (e[i].anim_timer >= 10) {
            e[i].anim_frame = (e[i].anim_frame + 1) % 2;
            e[i].anim_timer = 0;
        }

        // Tir
        if (e[i].cooldown_tir > 0) {
            e[i].cooldown_tir--;
        } else {
            tirer_ennemi(tirs_ennemis, nb_tirs_ennemis,
                         e[i].x, e[i].y + e[i].h / 2.0f - 2.0f);
            e[i].cooldown_tir = COOLDOWN_TIR_ENNEMI;
        }
    }
}

// ----------------------------------------------------------------
// Dessin
// ----------------------------------------------------------------

void dessiner_ennemis(Ennemi e[], int taille, SpritesEnnemi *s) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) continue;

        if (e[i].en_explosion) {
            ALLEGRO_BITMAP *expl = s->explosion[e[i].explosion_frame];
            if (expl) {
                float bw = al_get_bitmap_width(expl);
                float bh = al_get_bitmap_height(expl);
                al_draw_scaled_bitmap(expl, 0, 0, bw, bh,
                                      e[i].x, e[i].y, e[i].w, e[i].h, 0);
            }
            continue;
        }

        float dy = e[i].y - e[i].prev_y;
        ALLEGRO_BITMAP *sprite = NULL;
        int f = e[i].anim_frame;

        if (e[i].en_charge || e[i].comport == COMPORT_KAMIKAZE)
            sprite = s->gauche;
        else if (dy < -0.8f)
            sprite = s->haut[f];
        else if (dy > 0.8f)
            sprite = s->bas[f];
        else
            sprite = s->droite[f];

        if (!sprite) continue;

        float bmp_w = al_get_bitmap_width(sprite);
        float bmp_h = al_get_bitmap_height(sprite);
        al_draw_scaled_bitmap(sprite, 0, 0, bmp_w, bmp_h,
                              e[i].x, e[i].y, e[i].w, e[i].h, 0);
    }
}