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

#define POINTSIZE 3.5f

int FrameTiming();


//Propagator gen; //main propagat


//point storage
struct SetCollection {
  point_space allPoints;
  point_list allPointsToKill;
  point_list allPointsSelected;
} sets;
//point_list toModify;




bool do_erode;

//calculate normals and stuff
Point calculate_point(raw_volume & vol, V3i & w){
  Point tmp;
  tmp.pos=w;
  tmp.norm.set(0.0f,0.0f,0.0f);
  V3i cur;
  float max = 0;
  for(cur.x = -2; cur.x <= 2; cur.x++) //loop over
    for(cur.y = -2; cur.y <= 2; cur.y++)
      for(cur.z = -2; cur.z <= 2; cur.z++)
	if((cur.length2() < 2*2) && (1.0<vol(w.x+cur.x, w.y+cur.y, w.z+cur.z))){ //within a sphere
	  tmp.norm+=V3f((float)cur.x,(float)cur.y,(float)cur.z);
	  if(vol(w.x+cur.x, w.y+cur.y, w.z+cur.z) > max)max = vol(w.x+cur.x, w.y+cur.y, w.z+cur.z);
	};
  tmp.norm /= -tmp.norm.length();
  max = vol(w);
  tmp.col.set(max, max, max);
  tmp.col /= 400.0;
  //todo calculate volume;
	
  return tmp;
};

inline bool is_border(raw_volume & vol, V3i cur){
  if(vol(cur.x, cur.y, cur.z) >= 1.0){
    if(vol(cur.x+1, cur.y, cur.z) < 1.0)return true; //has border
    if(vol(cur.x-1, cur.y, cur.z) < 1.0)return true; //has border
    if(vol(cur.x, cur.y+1, cur.z) < 1.0)return true; //has border
    if(vol(cur.x, cur.y-1, cur.z) < 1.0)return true; //has border
    if(vol(cur.x, cur.y, cur.z+1) < 1.0)return true; //has border
    if(vol(cur.x, cur.y, cur.z-1) < 1.0)return true; //has border
    return false;
  }else{
    return false;
  };
};

size_t find_points(raw_volume & vol, point_space & pts){

  V3i cur;
  int count = 0;
  for(cur.x = 1; cur.x <= vol.dim[0]-1; cur.x++) //loop over
    for(cur.y = 1; cur.y <= vol.dim[1]-1; cur.y++)
      for(cur.z = 1; cur.z <= vol.dim[2]-1; cur.z++)
	if(is_border(vol, cur))
	  pts.insert(pair<unsigned int, Point>(key(cur), calculate_point(vol, cur)));
  return pts.size();
};

size_t find_band(raw_volume & vol, point_space & from, point_list & pts, float min, float max){
  pts.clear();
  for(point_space::iterator i = from.begin(); i!= from.end(); i++){
    if(vol(key(i->first)) <= max && vol(key(i->first)) >= min )pts.insert(i->first);
  }
  return pts.size();
};

	
int erode_band(raw_volume & vol, point_space & pts, point_list & marked, point_list & killed, float from, float to){
  point_list to_search;
  point_list to_propagate;
  //clearing 
  int count = 0;
  for(point_space::iterator iv = pts.begin(); iv !=pts.end(); iv++){
    V3i pnt(key(iv->first));
    float cur = vol(pnt);
    //	 if((cur >= (from-0.5) ) && (cur <= (to+0.5) ) || (marked.find(iv->first) != marked.end())){
    if((marked.find(iv->first) != marked.end())){
      //       vol(iv->first.x, iv->first.y, iv->first.z)=vol(iv->first.x, iv->first.y, iv->first.z); //eroding
      killed.insert(iv->first);
      for(int i = -1; i<=1; i++)//loop around single point
	for(int j = -1; j<=1; j++)
	  for(int k = -1; k<=1; k++){
	    V3i around(pnt.x+i, pnt.y+j, pnt.z+k);
	    int a = vol(around); int b = vol(pnt);
	    int diff = (a>b)?(a-b):(b-a);
			
	    //do read-ahead propagation			
			
	    if(diff < 10) to_propagate.insert(key(around)); //checking only similar stuff
	    to_search.insert(key(around)); //where to look for surface
	    //			if(pts.find(around) != pts.end())*/to_erase.insert(around);
	  };
      if(vol(pnt)>0)vol.set(pnt, (short)-vol(pnt)); //eroding
    }
  }
  //marked.clear();
  //adding marked stuff
  for(point_list::iterator r = to_propagate.begin(); r != to_propagate.end(); r++){
    marked.insert(*r); //we have already inserted interesting stuff
    count++;
  };
     
  //eraze possibly modified points
  for(point_list::iterator r = to_search.begin(); r != to_search.end(); r++){
    pts.erase(*r);
  };
	 
  //updating vicinity of possibly modified area
  for(point_list::iterator r = to_search.begin(); r != to_search.end(); r++){
    V3i cur_pnt;
    if(is_border(vol, key(*r))){
      cur_pnt=key(*r);
      pts.insert(pair<int, Point>(key(cur_pnt), calculate_point(vol, cur_pnt)));
      //simple band	  if((vol(cur_pnt) >= (from-0.5) ) && (vol(cur_pnt) <= (to+0.5) ))marked.insert(key(cur_pnt));
    };
  };

  return count;
};


