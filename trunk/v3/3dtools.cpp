#include <stdlib.h>
#include <string.h> //for memset()

#include "3dtools.h"

#ifndef PI
#define PI 3.14159265
#endif

/*
  a: [0..2*PI];
  b: [-PI/2..PI/2].
 */
void vertex_sphere(float a, float b, float r){
  glColor3f(cos(b), cos(b)*cos(a), cos(b)*cos(a+b)); //come up with some colors
  glVertex3f(cos(b)*sin(a)*r, cos(b)*cos(a)*r, sin(b)*r);
};

void drawSphere(int steps, float r){
  V3f cur;
  const float da = PI/steps;
  const float db = PI/steps;
  glBegin(GL_QUADS);
  for(float a = 0; a < 2*PI; a+=da)
    for(float b = -PI/2; b < PI/2; b+=db){
      vertex_sphere(a,b,r);
      vertex_sphere(a,b+db,r);
      vertex_sphere(a+da,b+db,r);
      vertex_sphere(a+da,b,r);
    }; 
  glEnd();
};

/*
  Texturizer.
*/


//making compiler shut up; in vain.
#define BPP 3 
#define SIZE 64
typedef unsigned char * BYTE;

Texturizer::Texturizer(){
  data = malloc(SIZE*SIZE*SIZE*BPP);
  if(!data)valid(false);
};

Texturizer::~Texturizer(){
  if(data) free(data);
  force_update = true;
};

int Offset(int x, int y, int z){
  return BPP*(z*SIZE*SIZE+y*SIZE+x);
};

