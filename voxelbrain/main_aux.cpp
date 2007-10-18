#include "main.h"

	main_module::main_module() {
		ill.setup(2, 0.2);
		do_erode = false;
		shift_pressed = false;
		
		info = NULL;
		width = 600;
		height = 600;
		bpp = 0;
		flags = 0;

		threshold = 0.9f;
		max_val = -100.0f;

		zoom = 0.2f;
		POINTSIZE=1;

		do_band = false;
		selection_run=false;
		alert_center = 5.0f;
		alert_width = 1.0f;

		disable_lighting = false;
		application_times = 1;
		quit = 0;
		not_hidden = true;
		hide_selection = false;
		update_band_interactively = false;
		only_modified = false;
		only_points = false;
		use_colors = true;

		proper_normal = true;

	
		editing_mode = SEEDS_NOP;

		count = 0;

		//structure 
		//rotx = 0.0f;
		//roty = 0.0f;
		mousex = 0.0f;
		mousey = 0.0f;

		do_erosion=false;
		do_zoom=false;
		do_erosion_level=0; //select if we want undo or propagation

		band[0]=0;
		band[1]=1;
		coord_updated=true;
		band_focus=0; //which band to fill in.
		sparse=0;
		background_color = 0;
		
		//lastZ = 0;
		
		view.center = V3f(0.0f, 0.0f, 0.0f);
		view.eye = V3f(0.0f, 0.0f, -5.0f);
		view.up = V3f(0.0f, 1.0f, 0.0f);

	};


void main_module::select_point() {
	propagator.active.insert(key(i_point));
//	propagator.border.insert(key(i_point));
	propagator.set_band(vol);
};

void main_module::recalculate_points(){
	  sets.allPoints.clear();
	  sets.allPointsToKill.clear();
	  printf("%d points found ", find_points(vol, sets.allPoints));
}

void main_module::perform_undo(){
	  if(hard_undo.empty()){
	    printf("No undo information; unselecting\n");
		  for(point_list::iterator i = sets.allPointsToKill.begin(); i!=sets.allPointsToKill.end(); i++){
		    case_pnt=key(*i);
		    vol.set(case_pnt, -vol(case_pnt));
		  };
		  printf("Done.\n");
	    return;
	  }else{;
	  	undo::undo_step cur;
	  	hard_undo.restore(cur);
		  for(undo::undo_step::iterator i = cur.begin(); i!=cur.end(); i++){
		      V3i cur(key(*i));
			  //printf("Restored %d, %d, %d\n", cur.x, cur.y, cur.z);
			  if(vol(cur)<0)vol.set(cur, -vol(cur)); 
		  };		
	  };
	  printf("Undone.\n");
	  return;
}

void main_module::ajust_band(){
		alert_center = mousex;
		alert_width = mousey/10;
		printf("band from %f, to %f\n", band[0], band[1]);
		if (band[0]!=band[1]) {
			alert_center=(band[0]+band[1])/2;
			alert_width=(band[1]-band[0])/2;
			if (alert_width==0)
				alert_width=2;
			sparse++;
			if ((sparse%40)==0)
				printf("band from %f, to %f\n", alert_center-alert_width,
						alert_center+alert_width);
		};
}

void main_module::apply_modification(){
    application_times = 1;
    for(point_list::iterator i = propagator.active.begin(); i!=propagator.active.end(); i++){
	    V3i cur(key(*i)); if(vol(cur)>0)vol.set(cur,-vol(cur)); //actually deleting
      hard_undo.add_point(*i); //adding to undo
	  };
	  hard_undo.save();
	  printf("Saved undo information.\n");
	  propagator.active.clear();
//	  propagator.border.clear();
	  sets.allPoints.clear();
	  printf("%d points found.", find_points(vol, sets.allPoints));
	  printf("Done.\n");
}

void main_module::predefined_shape_axes(){
	  printf("Modifying shape...\n");
	  count=0;
	  for(int i=3;i<vol.dim[0]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(i,3+k,3+j,count);}; count++;};
	  for(int i=3;i<vol.dim[1]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(3+k,i,3+j,count);}; count++;};
	  for(int i=3;i<vol.dim[2]-3;i++){for(int k=0; k<=3; k++){for(int j=0; j<3; j++)vol.set(3+k,3+j,i,count);}; count++;};	
}

