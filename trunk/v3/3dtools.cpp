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

void DrawSphereVertex(float a, float b, const V3f & where, float radius, Textured * t){
  V3f pos(where); pos += V3f(cos(b)*sin(a)*radius, cos(b)*cos(a)*radius, sin(b)*radius);
  if(t){
    t->SetTexture(pos);
  }else{
    glColor3f(cos(b), cos(b)*cos(a), cos(b)*cos(a+b));
  };
    glVertex3f(pos);
};

void DrawSphere(const V3f & where, float radius, int steps, Textured * t){
   glEnable(GL_TEXTURE_3D);
  if(t){ //Ensure we are in the proper place
    V3f diagonal(radius, radius, radius);
    Range range_to_check(where-diagonal, where+diagonal);
    t->CheckTexture(range_to_check);
  };
  V3f cur;
  const float da = PI/steps;
  const float db = PI/steps;
  glBegin(GL_QUADS);
    for(float b = -PI/2; b < PI/2; b+=db)
      for(float a = 0; a < 2*PI; a+=da){
      DrawSphereVertex(a,b,where, radius,t);
      DrawSphereVertex(a,b+db,where, radius,t);
      DrawSphereVertex(a+da,b+db,where, radius,t);
      DrawSphereVertex(a+da,b,where, radius,t);
    }; 
  glEnd();
  glDisable(GL_TEXTURE_3D);

};

//Draw every triangle of the surface
void DrawSurface( const Surface & surf){
  glBegin(GL_TRIANGLES);
  glColor3f(0,1,0);
  for(vector<V3i>::const_iterator i = surf.tri.begin(); i != surf.tri.end(); i++){
    for(int vertex = 0; vertex < 3; vertex++){ //Each verex of a face
      // glColor3f(surf.c[(*i)[vertex]]);
      glVertex3f(surf.v[(*i)[vertex]]-V3f(128,128,128));
    }; //Each vertex of a face
  }; //Each face
  glEnd();
};


/*
  Texturizer.
*/


//making compiler shut up; in vain.
#define BPP 4 
#define SIZE 64
typedef unsigned char * BYTE;

Textured::Textured(){
  texturing_fastvolume = NULL;
  printf("Allocating data.\n");
  data = malloc(SIZE*SIZE*SIZE*BPP);
  if(!data){
    //valid(false);
    printf("Allocation failed.\n");
  }else{
    printf("Data allocated.\n");
  };
};

Textured::~Textured(){
  if(data) free(data);
  force_update = true;
};

int Offset(int x, int y, int z){
  return BPP*(z*SIZE*SIZE+y*SIZE+x);
};

GLuint texname = 0;

#ifndef KDL_CYGWIN
#define HAS_GL_TEX_IMAGE_3D 
#endif

#ifndef HAS_GL_TEX_IMAGE_3D 
PFNGLTEXIMAGE3DPROC glTexImage3D;
#endif //HAS_GL_TEX_IMAGE_3D

void UploadTexture(void * data){

#ifndef HAS_GL_TEX_IMAGE_3D
   glTexImage3D = (PFNGLTEXIMAGE3DPROC) glfwGetProcAddress("glTexImage3D");
	if (glTexImage3D == NULL)
		return;
#endif //HAS_GL_TEX_IMAGE_3D
	
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // our texture colors will replace the untextured colors
  // request 1 texture name from OpenGL
  if(!texname)glGenTextures(1, &texname);
  // tell OpenGL we're going to be setting up the texture name it gave us	
  glBindTexture(GL_TEXTURE_3D, texname);	
  // when this texture needs to be shrunk to fit on small polygons, use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // when this texture needs to be magnified to fit on a big polygon, use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // we want the texture to repeat over the S axis, so if we specify coordinates out of range we still get textured.
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // same as above for T axis
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // same as above for R axis
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  // this is a 3d texture, level 0 (max detail), GL should store it in RGB8 format, its WIDTHxHEIGHTxDEPTH in size, 
  // it doesnt have a border, we're giving it to GL in RGB format as a series of unsigned bytes, and texels is where the texel data is.
  //  memset(data, 100, SIZE*SIZE*SIZE*BPP);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, SIZE, SIZE, SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
};

bool UpdateTextured(Textured & t, Range & r){
  
  //now, set new current_range:
  V3f center((r.min+r.max)/2);
  center.x = floorf(center.x);
  center.y = floorf(center.y);
  center.z = floorf(center.z);

  V3f half_diagonal(SIZE/2, SIZE/2, SIZE/2);
  t.current_range = Range(center-half_diagonal, center+half_diagonal);
  
  //fill it with a dummy texture for now (maybe better done with an external class
  Range fast_volume_range(V3f(0,0,0), V3f(255,255,255));

  unsigned char res;

  V3f c(t.current_range.min); //start
  for(int x = 0; x < SIZE; x++)
    for(int y = 0; y < SIZE; y++)
      for(int z = 0; z < SIZE; z++){
	V3f cur(c.x+x+128, c.y+y+128, c.z+z+128);
	if(t.texturing_fastvolume){
	  //	  if( cur.x > 0 && cur.y > 0 && cur.z > 0 &&
	  //     cur.x < 255 && cur.y < 255 && cur.z < 255 ){
	  res = (unsigned char)t.texturing_fastvolume->Sample((int)cur.x%255, (int)cur.y%255, (int)cur.z%255);
	    //}else{
	    //  res = 0;
	    //};
	  ((BYTE)t.data)[Offset(x,y,z)] = res;
	  ((BYTE)t.data)[Offset(x,y,z)+1] = res;
	  ((BYTE)t.data)[Offset(x,y,z)+2] =  res;
	  ((BYTE)t.data)[Offset(x,y,z)+3] =  res > 5?250:0;
	   
	}else{
	  bool line_hit = !((int)cur.x % 10) || !((int)cur.y % 10) || !((int)cur.z % 10);
	  ((BYTE)t.data)[Offset(x,y,z)] = line_hit?30:300;
	  ((BYTE)t.data)[Offset(x,y,z)+1] = line_hit?70:0;
	  ((BYTE)t.data)[Offset(x,y,z)+2] =  line_hit?300:30;
	  ((BYTE)t.data)[Offset(x,y,z)+3] =  200;
	};
      };
  //ready; now load the texture
  UploadTexture(t.data);

  return true;
};

