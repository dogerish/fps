GCC=g++
CFLAGS := $(shell sdl2-config --cflags) --std=c++20 --debug
LIBS := $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_gfx

OBJ=main.o utils.o rays.o render.o

game: $(OBJ)
	$(GCC) $(CFLAGS) $(LIBS) $(OBJ) -o game

main.o: main.cpp utils.h rays.h render.h
	$(GCC) $(CFLAGS) -c main.cpp

utils.o: utils.cpp utils.h
	$(GCC) $(CFLAGS) -c utils.cpp

rays.o: rays.cpp rays.h utils.h
	$(GCC) $(CFLAGS) -c rays.cpp

render.o: render.cpp render.h rays.h utils.h
	$(GCC) $(CFLAGS) -c render.cpp

clean:
	rm $(OBJ)
redo:
	make clean && make
