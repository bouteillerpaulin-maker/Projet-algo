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

    s.haut[0]  = al_load_bitmap("ennehaut1.png");
    s.haut[1]  = al_load_bitmap("ennehaut2.png");
    s.bas[0]   = al_load_bitmap("ennebas1.png");
    s.bas[1]   = al_load_bitmap("ennebas2.png");
    s.droite[0]= al_load_bitmap("ennedr1.png");
    s.droite[1]= al_load_bitmap("ennedr2.png");
    s.gauche   = al_load_bitmap("ennegh2.png");

    // Vérification
    if (!s.haut[0])   printf("ERREUR : ennehaut1.png\n");
    if (!s.haut[1])   printf("ERREUR : ennehaut2.png\n");
    if (!s.bas[0])    printf("ERREUR : ennebas1.png\n");
    if (!s.bas[1])    printf("ERREUR : ennebas2.png\n");
    if (!s.droite[0]) printf("ERREUR : ennedr1.png\n");
    if (!s.droite[1]) printf("ERREUR : ennedr2.png\n");
    if (!s.gauche)    printf("ERREUR : ennegh2.png\n");

    return s;
}

void liberer_sprites_ennemi(SpritesEnnemi *s) {
    for (int i = 0; i < 2; i++) {
        if (s->haut[i])   al_destroy_bitmap(s->haut[i]);
        if (s->bas[i])    al_destroy_bitmap(s->bas[i]);
        if (s->droite[i]) al_destroy_bitmap(s->droite[i]);
    }
    if (s->gauche) al_destroy_bitmap(s->gauche);
}

// ----------------------------------------------------------------
// Tirs ennemis
// ----------------------------------------------------------------

void init_tirs_ennemis(TirEnnemi t[], int taille) {
    for (int i = 0; i < taille; i++) {
        t[i].actif = 0;
        t[i].w = 8;
        t[i].h = 4;
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
            tirs[i].x = x;
            tirs[i].y = y + (float)((rand() % 11) - 5);
            break;
        }
    }
}

// ----------------------------------------------------------------
// Ennemis
// ----------------------------------------------------------------

void init_ennemis(Ennemi e[], int taille) {
    for (int i = 0; i < taille; i++) {
        e[i].actif        = 0;
        e[i].w            = 64;
        e[i].h            = 64;
        e[i].pv           = 1;
        e[i].phase        = 0;
        e[i].amp          = 0;
        e[i].freq         = 0;
        e[i].y_base       = 0;
        e[i].en_charge    = 0;
        e[i].vx           = 0;
        e[i].vy           = 0;
        e[i].cooldown_tir = 0;
        e[i].anim_frame   = 0;
        e[i].anim_timer   = 0;
        e[i].prev_y       = 0;
    }
}

void spawn_ennemi(Ennemi e[], int taille, float x, float y, int pv) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) {
            e[i].actif        = 1;
            e[i].x            = x;
            e[i].y            = y;
            e[i].w            = 64;
            e[i].h            = 64;
            e[i].pv           = pv;
            e[i].y_base       = y;
            // Chaque ennemi a une oscillation unique
            e[i].phase        = (float)(rand() % 628) / 100.0f; // 0 à 2*PI
            e[i].amp          = 30.0f + (float)(rand() % 40);   // 30 à 70px
            e[i].freq         = 0.03f + (float)(rand() % 30) / 1000.0f; // fréquence légèrement variable
            e[i].en_charge    = 0;
            e[i].vx           = 0;
            e[i].vy           = 0;
            e[i].cooldown_tir = COOLDOWN_TIR_ENNEMI;
            e[i].anim_frame   = 0;
            e[i].anim_timer   = 0;
            e[i].prev_y       = y;
            break;
        }
    }
}

void maj_ennemis(Ennemi e[], int taille, float joueur_x, float joueur_y,
                 TirEnnemi tirs[], int nb_tirs) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) continue;

        e[i].prev_y = e[i].y;

        float dx = joueur_x - e[i].x;
        float dy = joueur_y - e[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (!e[i].en_charge && dist < DISTANCE_CHARGE) {
            e[i].en_charge = 1;
            if (dist > 0.01f) {
                e[i].vx = (dx / dist) * VITESSE_CHARGE_MAX;
                e[i].vy = (dy / dist) * VITESSE_CHARGE_MAX;
            }
        }

        if (e[i].en_charge) {
            // Poursuite douce avec interpolation
            if (dist > 5.0f) {
                float cible_vx = (dx / dist) * VITESSE_CHARGE_MAX;
                float cible_vy = (dy / dist) * VITESSE_CHARGE_MAX;
                e[i].vx += (cible_vx - e[i].vx) * 0.04f;
                e[i].vy += (cible_vy - e[i].vy) * 0.04f;
            }
            e[i].x += e[i].vx;
            e[i].y += e[i].vy;
        } else {
            // Avance vers la gauche + ondulation verticale sinusoïdale permanente
            e[i].x -= VITESSE_ENNEMI;
            e[i].phase += e[i].freq;

            // Le y_base descend légèrement avec x pour donner un effet de trajectoire courbe
            float new_y = e[i].y_base + sinf(e[i].phase) * e[i].amp;

            // Clamp dans l'écran
            if (new_y < 0) new_y = 0;
            if (new_y + e[i].h > HAUTEUR_FENETRE) new_y = HAUTEUR_FENETRE - e[i].h;

            e[i].y = new_y;
        }

        // Sortie écran
        if (e[i].x + e[i].w < 0 || e[i].x > LARGEUR_FENETRE ||
            e[i].y + e[i].h < 0 || e[i].y > HAUTEUR_FENETRE) {
            e[i].actif = 0;
            continue;
        }

        // Animation : change de frame toutes les 12 frames
        e[i].anim_timer++;
        if (e[i].anim_timer >= 12) {
            e[i].anim_frame = (e[i].anim_frame + 1) % 2;
            e[i].anim_timer = 0;
        }

        // Tir
        if (e[i].cooldown_tir > 0) {
            e[i].cooldown_tir--;
        } else {
            tirer_ennemi(tirs, nb_tirs,
                         e[i].x,
                         e[i].y + e[i].h / 2.0f - 2.0f);
            e[i].cooldown_tir = COOLDOWN_TIR_ENNEMI;
        }
    }
}

void dessiner_ennemis(Ennemi e[], int taille, SpritesEnnemi *s) {
    for (int i = 0; i < taille; i++) {
        if (!e[i].actif) continue;

        float dy = e[i].y - e[i].prev_y;
        ALLEGRO_BITMAP *sprite = NULL;
        int f = e[i].anim_frame; // 0 ou 1

        if (e[i].en_charge) {
            // En charge : sprite gauche (fonce vers le joueur à gauche)
            sprite = s->gauche;
        } else if (dy < -0.5f) {
            // Monte
            sprite = s->haut[f];
        } else if (dy > 0.5f) {
            // Descend
            sprite = s->bas[f];
        } else {
            // Horizontal : droite ou gauche selon mouvement x
            // Les ennemis vont vers la gauche donc on utilise droite (ils "regardent" à gauche)
            sprite = s->droite[f];
        }

        if (!sprite) continue;

        float bmp_w = al_get_bitmap_width(sprite);
        float bmp_h = al_get_bitmap_height(sprite);

        al_draw_scaled_bitmap(sprite,
                              0, 0, bmp_w, bmp_h,
                              e[i].x, e[i].y,
                              e[i].w, e[i].h,
                              0);
    }
}