ifdef CROSS
	CXX:=$(CROSS)-w64-mingw32-g++
	SDL2_CFG:=/usr/local/cross-tools/$(CROSS)-w64-mingw32/bin/sdl2-config
	TDIR:=win-$(CROSS)
	EXT:=-$(CROSS).exe
else
	CXX=g++
	SDL2_CFG=sdl2-config
	TDIR=.
endif

CXXFLAGS := $(shell ${SDL2_CFG} --cflags) --std=c++2a --debug
LIBS := $(shell ${SDL2_CFG} --libs) -lSDL2_ttf -lSDL2_image

OBJ := main.o utils.o rays.o render.o gui.o guipage.o maingui.o map.o game.o
OBJ := $(addprefix $(TDIR)/,$(OBJ))

TARGET=game$(EXT)
$(TARGET): $(TDIR) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@ #-lstdc++fs

$(TDIR): ; mkdir $(TDIR)

$(wordlist 2,$(words $(OBJ)),$(OBJ)): $(TDIR)/%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TDIR)/main.o: main.cpp render.h maingui.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(filter-out $(TDIR)/utils.o,$(OBJ)): utils.h
$(addprefix $(TDIR)/,main.o render.o game.o map.o): rays.h
$(addprefix $(TDIR)/,main.o guipage.o maingui.o): gui.h
$(addprefix $(TDIR)/,main.o maingui.o): guipage.h
$(addprefix $(TDIR)/,main.o rays.o maingui.o render.o game.o): map.h
$(addprefix $(TDIR)/,main.o guipage.o maingui.o): game.h

.PHONY: redo clean
redo: clean $(TARGET)
clean: ; rm $(OBJ) 2> /dev/null || true
