#include <stdlib.h>
#include "niveau.h"
#include "config.h"
#include "ennemi.h"
#include "tourelle.h"

void init_niveau(Niveau *n, int id) {
    n->id         = id;
    n->temps      = 0;
    n->fini       = 0;
    n->boss_phase = 0;
}

void script_niveau(Niveau *n, Ennemi e[], int nennemis,
                   Tourelle t[], int ntourelles, int frame) {
    n->temps = frame;

    // y ignoré : spawn_tourelle force le sol automatiquement
    float y_sol = 0;

    if (n->id == 1) {
        // Ennemis volants : toutes les 90 frames (était 60 → moins de spam)
        if (frame % 90 == 0 && frame < FPS * 30)
            spawn_ennemi(e, nennemis,
                         LARGEUR_FENETRE - 50,
                         50 + rand() % (HAUTEUR_FENETRE / 2), 1);

        // Tourelles : 1 toutes les 600 frames (~10s) max 2 au niveau 1
        if (frame % 600 == 0 && frame > 0 && frame < FPS * 25)
            spawn_tourelle(t, ntourelles,
                           LARGEUR_FENETRE - 50, y_sol, 2);

    } else if (n->id == 2) {
        if (frame % 70 == 0 && frame < FPS * 40)
            spawn_ennemi(e, nennemis,
                         LARGEUR_FENETRE - 50,
                         50 + rand() % (HAUTEUR_FENETRE / 2), 2);

        // Tourelles : toutes les 450 frames (~7.5s) au niveau 2
        if (frame % 450 == 0 && frame > 0 && frame < FPS * 35)
            spawn_tourelle(t, ntourelles,
                           LARGEUR_FENETRE - 50, y_sol, 3);

    } else if (n->id == 3) {
        if (frame % 50 == 0 && frame < FPS * 30)
            spawn_ennemi(e, nennemis,
                         LARGEUR_FENETRE - 50,
                         50 + rand() % (HAUTEUR_FENETRE / 2), 2);

        // Tourelles : toutes les 360 frames (~6s) au niveau 3
        if (frame % 360 == 0 && frame > 0 && frame < FPS * 28)
            spawn_tourelle(t, ntourelles,
                           LARGEUR_FENETRE - 50, y_sol, 4);

        if (frame > FPS * 30 && !n->boss_phase)
            n->boss_phase = 1;
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