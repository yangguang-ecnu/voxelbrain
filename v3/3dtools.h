// 3D visualization tools.

#ifndef __3dtools_h__
#define __3dtools_h__

#include "v3.h"
#include "v3tools.h" //Range
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

/*
  3D texturing.
*/

struct Texturizer: Validatable {
  Texturizer(); //Size
  ~Texturizer();
  Range current_range;
  bool force_update;
  void * data;
  int size;
};

//Make sure current texture area is suitable for the desired range
//return false if it is not possible.
bool CheckTexture(Texturizer & t, Range & r);
const V3f & SetTexture(const Texturizer & , const V3f & where);


int runScene(Drawable &);

/*
  Conviniences.
 */

const V3f & SetVertex(const V3f &);
const V3f & SetTexture(const V3f &);
const V3f & SetColor(const V3f &);


#endif // __3dtools_h__
