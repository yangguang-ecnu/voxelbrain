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
slices.o \
color.o \
gui.o \
surface.o \
native.o \
misc.o \
io.o \
3dtools.o

VX2_TESTS := surface_unittest.o \
	  io_unittest.o \
	  3dtools_unittest.o \
	  v3tools_unittest.o

#build dependencies using g++
deps: 
	$(CXX) $(CXXFLAGS) *.cpp -MM > main.dep 

#vx2 main build taret.
vx2: $(VX2_CODE) vx2.o 
	$(CXX) $(LINKFLAGS) $(VX2_CODE) vx2.o  -o vx2

#vx2 main build taret.
tests: $(VX2_CODE) $(VX2_TESTS) 
	$(CXX) $(VX2_CODE) $(VX2_TESTS) $(GTEST_STATIC) $(LINKFLAGS) -o tests

tags:
	etags --declarations *.cpp *.h $(GLFW_INCLUDE)/GL/glfw.h

glfwtest: glfwtest.o
	$(CXX) glfwtest.o $(LINKFLAGS) -o glfwtest.exe
	
clean:
	rm *.o vx2

.PHONY: clean tags deps


