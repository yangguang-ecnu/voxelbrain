/*
For reference only.
*/

#include <GL/glfw.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "3dtex.h"
#include "loader.h"
#include "fastvolume.h"
#include "io.h"


typedef unsigned char BYTE;

unsigned int fpsCurrent = 0;
unsigned int fpsCount = 0;

unsigned long tick;
//PFNGLTEXIMAGE3DPROC glTexImage3D;
int windownumber;

unsigned int texname;
unsigned int angle;
unsigned char *tex = NULL;

// these define a square in the X-Y plane clockwise order from the lower left,
GLdouble verts[4][3] = { { -1.0, -1.0, 0.0}, {-1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, -1.0, 0.0} };
GLdouble centervert[3] = { 0.0, 0.0, 0.0 };
int display_list;
void build_display_list();
int * indices;
float * vertices;
float * colors;


int main(int argc, char **argv) {
  glutInit(&argc, argv);
	
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT);
  glutInitWindowPosition(80, 80);
  windownumber = glutCreateWindow (APP_TITLE);
	
  glutKeyboardFunc (keyboard);
  glutDisplayFunc (display);
  glutReshapeFunc (reshape); 
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
	
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
  // VERY IMPORTANT:
  // this line loads the address of the glTexImage3D function into the function pointer of the same name.
  // glTexImage3D is not implemented in the standard GL libraries and must be loaded dynamically at run time,
  // the environment the program is being run in MAY OR MAY NOT support it, if not we'll get back a NULL pointer.
  // this is necessary to use any OpenGL function declared in the glext.h header file
  // the Pointer to FunctioN ... PROC types are declared in the same header file with a type appropriate to the function name
  //Not needed as we have it in the standard library.	
  //glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
  if (glTexImage3D == NULL) {
    printf("Error in line %d: Couldn't load glTexImage3D function. Aborting.\n", __LINE__);
    return -1;
  }

	
  tex = build_texture();
  build_display_list();
	
  glInit (WIDTH, HEIGHT);
  glutTimerFunc(1000, countFPS, 1);
  glutIdleFunc(idle);
  glutMainLoop ();
	
  return 0;
}

void keyboard(unsigned char key, int x, int y) {
  switch ( key ) {
  case ESCAPE:
    printf("Escape key pressed. exit.\n");
    exit(0);
  default:
    printf("Key %d corresponding to char '%c' is not assigned to any action.\n", key, key);
    break;
  }
}

void mouse(int button, int state, int x, int y){
  tick = y;
  glutPostRedisplay();
};

void motion( int x, int y){
  tick = y;
  angle = x;
  glutPostRedisplay();
};

int len;
float dx;

void update_arrays(float tt){
  for(int i = -len; i < len; i++)
    for(int j = -len; j < len; j++){

	    float dist = (float)((len*len-j*j)+(len*len-i*i))/(float)(len*len*4);
            dist = 0.2;
      //arrays
      int pos = ((i+len)+(j+len)*2*len);
      //vertices[3*pos] = dx*i;
     // vertices[3*pos+1] = dx*j;
      vertices[3*pos+2] = dist*(sin(tt+(float)i/10.0f)+cos(tt+(float)j/10.0f));
      //colors[3*pos] = 0.7;
      colors[3*pos+1] = 0.5;
      colors[3*pos+2] = 0.2*(sin(tt+(float)i/10.0f)+cos(tt+(float)j/10.0f));
      //if(i < (len-1) && j < (len-1)){
      // indices[cur] = (i+len)+(j+len)*2*len;
      // indices[cur+1] = (i+len)+(j+1+len)*2*len;
      // indices[cur+2] = (i+1+len)+(j+1+len)*2*len;
      // indices[cur+3] = (i+1+len)+(j+len)*2*len;
      // cur += 4;
      //};
    };

};

