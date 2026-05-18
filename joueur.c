#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "joueur.h"
#include "config.h"

static ALLEGRO_BITMAP *sprite_joueur   = NULL;
static ALLEGRO_BITMAP *explosion[5]    = {NULL}; // 1explosion à 5explosion

void init_joueur(Joueur *j) {
    j->w            = 150;
    j->h            = 75;
    j->x            = 100;
    j->y            = HAUTEUR_FENETRE / 2 - j->h / 2;
    j->vies         = 3;
    j->vivant       = 1;
    j->invincible   = 0;
    j->clignotement = 0;
    j->en_explosion    = 0;
    j->explosion_frame = 0;
    j->explosion_timer = 0;

    if (!sprite_joueur)
        sprite_joueur = al_load_bitmap("vaisseau.png");

    // Charge les 5 frames d'explosion (1explosion à 5explosion)
    if (!explosion[0]) {
        explosion[0] = al_load_bitmap("1explosion.png");
        explosion[1] = al_load_bitmap("2explosion.png");
        explosion[2] = al_load_bitmap("3explosion.png");
        explosion[3] = al_load_bitmap("4explosion.png");
        explosion[4] = al_load_bitmap("5explosion.png");
    }
}

// Appelée à chaque fois que le joueur prend un dégât
void joueur_prend_degat(Joueur *j) {
    if (!j->vivant || j->invincible > 0) return; // déjà invincible → ignore

    j->vies--;
    j->en_explosion    = 1;
    j->explosion_frame = 0;
    j->explosion_timer = 0;

    if (j->vies <= 0) {
        j->vivant = 0;
    } else {
        // Active l'invincibilité de 2 secondes
        j->invincible   = DUREE_INVINCIBLE;
        j->clignotement = 0;
    }
}

void maj_joueur(Joueur *j, int haut, int bas, int gauche, int droite) {
    if (!j->vivant) return;

    // Déplacement
    if (haut)   j->y -= VITESSE_JOUEUR;
    if (bas)    j->y += VITESSE_JOUEUR;
    if (gauche) j->x -= VITESSE_JOUEUR;
    if (droite) j->x += VITESSE_JOUEUR;

    if (j->x < 0)                      j->x = 0;
    if (j->y < 0)                      j->y = 0;
    if (j->x + j->w > LARGEUR_FENETRE) j->x = LARGEUR_FENETRE - j->w;
    if (j->y + j->h > HAUTEUR_FENETRE) j->y = HAUTEUR_FENETRE - j->h;

    // Décompte invincibilité
    if (j->invincible > 0) {
        j->invincible--;
        j->clignotement++;
    } else {
        j->clignotement = 0;
    }

    // Avance l'animation d'explosion
    if (j->en_explosion) {
        j->explosion_timer++;
        if (j->explosion_timer >= 5) {  // change de frame toutes les 5 frames
            j->explosion_frame++;
            j->explosion_timer = 0;
            if (j->explosion_frame >= 5)
                j->en_explosion = 0;    // fin de l'animation
        }
    }
}

void dessiner_joueur(Joueur *j) {
    if (!j->vivant) return;

    // Clignotement : invisible une frame sur deux pendant l'invincibilité
    if (j->invincible > 0 && (j->clignotement % 6) < 3)
        return;  // frame invisible → on ne dessine pas

    // Dessin de l'explosion par-dessus le joueur
    if (j->en_explosion && explosion[j->explosion_frame]) {
        ALLEGRO_BITMAP *expl = explosion[j->explosion_frame];
        float bw = al_get_bitmap_width(expl);
        float bh = al_get_bitmap_height(expl);
        // Centrée sur le joueur, un peu plus grande
        float ex = j->x - 20;
        float ey = j->y - 20;
        float ew = j->w + 40;
        float eh = j->h + 40;
        al_draw_scaled_bitmap(expl, 0, 0, bw, bh, ex, ey, ew, eh, 0);
    }

    // Dessin du vaisseau
    if (sprite_joueur) {
        float bmp_w = al_get_bitmap_width(sprite_joueur);
        float bmp_h = al_get_bitmap_height(sprite_joueur);
        al_draw_scaled_bitmap(sprite_joueur,
                              0, 0, bmp_w, bmp_h,
                              j->x, j->y, j->w, j->h, 0);
    } else {
        al_draw_filled_rectangle(j->x, j->y, j->x + j->w, j->y + j->h,
                                 al_map_rgb(0, 255, 0));
    }
}