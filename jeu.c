#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "jeu.h"
#include "joueur.h"
#include "tir.h"
#include "ennemi.h"
#include "collision.h"
#include "niveau.h"
#include "boss.h"
#include "background.h"

static void dessiner_menu(ALLEGRO_FONT *font, int selection) {
    al_clear_to_color(al_map_rgb(0, 0, 40));
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGEUR_FENETRE / 2, 300, ALLEGRO_ALIGN_CENTER, "ECE-TYPE");
    const char *items[] = {
        "Nouvelle partie",
        "Niveau 1",
        "Niveau 2",
        "Boss final",
        "Quitter"
    };
    for (int i = 0; i < 5; i++) {
        ALLEGRO_COLOR c = (i == selection) ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 200);
        al_draw_text(font, c, LARGEUR_FENETRE / 2, 400 + i * 50, ALLEGRO_ALIGN_CENTER, items[i]);
    }
}

static void dessiner_game_over(ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 0, 0), LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 20,
                 ALLEGRO_ALIGN_CENTER, "GAME OVER");
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 + 20,
                 ALLEGRO_ALIGN_CENTER, "Appuyez sur ENTREE pour revenir au menu");
}

static void dessiner_victoire(ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(0, 255, 0), LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 20,
                 ALLEGRO_ALIGN_CENTER, "VICTOIRE !");
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 + 20,
                 ALLEGRO_ALIGN_CENTER, "Appuyez sur ENTREE pour revenir au menu");
}


void boucle_jeu(void) {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font = NULL;

    printf("Début boucle_jeu\n"); fflush(stdout);

    if (!al_init()) {
        printf("ERREUR : al_init a échoué\n");
        return;
    }
    if (!al_install_keyboard()) {
        printf("ERREUR : al_install_keyboard a échoué\n");
        return;
    }
    if (!al_init_primitives_addon()) {
        printf("ERREUR : al_init_primitives_addon a échoué\n");
        return;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        printf("ERREUR : al_init_ttf_addon a échoué\n");
        return;
    }
    if (!al_init_image_addon()) {
        printf("ERREUR : al_init_image_addon a échoué\n");
        return;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 2, ALLEGRO_REQUIRE);
    al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 1, ALLEGRO_REQUIRE);

    display = al_create_display(1920, 1000);
    if (!display) {
        printf("ERREUR : al_create_display a renvoyé NULL\n");
        return;
    }

    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        al_destroy_display(display);
        return;
    }

    queue = al_create_event_queue();
    if (!queue) {
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

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
    int niveau_courant = 1;
    int frame = 0;
    int score = 0;

    int touche_haut = 0, touche_bas = 0, touche_gauche = 0, touche_droite = 0;

    init_boss(&boss);
    init_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);

    SpritesEnnemi sprites_ennemis = charger_sprites_ennemi();

    init_background();
    al_start_timer(timer);

    int fin = 0;
    int redraw = 1;

    while (!fin) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fin = 1;
        } else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (etat == ETAT_JEU) {
                frame++;

                maj_joueur(&joueur, touche_haut, touche_bas, touche_gauche, touche_droite);
                maj_tirs(tirs, MAX_TIRS);
                maj_ennemis(ennemis, MAX_ENNEMIS, joueur.x, joueur.y, tirs_ennemis, MAX_TIRS_ENNEMIS);
                maj_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
                maj_background();

                script_niveau(&niveau, ennemis, MAX_ENNEMIS, frame);

                if (niveau.id == 3 && niveau.boss_phase && !boss.actif) {
                    activer_boss(&boss);
                }

                maj_boss(&boss);

                collisions_tirs_ennemis(tirs, MAX_TIRS, ennemis, MAX_ENNEMIS, &score);

                if (boss.actif) {
                    for (int i = 0; i < MAX_TIRS; i++) {
                        if (!tirs[i].actif) continue;
                        if (rects_en_collision(tirs[i].x, tirs[i].y, tirs[i].w, tirs[i].h,
                                               boss.x, boss.y, boss.w, boss.h)) {
                            tirs[i].actif = 0;
                            boss.pv--;
                            if (boss.pv <= 0)
                                boss.actif = 0;
                        }
                    }
                }

                collisions_joueur_ennemis(&joueur, ennemis, MAX_ENNEMIS);

                for (int i = 0; i < MAX_TIRS_ENNEMIS; i++) {
                    if (!tirs_ennemis[i].actif) continue;
                    if (rects_en_collision(tirs_ennemis[i].x, tirs_ennemis[i].y,
                                           tirs_ennemis[i].w, tirs_ennemis[i].h,
                                           joueur.x, joueur.y, joueur.w, joueur.h)) {
                        tirs_ennemis[i].actif = 0;
                        joueur.vies--;
                        if (joueur.vies <= 0) joueur.vivant = 0;
                    }
                }

                if (boss.actif &&
                    rects_en_collision(joueur.x, joueur.y, joueur.w, joueur.h,
                                       boss.x, boss.y, boss.w, boss.h)) {
                    joueur.vies--;
                    if (joueur.vies <= 0)
                        joueur.vivant = 0;
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
                        init_joueur(&joueur);
                    } else {
                        etat = ETAT_VICTOIRE;
                    }
                }
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
                        if (selection_menu == 0) {
                            niveau_courant = 1;
                        } else if (selection_menu == 1) {
                            niveau_courant = 1;
                        } else if (selection_menu == 2) {
                            niveau_courant = 2;
                        } else if (selection_menu == 3) {
                            niveau_courant = 3;
                        } else if (selection_menu == 4) {
                            fin = 1;
                            break;
                        }
                        init_joueur(&joueur);
                        init_tirs(tirs, MAX_TIRS);
                        init_ennemis(ennemis, MAX_ENNEMIS);
                        init_tirs_ennemis(tirs_ennemis, MAX_TIRS_ENNEMIS);
                        init_niveau(&niveau, niveau_courant);
                        init_boss(&boss);
                        frame = 0;
                        score = 0;
                        etat = ETAT_JEU;
                        break;
                }
            } else if (etat == ETAT_JEU) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        etat = ETAT_MENU;
                        break;
                    case ALLEGRO_KEY_UP:
                        touche_haut = 1; break;
                    case ALLEGRO_KEY_DOWN:
                        touche_bas = 1; break;
                    case ALLEGRO_KEY_LEFT:
                        touche_gauche = 1; break;
                    case ALLEGRO_KEY_RIGHT:
                        touche_droite = 1; break;
                    case ALLEGRO_KEY_SPACE:
                        if (joueur.vivant)
                            tirer(tirs, MAX_TIRS, joueur.x + joueur.w, joueur.y + joueur.h / 2);
                        break;
                }
            } else if (etat == ETAT_GAME_OVER || etat == ETAT_VICTOIRE) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    etat = ETAT_MENU;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (etat == ETAT_JEU) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        touche_haut = 0; break;
                    case ALLEGRO_KEY_DOWN:
                        touche_bas = 0; break;
                    case ALLEGRO_KEY_LEFT:
                        touche_gauche = 0; break;
                    case ALLEGRO_KEY_RIGHT:
                        touche_droite = 0; break;
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

                char buf[64];
                sprintf(buf, "Vies: %d  Score: %d  Niveau: %d", joueur.vies, score, niveau_courant);
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