void UploadTexture(void * data){
  glEnable(GL_TEXTURE_3D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // our texture colors will replace the untextured colors

  // request 1 texture name from OpenGL
  GLuint texname = 1;
  //glGenTextures(1, &texname);
  // tell OpenGL we're going to be setting up the texture name it gave us	
  glBindTexture(GL_TEXTURE_3D, texname);	
  // when this texture needs to be shrunk to fit on small polygons, use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // when this texture needs to be magnified to fit on a big polygon, use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // we want the texture to repeat over the S axis, so if we specify coordinates out of range we still get textured.
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // same as above for T axis
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // same as above for R axis
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  // this is a 3d texture, level 0 (max detail), GL should store it in RGB8 format, its WIDTHxHEIGHTxDEPTH in size, 
  // it doesnt have a border, we're giving it to GL in RGB format as a series of unsigned bytes, and texels is where the texel data is.
  //  memset(data, 100, SIZE*SIZE*SIZE*BPP);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, SIZE, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
};

bool UpdateTexturizer(Texturizer & t, Range & r){
  
  //now, set new current_range:
  V3f center((r.min+r.max)/2);
  V3f half_diagonal(SIZE/2, SIZE/2, SIZE/2);
  t.current_range = Range(center-half_diagonal, center+half_diagonal);
  
  //fill it with a dummy texture for now (maybe better done with an external class
  V3f c(t.current_range.min); //start
  for(int x = 0; x < SIZE; x++)
    for(int y = 0; y < SIZE; y++)
      for(int z = 0; z < SIZE; z++){
	V3f cur(c.x+x, c.y+y, c.z+z);
	bool line_hit = !((int)cur.x % 10) || !((int)cur.y % 10) || !((int)cur.z % 10);
	((BYTE)t.data)[Offset(x,y,z)] = 222;
	((BYTE)t.data)[Offset(x,y,z)+1] = line_hit?100:200;
	((BYTE)t.data)[Offset(x,y,z)+2] =  line_hit?200:100;
      };
  //ready; now load the texture
  UploadTexture(t.data);

  return true;
};

//TODO - implement texture source; dummy for now.
bool CheckTexture(Texturizer & t, Range & r){

  if(t.force_update){ //First time.
    t.force_update = false;
    return UpdateTexturizer(t,r);
  };

  if((r.max.x - r.min.x) > SIZE ||
     (r.max.y - r.min.y) > SIZE ||
     (r.max.z - r.min.z) > SIZE)return false;
  if(ContainsRange(t.current_range, r))return true;
  
  return UpdateTexturizer(t, r);
};


const V3f & SetTexture(const Texturizer & t, const V3f & where){
  V3f pos = (where - t.current_range.min)/SIZE; //calculating the 0..1 range
  SetTexture(pos);
  return where;
};


/*
  Conviniences.
*/

const V3f & SetVertex(const V3f & v){
  glVertex3f(v.x, v.y, v.z); return v;
};

const V3f & SetTexture(const V3f & v){
  glTexCoord3f(v.x, v.y, v.z); return v;
};

const V3f & SetColor(const V3f & v){
  glColor3f(v.x, v.y, v.z); return v;
};


/* 
   Rudimentary scene management
*/


/*
  Projections
*/

int setupProjection(){
  int width, height;
  float zoomf = 1.0f;
  
  glClearColor(1.0,1.0,1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
 
  glMatrixMode(GL_PROJECTION);
  glfwGetWindowSize(&width, &height);
  glViewport(0,0, width, height);
  glLoadIdentity();
  if(width>height){
    glOrtho((-zoomf*(float)width/(float)height), zoomf*((float)width/(float)height), -zoomf*1, zoomf*1, -2, 2);
  }else{
    glOrtho(-zoomf*1, zoomf*1, (-zoomf*(float)height/(float)width), (zoomf*(float)height/(float)width), -2, 2);
  };
};

//assume 0,0,0 is in the center
int setupModelview(){
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(2.0/256.0, 2.0/256.0, 2.0/256.0);
  //  glTranslatef(-128.0, -128.0, -128.0);
  glDisable (GL_BLEND); 
  glDisable(GL_LIGHTING);
};

/*
GLFW callbacks
*/
void GLFWCALL GLFWWindowSizeCb(int,int){
  setupProjection();
};

int GLFWCALL GLFWWindowCloseCb(void){
  return 1;				  
};
void GLFWCALL GLFWWindowRefreshCb(void){
   setupProjection();
};

void GLFWCALL GLFWMouseButtonCb(int,int){
};

void GLFWCALL GLFWMousePosCb(int,int){
};

void GLFWCALL GLFWMouseWheelCb(int){
};

void GLFWCALL GLFWKeyCb(int,int){
};

void GLFWCALL GLFWCharCb(int,int){
};

void setupCallbacks(){
    glfwSetWindowSizeCallback( GLFWWindowSizeCb );
    glfwSetWindowCloseCallback( GLFWWindowCloseCb );
    glfwSetWindowRefreshCallback( GLFWWindowRefreshCb );
    glfwSetKeyCallback( GLFWMouseButtonCb );
    glfwSetCharCallback( GLFWMousePosCb );
    glfwSetMouseButtonCallback( GLFWMouseButtonCb );
    glfwSetMousePosCallback( GLFWMousePosCb );
    glfwSetMouseWheelCallback( GLFWMouseWheelCb );
};

/*
Running a drawable
*/
int runScene(Drawable & scene){
    
    glfwInit();
    if( !glfwOpenWindow( 500, 500, 0,0,0,0, 16,0, GLFW_WINDOW ) )
      {
        glfwTerminate();
      }
    glfwSetWindowTitle( "3D" );
   
    glfwEnable( GLFW_STICKY_KEYS );
    glfwEnable( GLFW_MOUSE_CURSOR );
    glfwDisable( GLFW_AUTO_POLL_EVENTS );
  
    setupCallbacks();

    setupProjection();
    do
      {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	setupModelview();
	
	scene.Draw();
	
       	glfwSwapBuffers();
	glfwWaitEvents();
      }
    while(!glfwGetKey( GLFW_KEY_ESC ) &&
	  glfwGetWindowParam( GLFW_OPENED ));
 

  return true;
};

/*
 */
