// 3D visualization tools.

#ifndef __3dtools_h__
#define __3dtools_h__

#include "v3.h"
#include "fastvolume.h"
#include "misc.h"

/*
  Draw unit sphere.
*/
void drawSphere(int steps, float radius);


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
};

int runScene(Drawable &);



#endif // __3dtools_h__