void build_display_list(){

  len = 100;
  const float dx = 1.0 / len;

  //arrays
  indices = new int[len*len*4 * 4];
  vertices = new float[len*len*4*4];
  colors =  new float[len*len*4*4];
  

  //lists
  glNewList(1, GL_COMPILE);
  glBegin(GL_QUADS);
  int cur = 0;
  for(int i = -len; i < len; i++)
    for(int j = -len; j < len; j++){

      //arrays
      int pos = ((i+len)+(j+len)*2*len);
      vertices[3*pos] = dx*i;
      vertices[3*pos+1] = dx*j;
      vertices[3*pos+2] = 1+sin(dx*i+dx*j);
      colors[3*pos] = 0.7;
      colors[3*pos+1] = 0.5;
      colors[3*pos+2] = 0.5+0.5*sin(dx*i+dx*j);
      if(i < (len-1) && j < (len-1)){
       indices[cur] = (i+len)+(j+len)*2*len;
       indices[cur+1] = (i+len)+(j+1+len)*2*len;
       indices[cur+2] = (i+1+len)+(j+1+len)*2*len;
       indices[cur+3] = (i+1+len)+(j+len)*2*len;
       cur += 4;
      };

      //list
      glColor3f(0.7,0,0.5+0.5*sin(dx*i+dx*j));
      glVertex3f(dx*i,dx*j, 1+sin(dx*i+dx*j));
      glColor3f(0.7,0,0.5+sin(dx*i+dx*j+dx));
      glVertex3f(dx*i,dx*j+dx,1+sin(dx*i+dx*j+dx));
      glColor3f(0.7,0,0.5+0.5*sin(dx*i+dx*j+2*dx));
      glVertex3f(dx*i+dx,dx*j+dx,1+sin(dx*i+dx*j+dx+dx));
      glColor3f(0.7,0,0.5+0.5*sin(dx*i+dx*j+dx));
      glVertex3f(dx*i+dx,dx*j,1+sin(dx*i+dx*j+dx));
    };
  glEnd();
  glEndList();

};

void draw_surface(){
//	glLineWidth(10);
  //glEnable(GL_COLOR_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_INDEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);  
  //glIndexPointer(GL_INT, 0, (GLvoid *)indices_t);
  glVertexPointer(3, GL_FLOAT, 0, (GLvoid *)vertices);
  glColorPointer(3, GL_FLOAT, 0, (GLvoid *)colors);
  glDrawElements(GL_QUADS, 4*len*len*4, GL_UNSIGNED_INT, (GLvoid *)indices);
  //glDrawElements(GL_QUADS, len*len, GL_SHORT, vertices);
  glDisableClientState(GL_VERTEX_ARRAY);
  //glDisableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);  
 
};


struct xyz{

};
void sphere_rasterizer(int x, int y, int maxx, int maxy){
  
};

void draw_cone(float displace){
  float x, float y;
	 //draw first time
k = 0;

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);

  glEnable(GL_TEXTURE_3D);
  glBegin(GL_TRIANGLES);
  // texture coordinates are always specified before the vertex they apply to.
  for (x = 0; x <= 4; x++) {
    glTexCoord3d(0.5+centervert[0]/2.0, 0.5+centervert[1]/2.0, centervert[2]/2.0);
    //glTexCoord3d(centervert[0], centervert[1], 2.0);			// texture stretches rather than glides over the surface with this
    glVertex3d(centervert[0], centervert[1], centervert[2]+k);

    glTexCoord3d(0.5+verts[x][0]/2.0, 0.5+verts[x][1]/2.0, 0.5+verts[x][2]/2.0);
    glVertex3d(verts[x][0], verts[x][1], k+verts[x][2]);

    glTexCoord3d(0.5+verts[(x+1)%4][0]/2.0, 0.5+verts[(x+1)%4][1]/2.0, 0.5+verts[(x+1)%4][2]/2.0);
    glVertex3d(verts[(x+1)%4][0], verts[(x+1)%4][1], k+verts[(x+1)%4][2]);
  }
  glEnd();
  glPolygonOffset(0,0);


  // we don't want the lines and points textured, so disable 3d texturing for a bit
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_POLYGON_OFFSET_FILL);
  // glDepthFunc(GL_EQUAL);


};

void display() {

  
  // tell GL we want it to use this texture when it textures our polygons
  // (this function has multiple uses, see BuildTexture())
  glBindTexture(GL_TEXTURE_3D, texname);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glLoadIdentity();
  // move to somewhere we can see
  glTranslated(0.0, -1.0, -2.4);	
  // spin the pyramid to a good orientation to see what happens to the 3d texturing when the vertex moves
  glRotated(-90, 1.0, 0.0, 0.0);
  glRotated((double)angle / 5.0, 0.0, 0.0, 1.0);
  centervert[2] = sin((double)tick / 70.0) + 1.0;

  //  glDepthFunc(GL_LESS);
  //draw_cone(-0.4);
 // glEnable(GL_STENCIL_TEST);
 // glStencilFunc();
 // glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

  //glDepthFunc(GL_GREATER);
  //glDepthMask(GL_FALSE); //don't write anything to depth buffer
 // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glEnable(GL_COLOR);
  draw_surface(); 
  draw_cone(0);

//  draw_surface(); 
#endif // lists or arrays
#endif //immediate or retained
  fpsCurrent++;
	
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  glViewport(0, 0, w, h); // Reset The Current Viewport And Perspective Transformation
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2,2,-2,2,1,6);
 
  glMatrixMode(GL_MODELVIEW);
}

