#ifndef __GUI_H__
#define __GUI_H__

#ifndef KDL_CYGWIN
#define USE_ATB // AntTweakBar switch
#endif

#ifndef USE_ATB 
typedef int TwBar ;
#define TW_CALL
#define TW_BODY {} //Substitute dummy bodies when ATB is disabled.
#endif 

#include "slices.h"
#include "gl_points.h"

void gui_start(slices *, GlPoints *);
void gui_draw();
void gui_resize(int w, int h);
void gui_stop();

void set_current_file(std::string in);

void read_voxels(std::string in, GlPoints * pnt, bool _half, bool _tru, bool _fill);


#endif // __GUI_H__
