ifdef CROSS
	GCC=i686-w64-mingw32-g++
	SDL2_CFG=/usr/local/cross-tools/i686-w64-mingw32/bin/sdl2-config
	TARGET=game-win32.exe
else
	GCC=g++
	SDL2_CFG=sdl2-config
	TARGET=game
endif
CFLAGS := $(shell ${SDL2_CFG} --cflags) --std=c++20 #--debug
LIBS := $(shell ${SDL2_CFG} --libs) -lSDL2_ttf

OBJ=main.o utils.o rays.o render.o

${TARGET}: $(OBJ)
	$(GCC) $(CFLAGS) $(OBJ) $(LIBS) -o $(TARGET)

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
