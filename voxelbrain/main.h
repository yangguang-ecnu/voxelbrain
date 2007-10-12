#ifndef __main_h__
#define __main_h__

#include "revision.h"
#ifndef SVN_REVISION //revision was generated successfully
#define SVN_REVISION "Voxel Brain (revision unknown)"
#endif

#include "glbase.h"
#include "v3sets.h"
#include "volio.h"
#include "propagator.h"
#include "misc.h"
#include "undo.h"

///Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

///STL
#include <vector>
#include <algorithm>
using namespace std;

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#ifdef MACOSX
#include <OpenGL/glu.h> //for macosx
#else
#include <GL/glu.h>
#endif

//environment
struct environment {
   bool parse(int argc, char ** argv);
   std::string input_header;  // where to read data from
   std::string input_data; 
   std::string output_header; // where to write data to
   std::string output_data;
   std::string err;
};

class Predicate{
public:
  virtual bool operator()(const V3i &  cur) const = 0;
};

class TestPredicate: public Predicate{
  raw_volume * vol;
public:
  TestPredicate(raw_volume & _vol):vol(&_vol){};
  bool operator()(const V3i & cur) const{
    if((*vol)(cur.x, cur.y, cur.z) >= 1.0){
      if((*vol)(cur.x+1, cur.y, cur.z) < 1.0)return true; //has border
      if((*vol)(cur.x-1, cur.y, cur.z) < 1.0)return true; //has border
      if((*vol)(cur.x, cur.y+1, cur.z) < 1.0)return true; //has border
      if((*vol)(cur.x, cur.y-1, cur.z) < 1.0)return true; //has border
      if((*vol)(cur.x, cur.y, cur.z+1) < 1.0)return true; //has border
      if((*vol)(cur.x, cur.y, cur.z-1) < 1.0)return true; //has border
      return false;
    }else{
      return false;
    };
  };
	
};

struct illumination {
	
	void setup(float K, float F_2){
		if(F_2 > 0.99f)F_2=0.99f; if(F_2 < 0.01f)F_2=0.01f;
		if(K>100.0f)K=100.0f; if(K<1.0f)K=1.0f;
		//K [1..100], F[0..2]
		middle = F_2*2.0f;
		setup_single(K,F_2*2.0f,a_left, b_left);
		setup_single(K,2.0-F_2*2.0f,a_right, b_right);
	};
	
	float eval(float _x){
		float x = 2*_x;

		if(x >=2.0f)return 1.0f;
		if(x <= 0.0f)return 0.0f;
		if(x < middle){
			return (x*x*a_left+x*b_left)/2.0f;
		}else{
			float x_r = (2.0f-x);
			float r = x_r*x_r*a_right+x_r*b_right;
			return (2.0f-r)/2.0f;
		};
	};
	
private:
	float a_left; float a_right; float b_left; float b_right;
	float middle;
	
	void setup_single(float K, float F, float &a, float &b){
		//ax^2+bx; f'(F)=K; f(F)=1
		a = (F*K-1.0f)/F/F;
		b = 2.0/F-K;
	};
};


////HPP

Point calculate_point(raw_volume & vol, V3i & w);
bool is_border(raw_volume & vol, V3i cur);
size_t find_points(raw_volume & vol, point_space & pts);
size_t find_band(raw_volume & vol, point_space & from, point_list & pts, float min, float max);
int erode_band(raw_volume & vol, point_space & pts, point_list & marked, point_list & killed, float from, float to);
V3f GetOGLPos(int x, int y, bool updateZ=true);
V3f GetOGLDirection();
void drawSlice(int w, int h, unsigned char * data);
size_t find_points_predicate(raw_volume & vol, Predicate & fits, point_space & pts);
int FrameTiming();

////HPP

/*****************************************
 *    MAIN MODULE                        *
 * **************************************/

struct main_module {

	//float lastZ;

	
	//structure 
	float rotx, roty, mousex, mousey; //rel coords
	bool do_erosion, do_erosion_level, selection_run; //select if we want undo or propagation

	bool shift_pressed;
	
	V3f point;
	V3i i_point;

	int band[2];
	int coord_updated, band_focus, sparse;

	V3i case_pnt;

	unsigned int background_color;

	illumination ill;
	propagator_t propagator;
	bool do_erode;

	SDL_Surface *screen;
	SDL_Event event;

	const SDL_VideoInfo* info;
	int width, height, bpp, flags;

	Uint32 interval;
	GLuint Plane;

	float threshold, max_val;

	float zoom;
	float POINTSIZE;

	bool do_band;
	float alert_center, alert_width;

	bool disable_lighting, not_hidden, hide_selection,
			update_band_interactively, only_modified, only_points, use_colors,
			proper_normal;
	int application_times, quit;

	enum modes {SEEDS_ADD, SEEDS_REMOVE, SEEDS_NOP};
	modes editing_mode;

	struct SetCollection {
		point_space allPoints;
		point_list allPointsToKill;
		point_list allPointsSelected;
	} sets;

	typedef vector<Point> pnt_sorted;
	typedef vector<Point> Storage;
	Storage points;
	Storage::iterator i;
	pnt_sorted points_to_sort;
	vector<Point> surfels;

	Xs section;

	V3f u;
	Vtr grid;

	undo hard_undo; //undo applied operations;

	environment volenv;
	raw_volume vol;

	//dumping:
	FILE * f;
	FILE * f_colors;
	//axes
	float count;
	
	
	//MSVC SUCKS!!! Cannot put this inside a function.
	struct psortable: public Point{
	  V3f dir;
	  psortable(V3f _dir):dir(_dir){};
	  bool operator()(const Point & a, const Point & b){
	    return a.pos.x*dir.x+a.pos.y*dir.y+a.pos.z*dir.z < b.pos.x*dir.x+b.pos.y*dir.y+b.pos.z*dir.z;
	  };
	  ~psortable(){};
	};
	

	/*****************************************
	 *          METHODS                      *
	 * ***************************************/
    main_module();

    void recalculate_points();
    void select_point(); //add current point to selection
	void apply_modification();  // removes all the points selected during propagation
	void perform_undo(); //if avaliable, undoes selection; else undoes last applied modification
	V3i locate_voxel(); //finds voxel at current mouse position (by scanning)
	int start(int argc, char ** argv);
	int setup_screen();
	void ajust_band();
	
	void predefined_shape_axes();
	void predefined_shape_sphere();
	void begin_frame();
	void render_selection();
	void render_points();
	void crossection_plane(const V3i & dx, const V3i & dy, int  xpos, int  ypos, int color);
	void catch_cursor(const V3i & dx, const V3i & dy, int xcenter, int ycenter, int r);
	void sort_points();
	
	void dump_test_data();
};

#endif // __main_h__
