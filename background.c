#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "background.h"
#include "config.h"

static Etoile etoiles[NB_ETOILES];

// Sol défilant
static ALLEGRO_BITMAP *sol_sprite = NULL;
static float           sol_offset = 0.0f;   // décalage horizontal pour le scroll
#define HAUTEUR_SOL    80                    // hauteur de la bande de sol en pixels
#define VITESSE_SOL    2.0f                  // scroll du sol (légèrement plus lent que le jeu)

void init_background(void) {
    for (int i = 0; i < NB_ETOILES; i++) {
        etoiles[i].x = (float)(rand() % LARGEUR_FENETRE);
        etoiles[i].y = (float)(rand() % (HAUTEUR_FENETRE - HAUTEUR_SOL));

        int couche = rand() % 3;
        if (couche == 0) {
            etoiles[i].vitesse    = 0.5f;
            etoiles[i].luminosite = 0.3f;
            etoiles[i].taille     = 1;
        } else if (couche == 1) {
            etoiles[i].vitesse    = 1.5f;
            etoiles[i].luminosite = 0.6f;
            etoiles[i].taille     = 1;
        } else {
            etoiles[i].vitesse    = 3.0f;
            etoiles[i].luminosite = 1.0f;
            etoiles[i].taille     = 2;
        }
    }

    // Charge le sprite de sol
    if (!sol_sprite)
        sol_sprite = al_load_bitmap("SOL.png");
    if (!sol_sprite)
        printf("ERREUR : SOL.png\n");

    sol_offset = 0.0f;
}

void maj_background(void) {
    // Étoiles
    for (int i = 0; i < NB_ETOILES; i++) {
        etoiles[i].x -= etoiles[i].vitesse;
        if (etoiles[i].x < 0) {
            etoiles[i].x = (float)LARGEUR_FENETRE;
            etoiles[i].y = (float)(rand() % (HAUTEUR_FENETRE - HAUTEUR_SOL));
        }
    }

    // Sol scroll vers la gauche
    sol_offset -= VITESSE_SOL;
    if (sol_sprite) {
        float tile_w = (float)al_get_bitmap_width(sol_sprite);
        if (sol_offset <= -tile_w)
            sol_offset += tile_w;
    }
}

void dessiner_background(void) {
    // Fond noir espace
    al_clear_to_color(al_map_rgb(0, 0, 10));

    // Étoiles
    for (int i = 0; i < NB_ETOILES; i++) {
        int v = (int)(etoiles[i].luminosite * 255);
        ALLEGRO_COLOR couleur = al_map_rgb(v, v, v);

        if (etoiles[i].taille == 1) {
            al_draw_pixel(etoiles[i].x, etoiles[i].y, couleur);
        } else {
            al_draw_filled_rectangle(
                etoiles[i].x, etoiles[i].y,
                etoiles[i].x + 2, etoiles[i].y + 2,
                couleur);
        }
    }

    // Sol défilant — on tile le sprite sur toute la largeur
    if (sol_sprite) {
        float tile_w = (float)al_get_bitmap_width(sol_sprite);
        float tile_h = (float)al_get_bitmap_height(sol_sprite);
        float y_sol  = HAUTEUR_FENETRE - HAUTEUR_SOL;

        float x = sol_offset;
        while (x < LARGEUR_FENETRE) {
            al_draw_scaled_bitmap(sol_sprite,
                                  0, 0, tile_w, tile_h,
                                  x, y_sol,
                                  tile_w, HAUTEUR_SOL,
                                  0);
            x += tile_w;
        }
    } else {
        // Fallback si le sprite n'a pas chargé : bande grise
        al_draw_filled_rectangle(0, HAUTEUR_FENETRE - HAUTEUR_SOL,
                                 LARGEUR_FENETRE, HAUTEUR_FENETRE,
                                 al_map_rgb(40, 40, 50));
    }
}