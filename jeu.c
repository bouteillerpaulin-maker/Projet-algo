#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "jeu.h"
#include "joueur.h"
#include "tir.h"
#include "ennemi.h"
#include "collision.h"
#include "niveau.h"
#include "boss.h"
#include "background.h"
#include "tourelle.h"

// Variables globales du menu
static int   menu_frame    = 0;   // compteur pour animations menu
static float titre_pulse   = 0.0f;

// Dessine un bouton avec fond semi-transparent et bordure
static void dessiner_bouton(ALLEGRO_FONT *font, const char *texte,
                             float cx, float cy, float w, float h,
                             int selectionne) {
    float x = cx - w / 2.0f;
    float y = cy - h / 2.0f;

    if (selectionne) {
        // Fond cyan semi-transparent
        al_draw_filled_rounded_rectangle(x, y, x + w, y + h, 8, 8,
                                         al_map_rgba(0, 200, 255, 60));
        // Bordure cyan vive
        al_draw_rounded_rectangle(x, y, x + w, y + h, 8, 8,
                                  al_map_rgb(0, 220, 255), 2.5f);
        // Texte blanc vif
        al_draw_text(font, al_map_rgb(255, 255, 255),
                     cx, cy - 4, ALLEGRO_ALIGN_CENTER, texte);

        // Flèche à gauche
        al_draw_text(font, al_map_rgb(0, 220, 255),
                     x - 20, cy - 4, ALLEGRO_ALIGN_RIGHT, ">");
    } else {
        // Fond très sombre
        al_draw_filled_rounded_rectangle(x, y, x + w, y + h, 8, 8,
                                         al_map_rgba(255, 255, 255, 15));
        // Bordure grise 
        al_draw_rounded_rectangle(x, y, x + w, y + h, 8, 8,
                                  al_map_rgba(150, 150, 150, 80), 1.0f);
        // Texte gris
        al_draw_text(font, al_map_rgb(160, 160, 180),
                     cx, cy - 4, ALLEGRO_ALIGN_CENTER, texte);
    }
}

// Menu principal
static void dessiner_menu(ALLEGRO_FONT *font, int selection) {
    // Background étoiles
    dessiner_background();

    float cx = LARGEUR_FENETRE / 2.0f;

    // Ligne décorative haut
    al_draw_line(cx - 400, 220, cx + 400, 220, al_map_rgba(0, 180, 255, 80), 1.0f);

    // Titre avec pulsation
    titre_pulse = sinf(menu_frame * 0.05f);
    float scale = 1.0f + titre_pulse * 0.03f;
    (void)scale; // builtin font ne supporte pas le scaling, on garde l'effet couleur

    // Ombre du titre
    al_draw_text(font, al_map_rgba(0, 100, 200, 120),
                 cx + 3, 133, ALLEGRO_ALIGN_CENTER, "ECE-TYPE");

    // Titre principal - couleur qui pulse entre blanc et cyan
    int pulse_v = (int)(200 + titre_pulse * 55);
    al_draw_text(font, al_map_rgb(pulse_v, pulse_v, 255),
                 cx, 130, ALLEGRO_ALIGN_CENTER, "ECE-TYPE");

    // Sous-titre
    al_draw_text(font, al_map_rgba(100, 180, 255, 180),
                 cx, 175, ALLEGRO_ALIGN_CENTER, "- SHOOT'EM UP -");

    // Ligne décorative bas titre
    al_draw_line(cx - 400, 210, cx + 400, 210, al_map_rgba(0, 180, 255, 80), 1.0f);

    // Boutons 
    const char *items[] = {
        "NOUVELLE PARTIE",
        "NIVEAU 1",
        "NIVEAU 2",
        "BOSS FINAL",
        "QUITTER"
    };

    float btn_w = 350.0f;
    float btn_h = 36.0f;
    float btn_start_y = 320.0f;
    float btn_espacement = 58.0f;

    for (int i = 0; i < 5; i++) {
        dessiner_bouton(font, items[i],
                        cx,
                        btn_start_y + i * btn_espacement,
                        btn_w, btn_h,
                        i == selection);
    }

    // Pied de page
    al_draw_text(font, al_map_rgba(80, 80, 120, 180),
                 cx, HAUTEUR_FENETRE - 40,
                 ALLEGRO_ALIGN_CENTER,
                 "Fleches directionnelles pour naviguer  |  ENTREE pour valider");

    // Petits points décoratifs animés
    for (int i = 0; i < 5; i++) {
        float px = cx - 50 + i * 25.0f;
        float py = btn_start_y + selection * btn_espacement + 50.0f;
        float r  = (i == 2) ? 4.0f : 2.5f;
        ALLEGRO_COLOR pc = (i == 2)
            ? al_map_rgb(0, 220, 255)
            : al_map_rgba(0, 150, 200, 120);
        al_draw_filled_circle(px, py, r, pc);
    }
}