//project sceen coordinates into model space.

float lastZ = 0;

V3f GetOGLPos(int x, int y, bool updateZ = true)
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetIntegerv( GL_VIEWPORT, viewport );

  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  if(updateZ){
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    lastZ = winZ;
  }else{
    winZ = lastZ;
  };
	
  gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

  return V3f((float)posX, (float)posY, (float)posZ);
}


V3f GetOGLDirection()
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];
  GLdouble posX, posY, posZ;

  V3f a,b;

  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetIntegerv( GL_VIEWPORT, viewport );

  gluUnProject( 0, 0, 0, modelview, projection, viewport, &posX, &posY, &posZ);
  a.set((float)posX, (float)posY, (float)posZ);
  gluUnProject( 0, 0, 1, modelview, projection, viewport, &posX, &posY, &posZ);
  b.set((float)posX, (float)posY, (float)posZ);

  return a-b;
}

typedef vector<Point> pnt_sorted;

//draws an array of pixels, rgba, provided in 'data';
void drawSlice(int w, int h, unsigned char * data){
  //  Vector3 c = GetOGLPos(1, 1+h);
  //  glRasterPos3f(c.x, c.y, 0.0);
  glDrawPixels(w,h,GL_RGB, GL_BYTE, (void *)data);  
};

//MSVC SUCKS!!! Cannot put this inside a function.
struct psortable: public Point{
  V3f dir;
  psortable(V3f _dir):dir(_dir){};
  bool operator()(const Point & a, const Point & b){
    return a.pos.x*dir.x+a.pos.y*dir.y+a.pos.z*dir.z < b.pos.x*dir.x+b.pos.y*dir.y+b.pos.z*dir.z;
  };
  ~psortable(){};
};


//testing predicates
	
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

size_t find_points_predicate(raw_volume & vol, Predicate & fits, point_space & pts){

  V3i cur;
  int count = 0;
  for(cur.x = 1; cur.x <= vol.dim[0]-1; cur.x++) //loop over
    for(cur.y = 1; cur.y <= vol.dim[1]-1; cur.y++)
      for(cur.z = 1; cur.z <= vol.dim[2]-1; cur.z++)
	if(fits(cur))
	  pts.insert(pair<unsigned int, Point>(key(cur), calculate_point(vol, cur)));
  return pts.size();
};



/////////////////////////////////////   MAIN   ////////////////////////////


