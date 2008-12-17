// 3D visualization tools.

#ifndef __3dtools_h__
#define __3dtools_h__

#include "v3.h"
#include "v3tools.h" //Range
#include "fastvolume.h"
#include "misc.h"
#include <GL/glfw.h>
#include <GL/glext.h>

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
  Draw unit spheres.
*/

void DrawSphere(const V3f & where, float radius, int steps = 6, Textured * t = NULL);


int runScene(Drawable &);

/*
  Conviniences.
 */

const V3f & glVertex3f(const V3f &);
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

#endif // __3dtools_h__
