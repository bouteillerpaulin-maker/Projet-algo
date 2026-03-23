#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include "background.h"
#include "config.h"
 
static Etoile etoiles[NB_ETOILES];
 
void init_background(void) {
    for (int i = 0; i < NB_ETOILES; i++) {
        // Position initiale aléatoire sur tout l'écran
        etoiles[i].x = (float)(rand() % LARGEUR_FENETRE);
        etoiles[i].y = (float)(rand() % HAUTEUR_FENETRE);
 
        // 3 couches de vitesse pour un effet de profondeur
        int couche = rand() % 3;
        if (couche == 0) {
            etoiles[i].vitesse    = 0.5f;   // lointaines
            etoiles[i].luminosite = 0.3f;
            etoiles[i].taille     = 1;
        } else if (couche == 1) {
            etoiles[i].vitesse    = 1.5f;   // moyennes
            etoiles[i].luminosite = 0.6f;
            etoiles[i].taille     = 1;
        } else {
            etoiles[i].vitesse    = 3.0f;   // proches
            etoiles[i].luminosite = 1.0f;
            etoiles[i].taille     = 2;
        }
    }
}
 
void maj_background(void) {
    for (int i = 0; i < NB_ETOILES; i++) {
        etoiles[i].x -= etoiles[i].vitesse;
 
        // Quand l'étoile sort à gauche, elle réapparaît à droite
        if (etoiles[i].x < 0) {
            etoiles[i].x = (float)LARGEUR_FENETRE;
            etoiles[i].y = (float)(rand() % HAUTEUR_FENETRE);
        }
    }
}
 
void dessiner_background(void) {
    // Fond noir espace
    al_clear_to_color(al_map_rgb(0, 0, 10));
 
    for (int i = 0; i < NB_ETOILES; i++) {
        int v = (int)(etoiles[i].luminosite * 255);
        ALLEGRO_COLOR couleur = al_map_rgb(v, v, v);
 
        if (etoiles[i].taille == 1) {
            al_draw_pixel(etoiles[i].x, etoiles[i].y, couleur);
        } else {
            al_draw_filled_rectangle(
                etoiles[i].x, etoiles[i].y,
                etoiles[i].x + 2, etoiles[i].y + 2,
                couleur
            );
        }
    }
}