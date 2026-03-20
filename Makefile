CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lallegro -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec

SRC = boss.c collision.c ennemi.c jeu.c joueur.c main.c niveau.c ressources.c tir.c
OBJ = $(SRC:.c=.o)
EXE = jeu.exe

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXE)
	./$(EXE)

clean:
	rm -f *.o $(EXE)
