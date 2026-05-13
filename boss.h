#ifndef BOSS_H
#define BOSS_H

typedef struct {
    float x, y;
    float w, h;
    int pv;
    int actif;
    int direction;
} Boss;

void init_boss(Boss *b);
void activer_boss(Boss *b);
void maj_boss(Boss *b);
void dessiner_boss(Boss *b);

#endif
