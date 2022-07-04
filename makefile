SOURCEDIR  = src
SOURCES    = $(wildcard $(SOURCEDIR)/*.cpp)
INCLUDEDIR = include
BUILDDIR   = build
OBJECTS    = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
OS         = native
ARCH       = native
EXECUTABLE = game
SDL2CFG    = sdl2-config

# cross compilation for windows
ifdef CROSS
	CXX     = $(CROSS)-w64-mingw32-g++
	SDL2CFG = /usr/local/cross-tools/$(CROSS)-w64-mingw32/bin/sdl2-config
	OS      = windows
	ARCH    = $(CROSS)
endif

EXECUTABLE   := $(BUILDDIR)/$(OS)/$(ARCH)/$(EXECUTABLE)
EXECUTABLEDIR = $(dir $(EXECUTABLE))

# -M flags for autogenerating makefile dependencies
CXXFLAGS = $(shell $(SDL2CFG) --cflags) -I$(INCLUDEDIR) \
           -MMD -MF $(@:%.o=%.d) -MT $@ \
           --std=c++2a
LDFLAGS  = $(shell $(SDL2CFG) --libs) -lSDL2_ttf -lSDL2_image

.PHONY: all
all: $(EXECUTABLEDIR) $(EXECUTABLE)
run: $(EXECUTABLEDIR) $(EXECUTABLE)
	exec $(EXECUTABLE)

$(EXECUTABLEDIR):
	mkdir -p $(EXECUTABLEDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@ #-lstdc++fs

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJECTS:%.o=%.d)

.PHONY: redo clean deepclean
redo: clean $(EXECUTABLE)
clean:
	rm -f $(OBJECTS) $(OBJECTS:%.o=%.d)
deepclean:
	rm -rf $(BUILDDIR)
