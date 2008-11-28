-include main.dep

#assemble flags:

INCLUDES := -I$(ATB_INCLUDE) -I$(GLFW_INCLUDE) -I$(GTEST_INCLUDE)
CXXFLAGS := $(CXXFLAGS) -g -O3 $(INCLUDES)
LINKFLAGS := $(ATB_STATIC) $(GLFW_STATIC) $(LINKFLAGS) 


#objects
%.o: ../../src/%.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@  

#list of object modules required for the project
VX2_CODE := navigator.o \
v3tools.o \
fastvolume.o \
loader.o \
gl_points.o \
gl_wrapper.o \
slices.o \
color.o \
gui.o \
surface.o \
native.o \
misc.o \
io.o \
3dtools.o

VX2_TESTS := surface_unittest.o \
	  misc_unittest.o \
	  io_unittest.o \
	  3dtools_unittest.o \
	  fastvolume_unittest.o

#build dependencies using g++
deps: 
	$(CXX) $(CXXFLAGS) *.cpp -MM > main.dep 

#vx2 main build taret.
vx2: $(VX2_CODE) vx2.o 
	$(CXX) $(VX2_CODE) vx2.o $(LINKFLAGS) -o vx2

clean:
	rm *.o vx2

.PHONY: clean tags deps