//TODO - implement texture source; dummy for now.
bool Textured::CheckTexture(Range & r){

  if(force_update){ //First time.
    force_update = false;
    return UpdateTextured( *this, r);
  };

  if((r.max.x - r.min.x) > SIZE ||
     (r.max.y - r.min.y) > SIZE ||
     (r.max.z - r.min.z) > SIZE)return false;
  if(ContainsRange(current_range, r)){
    return true;
  };
  
  return UpdateTextured( *this, r);
};


const V3f & Textured::SetTexture(const V3f & where){
  V3f pos = (where - current_range.min)/SIZE; //calculating the 0..1 range
  glTexCoord3f(pos);
  return where;
};


/*
  Conviniences.
*/

const V3f & glColor3f(const V3f & v){
  glColor3f(v.x, v.y, v.z); return v;
};

const V3f & glVertex3f(const V3f & v){
  glVertex3f(v.x, v.y, v.z); return v;
};

const V3f & glTexCoord3f(const V3f & v){
  glTexCoord3f(v.x, v.y, v.z); return v;
};

const V3f & SetColor(const V3f & v){
  glColor3f(v.x, v.y, v.z); return v;
};


/* 
   Rudimentary scene management
*/

void Drawable::NextFrame(){
  frame_no_++;
};

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
	
	scene.NextFrame();
	scene.Draw();
	
       	glfwSwapBuffers();
	glfwWaitEvents();
      }
    while(!glfwGetKey( GLFW_KEY_ESC ) &&
	  glfwGetWindowParam( GLFW_OPENED ));
 

  return true;
};

/*
Intersections.
 */

Ray::Ray(const V3f & from, const V3f & direction): O(from), D(direction){};
Ray::Ray(const Ray & ray): O(ray.O), D(ray.D){};

/// Extend ray to specified length.
V3f & Ray::Travel(float distance, V3f & result){
	result = D; result *= distance; result += O;
	return result;
};

//Convinience for setting intersection
Intersection & Intersection::is(bool hit_, float distance_){
   hit = hit_; distance = distance_;
   return *this;
};

Intersection & IntersectRayPlane(const Ray & ray, const Ray & plane, Intersection & result){
  //move origin of plane to 0.

  V3f dist(plane.O-ray.O);
  float need_to_travel = dist.dot(plane.D); //along plane normal
  float speed = ray.D.dot(plane.D); //speed of approaching the plane
  if(speed < 0.001)return result.is(false, 0);
  result.is(true, dist.dot(plane.D) / ray.D.dot(plane.D));
};


// (ray.start+ray.direction*t - center).length() = radius
Intersection & IntersectRaySphere(const Ray & ray, const V3f & center, float r, Intersection & result){
  //move origin of sphere to 0.
  Ray cur(ray);
  cur.O -= center;
  //  (cur.O.x + cur.d.x*t)^2 = cur.O.x*cur.O.x + 2*cur.D.x*cur.O.x*t+cur.D.x*cur.D.x*t*t
  //total length:
  //  cur.O.x*cur.O.x + 2*cur.D.x*cur.O.x*t+cur.D.x*cur.D.x*t*t+
  //  cur.O.y*cur.O.y + 2*cur.D.y*t+cur.D.y*cur.D.y*t*t+
  //  cur.O.z*cur.O.z + 2*cur.D.z*t+cur.D.z*cur.D.z*t*t
  //has to be equal to r*r
  //  cur.O.length2() + 2*sum(cur.D)*t+cur.D.length2()*t*t = r*r
  //  cur.D.length2()*t*t+2*(cur.D.dot(cur.O))*t+cur.O.length2()-r*r = 0;
  float a = cur.D.length2();
  float b = 2*cur.D.dot(cur.O);
  float c = cur.O.length2()-r*r;
  float discriminant = b*b-4*a*c;
  printf("a:%f b:%f c:%f; hence discriminant:%f\n", a, b, c, discriminant);
  if(discriminant < 0)return result.is(false, 0); 
  //interested in positive t's
  float discriminant_root = sqrtf(discriminant);
  float t1 = (-b + discriminant_root)/(2*a);
  float t2 = (-b - discriminant_root)/(2*a);
  return result.is(true, (t1 < t2)?t1:t2);
};
