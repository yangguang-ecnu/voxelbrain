CXXFLAGS := -g -O2 -I../include -I../deps/sparsehash/src -I../deps/glfw/include 
LINKFLAGS := -lGL -lGLU -L../deps/glfw/lib/x11/ -lglfw 
OBJECTS := glbase.o main_aux.o main.o mgz_io.o misc.o nifti.o propagator.o undo.o v3.o v3sets.o volio.o 
all: $(OBJECTS)
	$(CXX) $(LINKFLAGS) $(OBJECTS) -o voxbr
install: all
	mv voxbr ../bin

.PHONY: clean
clean:
	rm $(OBJECTS) voxbr 

tags:
	etags -e *.cpp ../include/*.h

tags_all:
	find ../ -name "*.cpp" -or -name "*.h" -or -name "*.c" | xargs etags -e