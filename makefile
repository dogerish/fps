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

OBJ := main.o utils.o rays.o render.o gui.o guipage.o maingui.o map.o
OBJ := $(addprefix $(TDIR)/,$(OBJ))

TARGET=game$(EXT)
$(TARGET): $(TDIR) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@ #-lstdc++fs

$(TDIR): ; mkdir $(TDIR)

$(wordlist 2,$(words $(OBJ)),$(OBJ)): $(TDIR)/%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TDIR)/main.o: main.cpp render.h gui.h guipage.h map.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(TDIR)/guipage.o: gui.h

$(filter-out $(TDIR)/utils.o,$(OBJ)): utils.h
$(addprefix $(TDIR)/,main.o render.o): rays.h
$(addprefix $(TDIR)/,rays.o maingui.o render.o): map.h

.PHONY: redo clean
redo: clean $(TARGET)
clean: ; rm $(OBJ) 2> /dev/null || true
