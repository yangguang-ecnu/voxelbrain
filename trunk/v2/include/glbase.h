/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:
 
  Interface for opengl funcitons.

*/

/*
  Graphics Objects
*/

#ifndef __globjects_h__
#define __globjects_h__

#include "glbase.h"
#include "v3.h"

#if HAS_SDL

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#endif

#include <GL/glfw.h>

#include <vector>

  
  class Xs {
  public:
    Xs():pos(0.0f,0.0f,0.0f), size(0.2f), hidden(true){};
    Xs(const V3f & _pos, float _size):pos(_pos), size(_size), hidden(true){};

    void move(V3f _pos){pos=_pos;};
    //
    void draw();
    void show(bool _s){hidden = !_s;};
    bool shown(){return !hidden;};
    
  private:
    V3f pos;
    float size;
    bool hidden;
    void plane(int first, int second, int third, float size, bool closed = false);

  };

  void gen_sphere(const V3f & in, float diameter, int precision);
  
  #endif // __globjects_h__
