ifdef CROSS
	CXX=i686-w64-mingw32-g++
	SDL2_CFG=/usr/local/cross-tools/i686-w64-mingw32/bin/sdl2-config
	TDIR=win32
	EXT=.exe
else
	CXX=g++
	SDL2_CFG=sdl2-config
	TDIR=.
endif
CXXFLAGS := $(shell ${SDL2_CFG} --cflags) --std=c++20 --debug
LIBS := $(shell ${SDL2_CFG} --libs) -lSDL2_ttf

OBJ := main.o utils.o rays.o render.o gui.o
OBJ := $(addprefix $(TDIR)/,$(OBJ))

TARGET=game$(EXT)
$(TARGET): $(TDIR) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@

$(TDIR): ; mkdir $(TDIR)

$(wordlist 2,$(words $(OBJ)),$(OBJ)): $(TDIR)/%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TDIR)/main.o: main.cpp render.h gui.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(filter-out $(TDIR)/utils.o,$(OBJ)): utils.h
$(addprefix $(TDIR)/,main.o render.o): rays.h

.PHONY: redo clean
redo: clean $(TARGET)
clean: ; rm $(OBJ) 2> /dev/null || true