// Game over
static void dessiner_game_over(ALLEGRO_FONT *font) {
    dessiner_background();

    float cx = LARGEUR_FENETRE / 2.0f;
    float cy = HAUTEUR_FENETRE / 2.0f;

    // Rectangle sombre centré
    al_draw_filled_rounded_rectangle(cx - 300, cy - 80, cx + 300, cy + 80,
                                     12, 12, al_map_rgba(0, 0, 0, 180));
    al_draw_rounded_rectangle(cx - 300, cy - 80, cx + 300, cy + 80,
                               12, 12, al_map_rgb(200, 0, 0), 2.0f);

    // Ombre
    al_draw_text(font, al_map_rgba(100, 0, 0, 150),
                 cx + 2, cy - 42, ALLEGRO_ALIGN_CENTER, "GAME OVER");
    // Texte principal
    al_draw_text(font, al_map_rgb(255, 50, 50),
                 cx, cy - 44, ALLEGRO_ALIGN_CENTER, "GAME OVER");

    al_draw_text(font, al_map_rgb(180, 180, 200),
                 cx, cy + 10, ALLEGRO_ALIGN_CENTER,
                 "Appuyez sur ENTREE pour revenir au menu");
}

// Victoire 
static void dessiner_victoire(ALLEGRO_FONT *font) {
    dessiner_background();

    float cx = LARGEUR_FENETRE / 2.0f;
    float cy = HAUTEUR_FENETRE / 2.0f;

    al_draw_filled_rounded_rectangle(cx - 300, cy - 80, cx + 300, cy + 80,
                                     12, 12, al_map_rgba(0, 0, 0, 180));
    al_draw_rounded_rectangle(cx - 300, cy - 80, cx + 300, cy + 80,
                               12, 12, al_map_rgb(0, 200, 100), 2.0f);

    al_draw_text(font, al_map_rgba(0, 80, 40, 150),
                 cx + 2, cy - 42, ALLEGRO_ALIGN_CENTER, "VICTOIRE !");
    al_draw_text(font, al_map_rgb(50, 255, 120),
                 cx, cy - 44, ALLEGRO_ALIGN_CENTER, "VICTOIRE !");

    al_draw_text(font, al_map_rgb(180, 180, 200),
                 cx, cy + 10, ALLEGRO_ALIGN_CENTER,
                 "Appuyez sur ENTREE pour revenir au menu");
}