void main_module::predefined_shape_sphere(){
	  printf("Modifying shape...\n");
	  for(int i=0;i<vol.dim[0];i++){
	    for(int j=0;j<vol.dim[1];j++){
	      for(int k=0;k<vol.dim[2];k++){
		vol.set(i,j,k,(vol.dim[0]*vol.dim[0]/16>((vol.dim[0]/2-i)*(vol.dim[0]/2-i)+(vol.dim[1]/2-j)*(vol.dim[1]/2-j)+(vol.dim[2]/2-k)*(vol.dim[2]/2-k)))?(i-vol.dim[0]/4)*600/vol.dim[0]/2:0);
		//i+j+k				vol(i,j,k)=(vol.dim[0]*vol.dim[0]/16>((vol.dim[0]/2-i)*(vol.dim[0]/2-i)+(vol.dim[1]/2-j)*(vol.dim[1]/2-j)+(vol.dim[2]/2-k)*(vol.dim[2]/2-k)))?i+j+k:0;
	      }}}
}


void main_module::setup_projection(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( -3.0*width/height*zoom, 3.0*width/height*zoom, -3.0*zoom,
			3.0*zoom, -20.0, 20.0);
	

}

int main_module::setup_screen() {
	printf("Trying to resize... %dx%d \n", width, height);
	if ((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0) {
		//  fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		exit(-1);
	}

	glViewport(0, 0, width, height);

	//SDL_WM_SetCaption("Brain Voxel", NULL);
	/* ----- OpenGL init --------------- */
	setup_projection();
	
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(view.eye.x, view.eye.y, view.eye.z,
			  view.center.x, view.center.y, view.center.z,
			  view.up.x, view.up.y, view.up.z);
	glLoadIdentity();


	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	//  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	
	
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(POINTSIZE*(float)width/(float)850/zoom);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	/* ------Lighting -------*/

	if (!disable_lighting)
		glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
    GLfloat specular[] = {0.1, 0.1, 0.1, 0.1};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void main_module::begin_frame(){
	/* ----- Blitting on the screen --------------- */
	setup_projection();
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(POINTSIZE*(float)width/(float)850/zoom);
	glLoadIdentity();
	gluLookAt(view.eye.x, view.eye.y, view.eye.z,
			  view.center.x, view.center.y, view.center.z,
			  view.up.x, view.up.y, view.up.z);
//	glScalef(4.0f, 4.0f, 4.0f);
//	glTranslatef(0.0f, 0.0f, -3.0f);
	//glRotatef(rotx / 2.0, 0.0f, 1.0f, 0.0f);
	//glRotatef(roty / 2.0, 1.0f, 0.0f, 0.0f);


	glDisable (GL_BLEND);
	glDisable(GL_LIGHTING);
}

void main_module::render_selection(){
	glBegin(GL_POINTS);
	V3f pos;
	glColor3f(0.0, 1.0, 0.0);
	for (point_list::iterator i = propagator.active.begin(); i
			!=propagator.active.end(); i++) {
		grid.flip(pos, key(*i));
		if (!hide_selection ) {
			//gen_sphere(pos, 0.008, 3);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}
	glEnd();
}

void main_module::recenter_camera(){
	V3f pos; V3f average = V3f(0.0f, 0.0f, 0.0f); int n = 0;
	for (point_list::iterator i = propagator.active.begin(); i
			!=propagator.active.end(); i++) {
		grid.flip(pos, key(*i));
		average+= pos; n++;
	}
	if(n!=0)average/=n;
	view.center=average;
};

void main_module::sort_points(){
	points_to_sort.clear();
	for (point_space::iterator i = sets.allPoints.begin(); i
			!=sets.allPoints.end(); i++) {
		points_to_sort.push_back(i->second);
	};

	sort(points_to_sort.begin(), points_to_sort.end(),
			psortable(GetOGLDirection()));
	
	printf("Sorted %d points\n", points_to_sort.size());
}

void main_module::render_points(){
	glBegin(GL_POINTS);

	for (pnt_sorted::iterator i = points_to_sort.begin(); i
			!=points_to_sort.end(); i++) {
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
		if (sets.allPointsSelected.find(key(tmp.pos))
				!=sets.allPointsSelected.end()) //not in the list  
			alert=1.0;//smoothBell((float)(points[i+6]-alert_center)/(float)alert_width);
		else {
			alert=0.0;
			if (only_modified)
				continue;
		};
		//glColor3f(0.5+0.5*alert,0.5,0.5+factor);
		//alert = 1.0;
		//  glColor3f(3.0*points[i+6]/max_val+0.5*alert,3.0*points[i+6]/max_val-0.5*alert,3.0*points[i+6]/max_val+factor-0.5*alert);

		tmp.col.x = ill.eval(tmp.col.x);
		tmp.col.y = tmp.col.x;
		tmp.col.z = tmp.col.x;
		if (!use_colors)
			tmp.col.set(0.5, 0.5, 0.5);
		if (alert > 0.5)
			tmp.col.set(1.0, 0.0, 0.0);
		glColor4f(tmp.col.x, tmp.col.y, tmp.col.z, 0.1+0.9*alert);
#else //don't highlight
		glColor3f((i%10)*0.1f,0.5f,0.5f);
#endif
		//\if(0 == (cur % 34*9) || (points[i+6] > 0.00001))printf("%f\n", points[i+6]);

		if (proper_normal ) {
			glNormal3f(tmp.norm.x, tmp.norm.y, tmp.norm.z);
		} else {
			glNormal3f(pnt.x, pnt.y, pnt.z);
		};
		glVertex3f(pnt.x, pnt.y, pnt.z);
	};
	
	glEnd();
}

void main_module::catch_cursor(const V3i & dx, const V3i & dy, int xcenter, int ycenter, int r){
	printf("Checking mouse %d-%d\n", mousex, mousey);
	V3i cur_coords;
	V3i res;
	if((ABS(mousex - xcenter) < r) && (ABS(mousey - ycenter) < r)){
		printf("Got inside...%d, %d\n", (mousex - xcenter), (mousey - ycenter));
		grid.flip(cur_coords, point);
		res=cur_coords+dx*((mousex-xcenter)/3)+dy*((mousey-ycenter)/3);
//		grid.flip(point, res);
		propagator.active.insert(key(res));
		
	}
}

void main_module::crossection_plane(const V3i & dx, const V3i & dy, int  xpos, int  ypos, int color){
	bool section_border;
	// area:
	const int xraySize = 200;
	const int xrayBufLength = xraySize*xraySize*3;
	int * buf;
	buf = new int[xrayBufLength*4+1];
	
	
	
	V3i cur_coords;
	V3i cur_pnt;
	grid.flip(cur_coords, point);

	//finding max:
	int slice_max = 0;
	for (int ix = 0; ix < xraySize; ix++)
		for (int iy = 0; iy < xraySize; iy++) {
			V3i cur = dx * ((ix - xraySize/2)/3)+dy * ((iy - xraySize/2)/3) + cur_coords;
			if (( (cur.x < vol.dim[0]) && (cur.x > 0)) && // check bounds
				( (cur.y < vol.dim[1]) && (cur.y > 0))&&
				( (cur.z < vol.dim[2]) && (cur.z > 0))) 
				    if(vol(cur)>slice_max)slice_max=vol(cur);
		};

	if(slice_max==0)slice_max=1; //to avoid /0
		
		//main loop
	for (int ix = 0; ix < xraySize; ix++)
		for (int iy = 0; iy < xraySize; iy++) {
			if ((ix < 2) || (ix >= (xraySize - 2))|| (iy < 2)|| (iy
					>= (xraySize - 2)))
				section_border = true;
			else
				section_border = false;
			V3i cur = dx * ((ix - xraySize/2)/3)+dy * ((iy - xraySize/2)/3) + cur_coords;

			if (( (cur.x < vol.dim[0]) && (cur.x > 0)) && // check bounds
					( (cur.y < vol.dim[1]) && (cur.y > 0))&&( (cur.z
					< vol.dim[2]) && (cur.z > 0))) {
				//				int cvol = 255*(int)vol(cur)/vol.max;
								int cvol = 180*(int)vol(cur)/slice_max;
				if (cvol <= 0) {
					buf[(iy*xraySize+ix)]= section_border ? 255 << (color * 8)
							: background_color;
				} else {
					buf[(iy*xraySize+ix)]= cvol + (cvol << 8)+ (cvol
							<< 16);
				};//if(vol_shell( curx, cury, curz ) > 1)buf[(iy*xraySize+ix)]=100;
			} else {
				buf[(iy*xraySize+ix)]=section_border ? 255
						: background_color; //out of bounds
			};
			//cross
			if (ix==xraySize/2&& (iy< 7*xraySize/16|| iy > 9*xraySize/
					16 ))
				buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
			if (iy==xraySize/2&& (ix< 7*xraySize/16|| ix > 9*xraySize/
					16 ))
				buf[(iy*xraySize+ix)]=~buf[(iy*xraySize+ix)];
			//hit
			if (!(hide_selection) && propagator.active.find(key(cur)) != propagator.active.end()) { //we hit current selection
				buf[(iy*xraySize+ix)] |= (255 << 8);
			};

		}
	;

	V3f point2d = GetOGLPos(xpos, ypos+xraySize, false);
	glRasterPos3f(point2d.x, point2d.y, point2d.z);
	glDrawPixels(xraySize, xraySize, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	delete buf;
}

void main_module::dump_test_data(){
	  printf("Dumping:\n");
	  f = fopen("points.txt", "w");
	  f_colors = fopen("colors.txt", "w");
	  if(!(f && f_colors)){printf("Cannot open file.\n"); return;}
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
}

bool environment::parse(int argc, char ** argv){
	if(argc == 1){ // ok, just everything as usual
		input_header = "bSS081a_1_3.hdr";
		input_data = "bSS081a_1_3.img";
		output_header = "bSS081a_1_3.hdr";
		output_data = "bSS081a_1_3.img";
		return true;
	}else if (argc == 2 || argc == 3) {
		string filename(argv[1]);
		string data(filename);
		string::size_type pos = filename.find(".hdr");
		if(string::npos == pos){
		   err = "Incorrect extension\n";
		   return false;
		}else{
			data.replace(pos, 4, ".img");
		}
		input_header = filename;
		input_data = data;
		if(argc == 2){ //use same files
		output_header = filename;
		output_data = data;
		
		}else{ //parse outfile
			filename=string(argv[2]);
			data=string(filename);
			string::size_type pos = filename.find(".hdr");
			if(string::npos == pos){
			   err = "Incorrect extension\n";
			   return false;
			}else{
				data.replace(pos, 4, ".img");
			}
			output_header = filename;
			output_data = data;
		}
		return true;
	}else{
		err = "Zero or one argument expected.\n";
		return false;		
	}
}

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
  tmp.col /= (float)vol.max;
  //todo calculate volume;
	
  return tmp;
};

bool is_border(raw_volume & vol, V3i cur){
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


V3f GetOGLPos(int x, int y, bool updateZ )
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
 // if(updateZ){
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 //   lastZ = winZ;
 // }else{
 //   winZ = lastZ;
  //};
	
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



//draws an array of pixels, rgba, provided in 'data';
void drawSlice(int w, int h, unsigned char * data){
  //  Vector3 c = GetOGLPos(1, 1+h);
  //  glRasterPos3f(c.x, c.y, 0.0);
  glDrawPixels(w,h,GL_RGB, GL_BYTE, (void *)data);  
};



//testing predicates
	
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

/* View */

V3f main_module::side(view_t & in){
	V3f diff(in.center-in.eye);
	printf("Diff:"); ::inspect(diff);
	V3f s; s.cross(diff,in.up);
	printf("Side, crossed:"); ::inspect(s);	
	s /= s.length();
	printf("Side, normaized:"); ::inspect(s);	
	return s;
}

void main_module::normalize(view_t & in){
	in.up.cross(side(in), in.center-in.eye);
	in.up /= in.up.length();
	V3f sight = (in.center-in.eye);
	sight /= sight.length();
	in.eye = in.center-sight;
}

void main_module::move(view_t & in, float x, float y){
	inspect(in);
	printf("Moving\n");
	in.eye += side(in)*x+in.up*y;
	printf("Moved\n");
	inspect(in);
	printf("Normaizing\n");
	normalize(in);
	inspect(in);
}


void main_module::inspect(const view_t & in){
	printf("Eye:"); ::inspect(in.eye);
	printf("Center:");::inspect(in.center);
	printf("Up:");::inspect(in.up);
}


/* Calculate frame interval and print FPS each 5s */
int FrameTiming(void) {
	Uint32 interval;
	static Uint32 current, last = 0, five = 0, nframes = 0;

	current = SDL_GetTicks();
	nframes++;

	if (current - five > 5*1000) {
		printf("%u frames in 5 seconds = %.1f FPS\n", nframes, (float)nframes
				/5.0f);
		nframes = 0;
		five = current;
	}

	interval = current - last;
	last = current;

	return interval;
}



