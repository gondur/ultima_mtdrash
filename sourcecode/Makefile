# --------------------------------------------------------
# ---- Makefile with stlport, sdl and openGL          ----
# ----             For Mingw32                        ----
# --------------------------------------------------------

# Name of the built project
OUTPUTNAME = mtdrash.exe

# --- All sources ----------------------------------------
SOURCES += src/main.cpp
SOURCES += src/game.cpp
SOURCES += src/math/primitives.cpp
SOURCES += src/utility/Logger.cpp
SOURCES += src/utility/FileIOObject.cpp
SOURCES += src/utility/os.cpp
SOURCES += src/vic20/vic20font.cpp
SOURCES += src/vic20/vic20screen.cpp
SOURCES += src/sdlUtil.cpp



# --------------------------------------------
# ---- Only change below for path changes ----
# --------------------------------------------

# path to SDL include
MINGW_INCLUDE = -Ic:/mingw/include/
SDL_INCLUDE = -Ic:/mingw/include/SDL/ 

# --------------------------------------------
# ---- No need to change below this       ----
# --------------------------------------------

# library configure stuff
SDL_LIB = -lSDLmain -lSDL -lSDL_mixer
WIN32_LIB = -lmingw32 -lkernel32 -luser32
OPENGL_LIB = -lopengl32 -lglu32

# includes and libs

#debug flags
#CPPFLAGS = -g $(SDL_INCLUDE) $(MINGW_INCLUDE) -I./src 
#LINKFLAGS = $(WIN32_LIB) -lwinmm $(OPENGL_LIB) $(SDL_LIB) 

#release flags
CPPFLAGS = -O3 $(SDL_INCLUDE) $(MINGW_INCLUDE) -I./src 
LINKFLAGS = $(WIN32_LIB) -lwinmm $(OPENGL_LIB) $(SDL_LIB) 


.PHONY: default
default: all

# Pattern rule for generating .d (depend) files
%.d: %.cpp
	@echo Generating $@ from source ; \
	set -e; $(CPP) $(GENERATE_D_FLAG) $(CPPFLAGS) $< \
	| sed 's/\($(*F)\)\.o[ ]*:/$(subst /,\/,$*)\.o $(subst /,\/,$@) : /g' > '$@'; \
	[ -s $@ ] || rm -f $@

# Pattern rule for compiling .cpp files
%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@


# Compiler setup
CPP = g++
CPPFLAGS += -Wall -Wno-sign-compare
LINKFLAGS += 
GENERATE_D_FLAG = -MM


OBJECTS = $(SOURCES:.cpp=.o) 
DEPENDS = $(SOURCES:.cpp=.d) 

# Include depend files only if goal isn't clean
ifneq ($(findstring clean, $(MAKECMDGOALS)),clean)
  -include $(DEPENDS)
endif

# target all: default target
all: $(OBJECTS) $(DEPENDS) Makefile
	@echo -------------------------------------------------------------------------------;\
	echo Linking with flags: $(LINKFLAGS); \
	$(CPP) -o $(OUTPUTNAME) $(OBJECTS) $(LINKFLAGS) 


# target clean
.PHONY: clean
clean: 
	@-find ./src -name "*.o" -exec rm '{}' ';'
	@-find ./src -name "*.d" -exec rm '{}' ';'
	@-rm $(OUTPUTNAME)

