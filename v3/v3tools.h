#ifndef __V3TOOLS_H__
#define __V3TOOLS_H__

#include "v3.h"
#include "misc.h"

struct Range{
  V3f min;
  V3f max;

  Range(const V3f &, const V3f &);
  Range();
  Range(const Range &);
};

bool ContainsPoint(const Range &, const V3f &);
bool ContainsRange(const Range &, const Range &);
bool IntersectsRange(const Range &, const Range &);
Range & ExpandRange(Range &, const V3f &);


///Rotate a vector along an axis
V3f rot_x(V3f in, float r);
V3f rot_y(V3f in, float r);
V3f rot_z(V3f in, float r);

void ortoNormalize(V3f &x, V3f &y, V3f &z);

void say(char *, const V3f &);

#endif //__V3TOOLS_H__







