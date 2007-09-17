#ifndef __vector_h__
#define __vector_h__

/*

Basic misc definitions

*/

#include "math.h"

  //common vector definition
  template<class T>
    struct V3 {
      T x; 
      T y; 
      T z;

      V3(){};
      V3(T _x, T _y, T _z):x(_x),y(_y),z(_z){};
      V3(const V3 & v):x(v.x),y(v.y),z(v.z){};
      V3 & operator=(const V3 & v){ x = v.x; y = v.y; z = v.z; return *this;};
      V3 & set(T _x, T _y, T _z){ x = _x; y = _y; z = _z; return *this;};
      V3 & set(double _x, double _y, double _z){ x = (T)_x; y = (T)_y; z = (T)_z; return *this;};

      bool operator==(const V3 & v) const { return (x == v.x && y == v.y && z == v.z);};

      //temporary var is required but more readable;
      V3 operator-(const V3 & v) const { return V3(x-v.x, y-v.y, z-v.z);};
      V3 operator+(const V3 & v) const { return V3(x+v.x, y+v.y, z+v.z);};
      V3 operator*(float m) const { return V3((T)(x*m), (T)(y*m), (T)(z*m));};
      V3 operator/(float m) const { return V3((T)(x/m), (T)(y/m), (T)(z/m));};
 
      //no temporary vars
      V3 & operator-=(const V3 & v){ x-=v.x; y-=v.y, z-= v.z; return *this;};
      V3 & operator+=(const V3 & v){ x+=v.x; y+=v.y, z+= v.z; return *this;};
      V3 & operator*=(float m){ x*=m; y*=m, z*=m; return *this;};
      V3 & operator/=(float m){ x/=m; y/=m, z/=m; return *this;};

      T length2(){ return (x*x+y*y+z*z);};
      float length(){ return sqrtf((float)length2());};
  
      T distance2(const V3 & v) {return (v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z);};
      float distance(const V3 & v) {return sqrtf((float)distance2());};

      //float vector definition
      T & cross(const T & a, const T & b){ x = a.y*b.z-a.z*b.y;
                                           y = a.z*b.x-a.x*b.z;
					   z = a.x*b.y-a.y*b.x;
                                           return *this;};  
      float dot(const V3 & a){ return x*a.x+y*a.y+z*a.z;};

    };

  
  typedef V3<int> V3i; //integer vector
  typedef V3<float> V3f; // float vector

  //transform from integer to float and back
  class Vtr{
  public:
    Vtr(const V3f & _z, const V3f _u):zero(_z),unit(_u){};
    V3i & flip(V3i & out, const V3f & in); 
    V3f & flip(V3f & out, const V3i & in);
  private:
    V3f zero;  //integer (0,0,0)
    V3f unit;  //integer (1,1,1)
  };

#endif


