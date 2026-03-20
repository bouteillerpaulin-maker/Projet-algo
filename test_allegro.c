#include <stdio.h>
#include <allegro5/allegro.h>

int main(void) {
    printf("Début test Allegro MIN (OpenGL forcé)\n"); fflush(stdout);

    if (!al_init()) {
        printf("al_init FAILED\n");
        return 1;
    }
    printf("al_init OK\n"); fflush(stdout);

    // Forcer OpenGL
    al_set_new_display_flags(ALLEGRO_OPENGL);

    // Forcer une version OpenGL stable
    al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 2, ALLEGRO_REQUIRE);
    al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 1, ALLEGRO_REQUIRE);

    printf("avant al_create_display\n"); fflush(stdout);
    ALLEGRO_DISPLAY *display = al_create_display(800, 600);
    printf("après al_create_display\n"); fflush(stdout);

    if (!display) {
        printf("display NULL\n");
        return 1;
    }

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    al_rest(2.0);

    al_destroy_display(display);
    printf("Fin test Allegro MIN\n"); fflush(stdout);
    return 0;
}
