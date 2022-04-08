# Makefile

CC = gcc -lm

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -O3
LDLIBS = `pkg-config --libs gtk+-3.0 sdl` -lSDL_image

OCR: src/pixel_operations.o src/Display.o src/reseaux.o src/detection_character.o src/SDL_rotozoom.o src/filters.o

clean:
	${RM} *.o
	${RM} *.d

# END
