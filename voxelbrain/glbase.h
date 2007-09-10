/*
  Graphics Objects
*/

#ifndef __globjects_h__
#define __globjects_h__

#include "glbase.h"
#include "v3.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

  
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
	
#endif // __globjects_h__
