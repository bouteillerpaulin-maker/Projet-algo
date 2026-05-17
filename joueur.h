#ifndef JOUEUR_H
#define JOUEUR_H

#define DUREE_INVINCIBLE  120   // 2 secondes à 60 FPS
#define DUREE_EXPLOSION   24    // 4 frames × 6 = 24 frames d'explosion

typedef struct {
    float x, y;
    float w, h;
    int vies;
    int vivant;

    // Invincibilité après dégât
    int invincible;       // compteur de frames restantes d'invincibilité
    int clignotement;     // timer pour alterner visible/invisible

    // Explosion au contact
    int en_explosion;
    int explosion_frame;
    int explosion_timer;
} Joueur;

void init_joueur(Joueur *j);
void maj_joueur(Joueur *j, int haut, int bas, int gauche, int droite);
void dessiner_joueur(Joueur *j);
void joueur_prend_degat(Joueur *j);  // à appeler à la place de j->vies--

#endif