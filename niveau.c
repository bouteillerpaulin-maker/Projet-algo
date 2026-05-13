#include <stdlib.h>
#include "niveau.h"
#include "config.h"
#include "ennemi.h"

void init_niveau(Niveau *n, int id) {
    n->id = id;
    n->temps = 0;
    n->fini = 0;
    n->boss_phase = 0;
}

void script_niveau(Niveau *n, Ennemi e[], int nennemis, int frame) {
    n->temps = frame;

    if (n->id == 1) {
        if (frame % 60 == 0 && frame < FPS * 30) {
            spawn_ennemi(e, nennemis, LARGEUR_FENETRE - 50, rand() % (HAUTEUR_FENETRE - 40), 1);
        }
    } else if (n->id == 2) {
        if (frame % 40 == 0 && frame < FPS * 40) {
            spawn_ennemi(e, nennemis, LARGEUR_FENETRE - 50, rand() % (HAUTEUR_FENETRE - 40), 2);
        }
    } else if (n->id == 3) {
        if (frame % 30 == 0 && frame < FPS * 30) {
            spawn_ennemi(e, nennemis, LARGEUR_FENETRE - 50, rand() % (HAUTEUR_FENETRE - 40), 2);
        }
        if (frame > FPS * 30 && !n->boss_phase) {
            n->boss_phase = 1;
        }
    }
}

int niveau_termine(Niveau *n, Ennemi e[], int nennemis, int boss_vivant) {
    if (n->id < 3) {
        if (n->temps > FPS * 35) {
            for (int i = 0; i < nennemis; i++)
                if (e[i].actif) return 0;
            return 1;
        }
        return 0;
    } else {
        if (n->boss_phase && !boss_vivant) return 1;
        return 0;
    }
}