float tt;
void glInit (int w, int h) {
tt = 0.0;	
  // set up our OpenGL state
  glEnable(GL_TEXTURE_3D);	// enable 3d texturing
  glEnable(GL_DEPTH_TEST);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // our texture colors will replace the untextured colors
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor4d(0.0, 0.0, 0.0, 1.0);
  glPointSize(3.0);
	
  glShadeModel(GL_FLAT);
	
  glViewport (0, 0, (GLsizei) SCREEN_WIDTH, (GLsizei) SCREEN_HEIGHT);
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10,10,-10,10,-1,1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
	
  fpsCount = 0;
  fpsCurrent = 0;

}

void idle() {
  // tick += 10;
  update_arrays(tt);
  tt += 0.01;  
  glutPostRedisplay();
}

void countFPS(int value) {
  char title[120];
  fpsCount = fpsCurrent;
  fpsCurrent = 0;
	
  snprintf(title, 120, "%s. FPS: %d", APP_TITLE, fpsCount);
  glutSetWindowTitle(title);
  glutTimerFunc(1000, countFPS, 1);
}

unsigned char*build_texture(void)
{

  // ask for enough memory for the texels and make sure we got it before proceeding
  unsigned char *texels = (BYTE *)malloc(WIDTH * HEIGHT * DEPTH * BYTES_PER_TEXEL);
  if (texels == NULL)
    return NULL;
  int s, t;

  // each of the following loops defines one layer of our 3d texture, there are 3 unsigned bytes (red, green, blue) for each texel so each iteration sets 3 bytes
  // the memory pointed to by texels is technically a single dimension (C++ won't allow more than one dimension to be of variable length), the 
  // work around is to use a mapping function like the one above that maps the 3 coordinates onto one dimension
  // layer 0 occupies the first (width * height * bytes per texel) bytes, followed by layer 1, etc...
  FastVolume volume;
  Loader loader;
  loader.read("brainmask.mgz");
  loader.read_volume(volume);

	
  int a,b,c;
  float PI = 3.14159;
  for ( a = 0; a < WIDTH; a++)
    for (b = 0; b < HEIGHT; b++)
      for(c = 0; c < DEPTH; c++){
	//float stripe = (float)(a) / 16;
	texels[TEXEL3(a,b,c)] = volume.Sample(70+a,70+b,70+c);//(BYTE)(100*(1+sin(stripe*2*PI)));;
	texels[TEXEL3(a,b,c)+1] = volume.Sample(70+a,70+b,70+c);//(BYTE)(100*(1+sin(stripe*2*PI)));;
	texels[TEXEL3(a,b,c)+2] = volume.Sample(70+a,70+b,70+c);//(BYTE)(99*(1+sin(stripe*2*PI)));;
      };
  /*
  // define layer 0 as red
  for (s = 0; s < WIDTH; s++) {
  for (t = 0; t < HEIGHT; t++) {
  texels[TEXEL3(s, t, 0)] = 0x80;
  texels[TEXEL3(s, t, 0)+1] = 0x00;
  texels[TEXEL3(s, t, 0)+2] = 0x00;
  }
  }
  // define layer 1 as green
  for (s = 0; s < WIDTH; s++) {
  for (t = 0; t < HEIGHT; t++) {
  texels[TEXEL3(s, t, 1)] = 0x00;
  texels[TEXEL3(s, t, 1)+1] = 0x80;
  texels[TEXEL3(s, t, 1)+2] = 0x00;
  }
  }
  // define layer 2 as blue
  for (s = 0; s < WIDTH; s++) {
  for (t = 0; t < HEIGHT; t++) {
  texels[TEXEL3(s, t, 2)] = 0x00;
  texels[TEXEL3(s, t, 2)+1] = 0x00;
  texels[TEXEL3(s, t, 2)+2] = 0x80;
  }
  }

  // define layer 3 as grey
  for (s = 0; s < WIDTH; s++) {
  for (t = 0; t < HEIGHT; t++) {
  texels[TEXEL3(s, t, 3)] = 0x80;
  texels[TEXEL3(s, t, 3)+1] = 0x80;
  texels[TEXEL3(s, t, 3)+2] = 0x80;
  }
  }
  */
  // request 1 texture name from OpenGL
  glGenTextures(1, &texname);	
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
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, WIDTH, HEIGHT, DEPTH, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);

  return texels;
}