int main(int argc, char **argv)
{

  propagator_t propagator;
	
  do_erode = false;

  SDL_Surface *screen;
  SDL_Event event;
  int application_times = 1;
  int quit = 0;
  bool not_hidden = true;
  bool update_band_interactively = false;
  bool only_modified = false;
  bool only_points = false;
  bool use_colors = true;
  bool proper_normal = true;
  enum modes {SEEDS_ADD, SEEDS_REMOVE, SEEDS_NOP};
  modes editing_mode = SEEDS_NOP;
	
  const SDL_VideoInfo* info = NULL;
  int width = 600;
  int height = 600;
  int bpp = 0;
  int flags = 0;

  Uint32 interval;
  GLuint Plane;
  //  GLfloat angle = 0.0f;
  // GLfloat roty = 0.0f;
  //GLfloat points[NPOINTS][NPOINTS][3];


  float threshold = 0.9f;
  float max_val = -100.0f;
  
  float zoom = 1.0f;

  bool do_band = false;
  float alert_center = 5.0f;
  float alert_width = 1.0f;

  /* ----- SDL init --------------- */
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    //fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
    exit(-1);
  }

  atexit(SDL_Quit);
	

  //stuff:
  Xs section;
  //generate points:

  //storage
  typedef vector<Point> Storage; 
  Storage points;

  /*
  typedef vector<unsigned int> undo_step;
  typedef vector<undo_step> undo_buffer;
  undo_buffer the_undo;
  undo_step * cur_undo_step;
  */
  
  undo hard_undo; //undoes final shape modifications

  
  //array manipulation
  raw_volume vol;
  vol.read_nifti_file("bSS081a_1_3.hdr", "bSS081a_1_3.img");
  vol.load("bSS081a_1_3.img");
  //Array<float, 3> vol(load_data_float("bSS081a_1_3.img", TinyVector<int, 3> (vol_data.dim[0], vol_data.dim[1], vol_data.dim[2]) ));

  sets.allPoints.clear();
  sets.allPointsToKill.clear();
  int was = SDL_GetTicks();
  //  printf("%d points found ", find_points(vol, allPoints));
  TestPredicate my_test(vol);
  printf("%d points found ", find_points_predicate(vol, my_test, sets.allPoints));
  printf("in %f seconds.\n", 
	 (float)(SDL_GetTicks()-was)/1000.0);

  V3f u(1.0f/(float)vol.dim[0], 1.0f/(float)vol.dim[0], 1.0f/(float)vol.dim[0]);
  Vtr grid(V3f(-u.x*vol.dim[0]/2.0f,-u.y*vol.dim[1]/2.0f,-u.z*vol.dim[2]/2.0f), u);

 
  was = SDL_GetTicks();




      

  info = SDL_GetVideoInfo();
  bpp = info->vfmt->BitsPerPixel;

	
  /* ----- OpenGL attribute setting via SDL --------------- */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
  /*	flags = SDL_OPENGL | SDL_FULLSCREEN; */
  flags = SDL_OPENGL | SDL_RESIZABLE;

  /* ----- Setting up the screen surface --------------- */
  if((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0) {
    //  fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
    exit(-1);
  }

  SDL_WM_SetCaption("Voxel Brain (1)", NULL);

  


  /* ----- OpenGL init --------------- */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( -3.0, 3.0, -3.0, 3.0, -20.0, 20.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  //  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glDisable(GL_POINT_SMOOTH);
  // glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glPointSize(POINTSIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  /* ------Lighting -------*/

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  GLfloat specular[] = {0.1f, 0.1f, 0.1f, 0.1f};
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);




  /* ----- Plane init --------------- */
  /* int i, j;
     for(i=0; i<NPOINTS; i++) {
     for(j=0; j<NPOINTS; j++) {
     points[i][j][0] = NPOINTS*(-DISTANCE/2.0f) + i*DISTANCE;
     points[i][j][1] = 0.0f;
     points[i][j][2] = NPOINTS*(DISTANCE/2.0f) - j*DISTANCE;
     }
     }*/

  Plane = glGenLists (1);

  //structure 
  float rotx = 0.0f; //rel coords
  float roty = 0.0f;
  float mousex = 0.0f; 
  float mousey = 0.0f;

  V3f point;	
  V3i i_point;	
	
  int band[2];
  band[0]=0;
  band[1]=1;
  int coord_updated=true;
  int band_focus=0; //which band to fill in.
  int sparse=0;
  Storage::iterator i;
  V3i case_pnt;

  //dumping:
  FILE * f;
  FILE * f_colors;
  //axes
  float count=0;

  /* ----- Event cycle --------------- */


  while (!quit) {
    //automated eroding
    if(do_erode){
      int killed = erode_band(vol, sets.allPoints, sets.allPointsSelected,sets.allPointsToKill, (int)band[0], (int)band[1]);
      printf("Eroded %d points.\n", killed);
      if(killed == 0)do_erode = false;
    };
    while (SDL_PollEvent(&event)) {

      V3f cur_coords(point.x*vol.dim[0]+vol.dim[0]/2,
		     point.y*vol.dim[0]+vol.dim[1]/2,
		     point.z*vol.dim[0]+vol.dim[2]/2);


      int modDir = 0;

      switch (event.type) {
	//resize
      case SDL_VIDEORESIZE:
	/* ----- Setting up the screen surface --------------- */
	width = event.resize.w;
	height = event.resize.h; 
	printf("Trying to resize... %dx%d \n", event.resize.w, event.resize.h);
	if((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0) {
	  //  fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
	  exit(-1);
	}

	glViewport(0,0,width, height);

	//SDL_WM_SetCaption("Brain Voxel", NULL);
	/* ----- OpenGL init --------------- */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( -3.0*width/height*zoom, 3.0*width/height*zoom, -3.0*zoom, 3.0*zoom, -20.0, 20.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	//  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glDisable(GL_POINT_SMOOTH);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(POINTSIZE*(float)width/(float)850/zoom);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	/* ------Lighting -------*/

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//  GLfloat specular[] = {0.1, 0.1, 0.1, 0.1};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	break;

      case SDL_QUIT:
	quit = 1;
	break;
      case SDL_MOUSEMOTION:
 
	coord_updated = true;
        if(event.motion.state & SDL_BUTTON(1)){ //update rotation of the brain
	  rotx+=event.motion.xrel;
	  roty+=event.motion.yrel;
	};

	 if(event.motion.state & SDL_BUTTON(4))printf("Wheel up\n");
	 if(event.motion.state & SDL_BUTTON(5))printf("Wheel down\n");
	
	mousex = event.motion.x;
	mousey = event.motion.y;
        break;
      
      case SDL_KEYDOWN:
	switch( event.key.keysym.sym ){
	  //eroding, fast
	case SDLK_e: 
	  //printf("Eroded %d points.\n", erode_band(vol, sets.allPoints, sets.allPointsSelected, sets.allPointsToKill, (int)band[0], (int)band[1]));
		for(int i = 0; i < application_times; i++){
		propagator.plan(vol);
	  propagator.act(vol);
		}; application_times*=2;
	  break;

	case SDLK_g:
	  do_erode=!do_erode;
	  break;

	case SDLK_o:
	  update_band_interactively =!update_band_interactively;
	  break;


	case SDLK_c:
	  use_colors=!use_colors;
	  break;

	  //switch normal rendering
	case SDLK_j:
	  proper_normal=!proper_normal;
	  break;

	case SDLK_t:
	  if(editing_mode != SEEDS_ADD)
	    editing_mode = SEEDS_ADD;
	  else
	    editing_mode = SEEDS_NOP;
		  
	  break;


	  //counting
	case SDLK_l: //changing low band
	  band_focus = 0;
	  band[band_focus]=0;
	  break;
	case SDLK_h: //changing high band
	  band_focus = 1;
	  band[band_focus]=0;
	  break;
	  break;
	  //counting [ugly :(]
	case SDLK_1: band[band_focus]*=10; band[band_focus]+=1; goto counted;
	case SDLK_2: band[band_focus]*=10; band[band_focus]+=2; goto counted;
	case SDLK_3: band[band_focus]*=10; band[band_focus]+=3; goto counted;
	case SDLK_4: band[band_focus]*=10; band[band_focus]+=4; goto counted;
	case SDLK_5: band[band_focus]*=10; band[band_focus]+=5; goto counted;
	case SDLK_6: band[band_focus]*=10; band[band_focus]+=6; goto counted;
	case SDLK_7: band[band_focus]*=10; band[band_focus]+=7; goto counted;
	case SDLK_8: band[band_focus]*=10; band[band_focus]+=8; goto counted;
	case SDLK_9: band[band_focus]*=10; band[band_focus]+=9; goto counted;
	case SDLK_0: band[band_focus]*=10; band[band_focus]+=0; goto counted;
	counted:
	  printf("Selected %d points", find_band(vol, sets.allPoints, sets.allPointsSelected, band[0], band[1])); 
	  break;

	  //save
	case SDLK_s:
	  printf("Saving...\n");
	  vol.save("bSS081a_1_3.img");
	  printf("done.\n");
	  break;



	  //quitting
	case SDLK_ESCAPE:
	case SDLK_q:
	  quit = 1;
	  break;

	case SDLK_d:
	  printf("Dumping:\n");
	  f = fopen("points.txt", "w");
	  f_colors = fopen("colors.txt", "w");
	  if(!(f && f_colors)){printf("Cannot open file.\n"); break;}
	  for(point_space::iterator i = sets.allPoints.begin(); i !=sets.allPoints.end(); i++){
	    case_pnt = key(i->first);
	    fprintf(f, "%d %d %d,\n", case_pnt.x, case_pnt.y, case_pnt.z);

	    fprintf(f_colors, "%f %f %f,\n", 
		    vol(case_pnt.x, case_pnt.y, case_pnt.z)/400.0,
		    vol(case_pnt.x, case_pnt.y, case_pnt.z)/400.0,
		    vol(case_pnt.x, case_pnt.y, case_pnt.z)/400.0);
	  };
	  fclose(f);
	  fclose(f_colors);
	  printf("Done.\n");
	  break;
	
	case SDLK_PAGEUP:
	  zoom*=1.1;
	  glPointSize(POINTSIZE*(float)width/(float)850/zoom);

	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  glOrtho( -3.0*width/height*zoom, 3.0*width/height*zoom, -3.0*zoom, 3.0*zoom, -20.0, 20.0 );
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();

	  printf("Zoom factor is %f\n", zoom);
	  break;

	case SDLK_PAGEDOWN:
	  zoom*=0.9;
	  glPointSize(POINTSIZE*(float)width/(float)850/zoom);

	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  glOrtho( -3.0*width/height*zoom, 3.0*width/height*zoom, -3.0*zoom, 3.0*zoom, -20.0, 20.0 );
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();
	  printf("Zoom factor is %f\n", zoom);
	  break;
	  //change band
	case SDLK_b:
	  do_band = !do_band;
	  band[0]=band[1];// signals not to use it for update;
	  break;

	case SDLK_v:
	  not_hidden = !not_hidden;
	  break;
        
	case SDLK_x:
	  only_modified = !only_modified;
	  break;

	case SDLK_n:
	  only_points = !only_points;
	  break;


	  //show crossections	
        case SDLK_p:
	  section.move(point);
	  section.show(!section.shown());
	  break;


	case SDLK_z:
	  if(hard_undo.empty()){
	    printf("No undo information.\n");
	    break;
	  }else{;
	  	undo::undo_step cur;
	  	hard_undo.restore(cur);
		  for(undo::undo_step::iterator i = cur.begin(); i!=cur.end(); i++){
		    vol.set(key(*i), -vol(case_pnt)); 
		  };		
	  };

	  sets.allPointsToKill.clear();
	  sets.allPoints.clear();
	  printf("%d points found ", find_points(vol, sets.allPoints));
	  printf("Undone.\n");
	  break;

	  //undo	
        case SDLK_a:

      application_times = 1;
      for(point_list::iterator i = propagator.active.begin(); i!=propagator.active.end(); i++){
	    V3i cur(key(*i)); if(vol(cur)>0)vol.set(cur,-vol(cur)); //actually deleting
        hard_undo.add_point(cur); //adding to undo
	  };
	  hard_undo.save();
	  printf("Saved undo information.\n");
	  propagator.active.clear();
	  sets.allPoints.clear();
	  printf("%d points found.", find_points(vol, sets.allPoints));
	  printf("Done.\n");
	  break;
	  //undo	
        case SDLK_u:
	  //printf("Unselecting...\n");
	  for(point_list::iterator i = sets.allPointsToKill.begin(); i!=sets.allPointsToKill.end(); i++){
	    case_pnt=key(*i);
	    vol.set(case_pnt, -vol(case_pnt));
	  };

	  sets.allPointsToKill.clear();
	  sets.allPoints.clear();
	  printf("%d points found ", find_points(vol, sets.allPoints));
	  printf("Done.\n");

	  break;
        
	
	case SDLK_m: // add axes
	  printf("Modifying shape...\n");
	  count=0;
	  for(int i=3;i<vol.dim[0]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(i,3+k,3+j,count);}; count++;};
	  for(int i=3;i<vol.dim[1]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(3+k,i,3+j,count);}; count++;};
	  for(int i=3;i<vol.dim[2]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(3+k,3+j,i,count);}; count++;};
	  sets.allPoints.clear();
	  sets.allPointsToKill.clear();
	  printf("%d points found ", find_points(vol, sets.allPoints));

	  printf("Done.\n");
	  break;

	case SDLK_y: // add sphere
	  printf("Modifying shape...\n");
	  for(int i=0;i<vol.dim[0];i++){
	    for(int j=0;j<vol.dim[1];j++){
	      for(int k=0;k<vol.dim[2];k++){
		vol.set(i,j,k,(vol.dim[0]*vol.dim[0]/16>((vol.dim[0]/2-i)*(vol.dim[0]/2-i)+(vol.dim[1]/2-j)*(vol.dim[1]/2-j)+(vol.dim[2]/2-k)*(vol.dim[2]/2-k)))?(i-vol.dim[0]/4)*600/vol.dim[0]/2:0);
		//i+j+k				vol(i,j,k)=(vol.dim[0]*vol.dim[0]/16>((vol.dim[0]/2-i)*(vol.dim[0]/2-i)+(vol.dim[1]/2-j)*(vol.dim[1]/2-j)+(vol.dim[2]/2-k)*(vol.dim[2]/2-k)))?i+j+k:0;
	      }}}
	  sets.allPoints.clear();
	  sets.allPointsToKill.clear();
	  printf("%d points found ", find_points(vol, sets.allPoints));
	  break;

	case SDLK_F1:
	  SDL_WM_ToggleFullScreen(screen);
	  break;
	}
      }
    }

		
    interval = FrameTiming();

		
    /* ----- Blitting on the screen --------------- */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		
    glLoadIdentity();
    glScalef(4.0f, 4.0f, 4.0f);
    glTranslatef(0.0f, 0.0f, -3.0f);
    glRotatef(rotx / 2.0, 0.0f, 1.0f, 0.0f);
    glRotatef(roty / 2.0, 1.0f, 0.0f, 0.0f);

    if(do_band){
      alert_center = mousex;
      alert_width = mousey/10;
      printf("band from %f, to %f\n", band[0], band[1]);
    }else{
      if(band[0]!=band[1]){
	alert_center=(band[0]+band[1])/2;
	alert_width=(band[1]-band[0])/2;
	if(alert_width==0)alert_width=2;
	sparse++;
	if((sparse%40)==0)printf("band from %f, to %f\n", alert_center-alert_width, alert_center+alert_width);
      };
    };
	
    /*	  //draw green opaque stuff to kill
	  glDisable (GL_BLEND);
	  glDisable(GL_LIGHTING);
	  glPointSize(POINTSIZE*(float)width/(float)850/zoom);
	  glBegin(GL_POINTS);
	  V3f pos;
	  glColor3f(0.0,1.0,0.0);
	  for(point_list::iterator i = sets.allPointsToKill.begin(); i!=sets.allPointsToKill.end(); i++){
	  grid.flip(pos, key(*i));
	  glVertex3f(pos.x, pos.y, pos.z);
	  }
	  glEnd();
    */

    glDisable (GL_BLEND);
    glDisable(GL_LIGHTING);
    glPointSize(POINTSIZE*(float)width/(float)850/zoom);
    glBegin(GL_POINTS);
    V3f pos;
    glColor3f(0.0,1.0,0.0);
    for(point_list::iterator i = propagator.active.begin(); i!=propagator.active.end(); i++){
      grid.flip(pos, key(*i));
      glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
	
	
    if(not_hidden && !only_modified){
      glEnable(GL_LIGHTING);
      //glPointSize(POINTSIZE*(float)width/(float)850/zoom);
      glDisable (GL_BLEND);
    }else{

      glEnable(GL_LIGHTING);
      glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
    };

    if(only_modified){
      glDisable (GL_BLEND);
      glDisable(GL_LIGHTING);		
    };
    //try transparency




    pnt_sorted points_to_sort;

    for(point_space::iterator i = sets.allPoints.begin(); i !=sets.allPoints.end(); i++){
      points_to_sort.push_back(i->second); };


    sort(points_to_sort.begin(), points_to_sort.end(), psortable(GetOGLDirection()));

    if(only_points){
      glPointSize(1.0);
    }else{
      glPointSize(POINTSIZE*(float)width/(float)850/zoom);
    };

    glBegin(GL_POINTS);

    //  for(point_space::iterator i = allPoints.begin(); i !=allPoints.end(); i++){
    for(pnt_sorted::iterator i = points_to_sort.begin(); i !=points_to_sort.end(); i++){
      Point tmp = *i;
      //  if((vol(ipos.x,ipos.y,ipos.z)>=band[0] && (vol(ipos.x,ipos.y,ipos.z)<=band[1]))  ||
      // (!(ipos.x%5) && !(ipos.y%5) && !(ipos.z%5))){
      V3f pnt;
      grid.flip(pnt, tmp.pos);

#if 1 //highlight
      float factor = smooth_bell((pnt-point).length2()*100);
      float alert;
      //      if(((points[i+6])>mousex/5-mousey/10) && ((points[i+6])<mousex/5+mousey/10))
      //if(vol(tmp.pos.x,tmp.pos.y,tmp.pos.z)>=band[0] && (vol(tmp.pos.x,tmp.pos.y,tmp.pos.z)<=band[1]))
      if(sets.allPointsSelected.find(key(tmp.pos))!=sets.allPointsSelected.end())	 //not in the list  
	alert=1.0;//smoothBell((float)(points[i+6]-alert_center)/(float)alert_width);
      else{ 
	alert=0.0;
	if(only_modified)continue;
      };
      //glColor3f(0.5+0.5*alert,0.5,0.5+factor);
      //alert = 1.0;
      //  glColor3f(3.0*points[i+6]/max_val+0.5*alert,3.0*points[i+6]/max_val-0.5*alert,3.0*points[i+6]/max_val+factor-0.5*alert);
      if(!use_colors)tmp.col.set(0.5,0.5,0.5);
      if(alert > 0.5)tmp.col.set(1.0,0.0,0.0);
      glColor4f(tmp.col.x,tmp.col.y,tmp.col.z, 0.1+0.9*alert);
#else //don't highlight
      glColor3f((i%10)*0.1f,0.5f,0.5f);
#endif
      //\if(0 == (cur % 34*9) || (points[i+6] > 0.00001))printf("%f\n", points[i+6]);

      if(proper_normal){ 
	glNormal3f(tmp.norm.x, tmp.norm.y, tmp.norm.z); 
      }else{
	glNormal3f(pnt.x,pnt.y,pnt.z);
      };
      glVertex3f(pnt.x,pnt.y,pnt.z);
    };
    glEnd();
    glDisable (GL_BLEND);
    glDisable(GL_LIGHTING);		





    //mouse

    point = GetOGLPos((int)mousex, (int)mousey);
    //grid.flip(pnt, ipos);

    section.move(point);
    section.draw();
    ///cur
    //print x,y,z
    V3f dir = GetOGLDirection();
    dir/=dir.length();


    if(coord_updated){
      for(int i = 0; i<100; i++){
	for(int sgn = -1; sgn <= 1; sgn+=2){ 
	  V3f cur_v = point+(dir*0.001*i*sgn);  
	  V3i iv;
  	  grid.flip(iv, cur_v);
	  if(iv.x>0 && iv.y>0 && iv.z>0 && iv.x<vol.dim[0] && iv.y<vol.dim[1] && iv.z<vol.dim[2]) //check that it is here.
	    {
	      if(vol(iv.x, iv.y, iv.z)>1.0 && is_border(vol, V3i(iv.x, iv.y, iv.z))){
		//printf("f(%d,%d,%d)=%f\n", iv.x, iv.y, iv.z, vol(iv.x, iv.y, iv.z)); //here is what we've got
		grid.flip(point, V3i(iv.x, iv.y, iv.z)); // actual point
		i_point = iv;
		goto found;
	      };
	    }; //if in
	}; //altering sign
      };
      printf("Cannot locate volume...\n");
    found:
      if(SEEDS_ADD==editing_mode){
	//sets.allPointsSelected.insert(key(i_point));
	propagator.active.insert(key(i_point));
	propagator.set_band(vol);
	if(update_band_interactively){
	  if(sets.allPointsSelected.size()==1){ //ok, we are starting a new selection
	    band[0]=vol(i_point)-10;
	    band[1]=vol(i_point)+10;
	  }
	  if(band[0]>vol(i_point))band[0]=vol(i_point);
	  if(band[1]<vol(i_point))band[1]=vol(i_point);
	}
      }
      coord_updated=false;
    };
    //      glCallList(Plane);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_POINTS);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(point.x,
	       point.y,
	       point.z);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    //}


    V3f cur_coords(point.x*vol.dim[0]+vol.dim[0]/2,
		   point.y*vol.dim[0]+vol.dim[1]/2,
		   point.z*vol.dim[0]+vol.dim[2]/2);


    //printf("(%f:%f:%f)\n", cur_coords.x, cur_coords.y, cur_coords.z); 
    if(section.shown()){
      // area:
      const int xraySize = 200;
      const int xrayBufLength = xraySize*xraySize*3;
      int buf[xrayBufLength*4+1];
    

      //x- y
      for(int ix  = 0; ix < xraySize; ix++) 
	for(int iy  = 0; iy < xraySize; iy++){
	  int curx = (int)(cur_coords.x + (ix - (xraySize/2))/2);
	  int cury = (int)(cur_coords.y + (iy - (xraySize/2))/2);
	  int curz = (int)(cur_coords.z);
	  if(( (curx < vol.dim[0]) && (curx > 0) ) && // check bounds
	     ( (cury < vol.dim[1]) && (cury > 0) ) &&
	     ( (curz < vol.dim[2]) && (curz > 0) )){
	    int cvol = (int)vol(curx, cury, curz)/4;
	    //  if(vol(curx, cury, curz) > band[0] && vol(curx, cury, curz) < band[1])cvol=1;  
	    // if(allPointsToKill.find(pack(V3i(curx, cury, curz))) != allPointsToKill.end())
	    //	 buf[(iy*xraySize+ix)]= 0;
	    if(cvol <= 0){	  
	      buf[(iy*xraySize+ix)]= 70;
	    }else{
	      buf[(iy*xraySize+ix)]= cvol + (cvol << 8) + (cvol << 16);
	    };//if(vol_shell( curx, cury, curz ) > 1)buf[(iy*xraySize+ix)]=100;
	  }else{
	    buf[(iy*xraySize+ix)]=70;	    //out of bounds
	  };
	  //cross
	  if(ix==xraySize/2 && (iy < 7*xraySize/16 || iy > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
	  if(iy==xraySize/2 && (ix < 7*xraySize/16 || ix > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
	  //hit
	  if(propagator.active.find(key(V3i(curx, cury, curz))) != propagator.active.end()){ //we hit current selection
	    buf[(iy*xraySize+ix)]= 255 << 8;
	  };

	}; 

      V3f point2d = GetOGLPos((int)(10), int(10+xraySize), false);
      glRasterPos3f(point2d.x, point2d.y, point2d.z);
      glDrawPixels(xraySize, xraySize,GL_RGBA, GL_UNSIGNED_BYTE, buf);  

      //z - x
      for(int ix  = 0; ix < xraySize; ix++) 
	for(int iy  = 0; iy < xraySize; iy++){
	  int curx = cur_coords.x + (ix - (xraySize/2))/2;
	  int curz = cur_coords.z + (iy - (xraySize/2))/2;
	  int cury = cur_coords.y;
	  if(( (curx < vol.dim[0]) && (curx > 0) ) && // check bounds
	     ( (cury < vol.dim[1]) && (cury > 0) ) &&
	     ( (curz < vol.dim[2]) && (curz > 0) )){
	    int cvol = vol(curx, cury, curz)/4;
	    // if(vol(curx, cury, curz) > band[0] && vol(curx, cury, curz) < band[1])cvol=1;  
	    if(cvol <= 0)	  buf[(iy*xraySize+ix)]= 70 << 8;
	    else
	      buf[(iy*xraySize+ix)]= cvol + (cvol << 8) + (cvol << 16);
	    //if(vol_shell( curx, cury, curz ) > 1)buf[(iy*xraySize+ix)]=100;
	  }else{
	    buf[(iy*xraySize+ix)]= 70 << 8;	    //out of bounds
	  };
	  //cross
	  if(ix==xraySize/2 && (iy < 7*xraySize/16 || iy > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
	  if(iy==xraySize/2 && (ix < 7*xraySize/16 || ix > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
	  //hit
	  if(propagator.active.find(key(V3i(curx, cury, curz))) != propagator.active.end()){ //we hit current selection
	    buf[(iy*xraySize+ix)]= 255 << 8;
	  };

	}; 

      point2d = GetOGLPos((int)(10), int(15+2*xraySize), false);
      glRasterPos3f(point2d.x, point2d.y, point2d.z);
      glDrawPixels(xraySize, xraySize,GL_RGBA, GL_UNSIGNED_BYTE, buf);  

      //y- z
      for(int ix  = 0; ix < xraySize; ix++) 
	for(int iy  = 0; iy < xraySize; iy++){
	  int curz = (int)(cur_coords.z + (ix - (xraySize/2))/2);
	  int cury = (int)(cur_coords.y + (iy - (xraySize/2))/2);
	  int curx = cur_coords.x;
	  if(( (curx < vol.dim[0]) && (curx > 0) ) && // check bounds
	     ( (cury < vol.dim[1]) && (cury > 0) ) &&
	     ( (curz < vol.dim[2]) && (curz > 0) )){
	    int cvol = vol(curx, cury, curz)/4;
	    // if(vol(curx, cury, curz) > band[0] && vol(curx, cury, curz) < band[1])cvol=1;  
	    if(cvol <= 0)	  buf[(iy*xraySize+ix)]= 70 << 16;
	    else
	      buf[(iy*xraySize+ix)]= cvol + (cvol << 8) + (cvol << 16);
	    //if(vol_shell( curx, cury, curz ) > 1)buf[(iy*xraySize+ix)]=100;
	  }else{
	    buf[(iy*xraySize+ix)]= 70 << 16;	    //out of bounds
	  };
	  //cross
	  if(ix==xraySize/2 && (iy < 7*xraySize/16 || iy > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
	  if(iy==xraySize/2 && (ix < 7*xraySize/16 || ix > 9*xraySize/16 )) buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];

	  //hit
	  if(propagator.active.find(key(V3i(curx, cury, curz))) != propagator.active.end()){ //we hit current selection
	    buf[(iy*xraySize+ix)]= 255 << 8;
	  };
	
	}; 

      point2d = GetOGLPos((int)(10), int(20+3*xraySize), false);
      glRasterPos3f(point2d.x, point2d.y, point2d.z);
      glDrawPixels(xraySize, xraySize,GL_RGBA, GL_UNSIGNED_BYTE, buf);  
    };
      		

    SDL_GL_SwapBuffers();		
    /*		SDL_Delay(20); */ /* Decomment this if you want 1/50th screen update */
  }

	
  /* ----- Quitting --------------- */
  glDeleteLists(Plane, 1);
  SDL_Quit();
  return 0;	
}


/* Calculate frame interval and print FPS each 5s */
int FrameTiming(void)
{
  Uint32 interval;
  static Uint32 current, last = 0, five = 0, nframes = 0;

  current = SDL_GetTicks();
  nframes++;

  if (current - five > 5*1000) {
    printf("%u frames in 5 seconds = %.1f FPS\n", nframes, (float)nframes/5.0f);
    nframes = 0;
    five = current;
  }
	
  interval = current - last;
  last = current;
	
  return interval;
}



