#include "3dtools.h"

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
