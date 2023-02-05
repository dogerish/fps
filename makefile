SOURCEDIR       = src
INCLUDEDIR      = include
BUILDDIR        = build
RELEASEDIR      = releases
RELEASEINCLUDE  = release-include
OS             ?= $(shell uname -s)
ARCH           ?= $(shell uname -m)
DEBUGGER       ?= lldb
EXECUTABLE      = game
SDL2CFG         = sdl2-config

# cross compilation for windows
ifdef CROSS
	CXX         = $(CROSS)-w64-mingw32-g++
	SDL2CFG     = /usr/local/cross-tools/$(CROSS)-w64-mingw32/bin/sdl2-config
	OS          = windows
	ARCH        = $(CROSS)
	EXECUTABLE := $(EXECUTABLE).exe
endif

OS          := $(shell tr '[:upper:]' '[:lower:]' <<< $(OS))
BUILDDIR    := $(BUILDDIR)/$(OS)/$(ARCH)
EXECUTABLE  := $(BUILDDIR)/$(EXECUTABLE)
RELEASEZIP  := $(RELEASEDIR)/$(OS)_$(ARCH).zip
# release zip depends on all the directories it would include
RELEASEDEPS := $(shell find $(RELEASEINCLUDE)/all \
                            $(RELEASEINCLUDE)/$(OS)/all \
                            $(RELEASEINCLUDE)/$(OS)/$(ARCH) \
                            -type d -not -name '.*' 2>/dev/null)

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

# -M flags for autogenerating makefile dependencies
CXXFLAGS = $(shell $(SDL2CFG) --cflags) -I$(INCLUDEDIR) \
           -MMD -MF $(@:%.o=%.d) -MT $@ \
           --std=c++2a $(if $(value DEBUG),-g,)
LDFLAGS  = $(shell $(SDL2CFG) --libs) -lSDL2_ttf -lSDL2_image

.PHONY: all run release
all run: $(EXECUTABLE)
run:
	$(if $(value DEBUG),$(DEBUGGER),exec) $(EXECUTABLE)
release: $(RELEASEZIP)
unrelease:
	rm -f $(RELEASEZIP)
rerelease: unrelease release

$(RELEASEZIP): $(EXECUTABLE) $(RELEASEDEPS) | $(RELEASEDIR)
	$(eval export RELEASEDIR RELEASEINCLUDE RELEASEZIP CROSS OS ARCH EXECUTABLE)
	./make-release

$(BUILDDIR) $(RELEASEDIR):
	mkdir -p $@

$(EXECUTABLE): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $^ $(LDFLAGS) -o $@ #-lstdc++fs

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJECTS:%.o=%.d)

.PHONY: clean deepclean purge
clean:
	rm -f $(OBJECTS) $(OBJECTS:%.o=%.d)
deepclean:
	rm -rf $(BUILDDIR)
purge:
	rm -rf build/