// Boucle principale
void boucle_jeu(void) {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font = NULL;

    printf("Début boucle_jeu\n"); fflush(stdout);

    if (!al_init()) { printf("ERREUR : al_init\n"); return; }
    if (!al_install_keyboard()) { printf("ERREUR : keyboard\n"); return; }
    if (!al_init_primitives_addon()) { printf("ERREUR : primitives\n"); return; }
    al_init_font_addon();
    if (!al_init_ttf_addon()) { printf("ERREUR : ttf\n"); return; }
    if (!al_init_image_addon()) { printf("ERREUR : image\n"); return; }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 2, ALLEGRO_REQUIRE);
    al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 1, ALLEGRO_REQUIRE);

    display = al_create_display(1920, 1000);
    if (!display) { printf("ERREUR : display\n"); return; }

    timer = al_create_timer(1.0 / FPS);
    if (!timer) { al_destroy_display(display); return; }

    queue = al_create_event_queue();
    if (!queue) { al_destroy_timer(timer); al_destroy_display(display); return; }

    font = al_create_builtin_font();
    if (!font) {
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    EtatJeu etat = ETAT_MENU;
    int selection_menu = 0;

    Joueur joueur;
    Tir tirs[MAX_TIRS];
    TirEnnemi tirs_ennemis[MAX_TIRS_ENNEMIS];
    Ennemi ennemis[MAX_ENNEMIS];
    Niveau niveau;
    Boss boss;
    int nb_tourelles = MAX_TOURELLES;
    int niveau_courant = 1;
    int frame = 0;
    int score = 0;
    Tourelle tourelles[MAX_TOURELLES];
    Missile  missiles[MAX_MISSILES];
    SpritesTourelle sprites_tourelles;

    int touche_haut = 0, touche_bas = 0, touche_gauche = 0, touche_droite = 0;
    int en_pause = 0;

    init_joueur(&joueur);
    init_boss(&boss);
    init_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
    SpritesEnnemi sprites_ennemis = charger_sprites_ennemi();
    init_background();
    sprites_tourelles = charger_sprites_tourelle();
    init_tourelles(tourelles, MAX_TOURELLES);
    init_missiles(missiles, MAX_MISSILES);
    al_start_timer(timer);

    int fin = 0;
    int redraw = 1;

    while (!fin) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fin = 1;
        } else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Le background et le menu s'animent en permanence
            maj_background();
            maj_tourelles(tourelles, MAX_TOURELLES, missiles, MAX_MISSILES, tirs, MAX_TIRS, joueur.x, joueur.y, &score);
            maj_missiles(missiles, MAX_MISSILES, joueur.x, joueur.y);
            menu_frame++;

            if (etat == ETAT_JEU) {
                if (!en_pause) {
                frame++;

                maj_joueur(&joueur, touche_haut, touche_bas, touche_gauche, touche_droite);
                maj_tirs(tirs, MAX_TIRS);
                maj_ennemis(ennemis, MAX_ENNEMIS, joueur.x, joueur.y,
                            tirs, MAX_TIRS, tirs_ennemis, MAX_TIRS_ENNEMIS);
                maj_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);

                script_niveau(&niveau, ennemis, MAX_ENNEMIS,tourelles, nb_tourelles, frame);

                if (niveau.id == 3 && niveau.boss_phase && !boss.actif)
                    activer_boss(&boss);

                maj_boss(
                &boss,
                joueur.y,
                tirs_ennemis,
                MAX_TIRS_ENNEMIS
                );


                collisions_tirs_ennemis(tirs, MAX_TIRS, ennemis, MAX_ENNEMIS, &score);

                if (boss.actif) {
                    for (int i = 0; i < MAX_TIRS; i++) {
                        if (!tirs[i].actif) continue;
                        if (rects_en_collision(tirs[i].x, tirs[i].y, tirs[i].w, tirs[i].h,
                                               boss.x, boss.y, boss.w, boss.h)) {
                            tirs[i].actif = 0;
                            boss.pv--;
                            if (boss.pv <= 0) boss.actif = 0;
                        }
                    }
                }

                collisions_joueur_ennemis(&joueur, ennemis, MAX_ENNEMIS);
                for (int i = 0; i < MAX_MISSILES; i++) {
                    if (!missiles[i].actif) continue;
                    if (rects_en_collision(missiles[i].x, missiles[i].y,
                                           missiles[i].w, missiles[i].h,
                                           joueur.x, joueur.y, joueur.w, joueur.h)) {
                        missiles[i].actif = 0;
                        joueur_prend_degat(&joueur);
                    }
                }

                for (int i = 0; i < MAX_TIRS_ENNEMIS; i++) {
                    if (!tirs_ennemis[i].actif) continue;
                    if (rects_en_collision(tirs_ennemis[i].x, tirs_ennemis[i].y,
                                           tirs_ennemis[i].w, tirs_ennemis[i].h,
                                           joueur.x, joueur.y, joueur.w, joueur.h)) {
                        tirs_ennemis[i].actif = 0;
                        joueur_prend_degat(&joueur);
                    }
                }

                if (boss.actif &&
                    rects_en_collision(joueur.x, joueur.y, joueur.w, joueur.h,
                                       boss.x, boss.y, boss.w, boss.h)) {
                    joueur_prend_degat(&joueur);
                }

                if (!joueur.vivant) {
                    etat = ETAT_GAME_OVER;
                } else if (niveau_termine(&niveau, ennemis, MAX_ENNEMIS, boss.actif)) {
                    if (niveau_courant < NB_NIVEAUX) {
                        niveau_courant++;
                        init_niveau(&niveau, niveau_courant);
                        frame = 0;
                        init_ennemis(ennemis, MAX_ENNEMIS);
                        init_tirs(tirs, MAX_TIRS);
                        init_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
                        init_boss(&boss);
                        init_tourelles(tourelles, MAX_TOURELLES);
                        init_missiles(missiles, MAX_MISSILES);
                        init_joueur(&joueur);
                    } else {
                        etat = ETAT_VICTOIRE;
                    }
                }
                } // fin pause
            }
            

            redraw = 1;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (etat == ETAT_MENU) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        if (selection_menu > 0) selection_menu--;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        if (selection_menu < 4) selection_menu++;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (selection_menu == 0)      niveau_courant = 1;
                        else if (selection_menu == 1) niveau_courant = 1;
                        else if (selection_menu == 2) niveau_courant = 2;
                        else if (selection_menu == 3) niveau_courant = 3;
                        else if (selection_menu == 4) { fin = 1; break; }

                        init_joueur(&joueur);
                        init_tirs(tirs, MAX_TIRS);
                        init_ennemis(ennemis, MAX_ENNEMIS);
                        init_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
                        init_niveau(&niveau, niveau_courant);
                        init_boss(&boss);
                        init_tourelles(tourelles, MAX_TOURELLES);
                        init_missiles(missiles, MAX_MISSILES);
                        frame = 0;
                        score = 0;
                        etat = ETAT_JEU;
                        break;
                }
            } else if (etat == ETAT_JEU) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE: etat = ETAT_MENU; break;
                    case ALLEGRO_KEY_P: en_pause = !en_pause; break;
                    case ALLEGRO_KEY_UP:     touche_haut    = 1; break;
                    case ALLEGRO_KEY_DOWN:   touche_bas     = 1; break;
                    case ALLEGRO_KEY_LEFT:   touche_gauche  = 1; break;
                    case ALLEGRO_KEY_RIGHT:  touche_droite  = 1; break;
                    case ALLEGRO_KEY_SPACE:
                        if (joueur.vivant)
                            tirer(tirs, MAX_TIRS, joueur.x + joueur.w, joueur.y + joueur.h / 2);
                        break;
                }
            } else if (etat == ETAT_GAME_OVER || etat == ETAT_VICTOIRE) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
                    etat = ETAT_MENU;
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (etat == ETAT_JEU) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:    touche_haut   = 0; break;
                    case ALLEGRO_KEY_DOWN:  touche_bas    = 0; break;
                    case ALLEGRO_KEY_LEFT:  touche_gauche = 0; break;
                    case ALLEGRO_KEY_RIGHT: touche_droite = 0; break;
                }
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = 0;

            if (etat == ETAT_MENU) {
                dessiner_menu(font, selection_menu);
            } else if (etat == ETAT_JEU) {
                dessiner_background();
                dessiner_joueur(&joueur);
                dessiner_tirs(tirs, MAX_TIRS);
                dessiner_ennemis(ennemis, MAX_ENNEMIS, &sprites_ennemis);
                dessiner_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
                dessiner_boss(&boss);
                dessiner_tourelles(tourelles, MAX_TOURELLES, &sprites_tourelles);
                dessiner_missiles(missiles, MAX_MISSILES);

                char buf[64];
                sprintf(buf, "Vies: %d  Score: %d  Niveau: %d",
                        joueur.vies, score, niveau_courant);
                al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, buf);
            } else if (etat == ETAT_GAME_OVER) {
                dessiner_game_over(font);
            } else if (etat == ETAT_VICTOIRE) {
                dessiner_victoire(font);
            }

            al_flip_display();
        }
    }

    liberer_sprites_ennemi(&sprites_ennemis);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
