// 3D visualization tools and functions for dealing with 3D.


#ifndef __3dtools_h__
#define __3dtools_h__

#include "v3.h"
#include "v3tools.h" //Range
#include "fastvolume.h"
#include "surface.h"
#include "misc.h"
#include <GL/glfw.h>

#ifdef KDL_CYGWIN
#include <GL/glext.h>
#endif
/*
  Simplest interface to OpenGL;
*/

// Visualization code; Should be easily copyable around.
/* Drawable is a thin wrapper around an object to visualize.
 It can assume that 0,0,0 is the center, and everything
should be placed inside a 256 cube.*/

class Drawable {
 public:
  virtual void Draw() = 0;
  virtual void NextFrame(); //next frame happened (More general tool for notifications?)

  int frame_no_;
};

/*
  3D texturing.
*/


class Textured: public Validatable {

 public:
  FastVolume * texturing_fastvolume;

  bool CheckTexture( Range & r );
  const V3f & SetTexture( const V3f & where );


  Textured(); 
  ~Textured();
  Range current_range;

  //bad decision
  bool force_update;

  //export to common Texture
  void * data;
  int size;

};

/*
  The idea is to provide each drawable with its own transform.
  ATM, camera is a standalone class to try out the technique.
  We are going to make it easy to transform it into the universal 
  solution.
 */

class Camera: public Drawable{
 public:
  Camera(Drawable &);
  void Draw ();
  
  Drawable & scene;
};

/* Simple helper functions. */

/*
  Draw unit spheres.
*/


V3f getZ();
V3f getE();
void setupLighting();
int runScene(Drawable &);
V3f Center();


/*
  Conviniences.
 */

const V3f & glVertex3f(const V3f &);
const V3f & glColor3f(const V3f &);
const V3f & glTexCoord3f(const V3f &);
const V3f & SetColor(const V3f &);

/*
  Rays.
  */
  
struct Ray {
	V3f O; //origin 
	V3f D; //direction
	Ray(const V3f &, const V3f &);
	Ray(const Ray &);
    V3f & Travel(float distance, V3f & result);
};

//Intersection information for a given ray.
struct Intersection{
	bool hit;
	float distance;
    Intersection & is(bool, float);
};

Intersection & IntersectRayPlane(const Ray & ray, const Ray & plane, Intersection & result);
Intersection & IntersectRaySphere(const Ray & ray, const V3f & center, float r, Intersection & result);

/// drawing functions:
void DrawSphere(const V3f & where, float radius, int steps = 6, Textured * t = NULL);
void DrawSurface(const Surface & surf);
void DrawPlane(const V3f & center, const V3f & dx, const V3f & dy, int cells);

#endif // __3dtools_h__
