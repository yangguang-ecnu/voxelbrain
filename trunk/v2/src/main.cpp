/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:
 
   Implementation of main(), using main_module
   Keyboard handling
   Rendering sequence

*/

/*!
 \mainpage Interactive MRI Editing Software
 
  \section intro_sec Introduction
 
  For brain research people use MRI scans. Automatic algorithms and software like freesurfer can automatically extract brain data from whole head scans.
  The goal of current software is to allow user to monitor and fix defects in automatically extracted brain images.
 
  \section install_sec Building and installation
 
  The software was designed to run on Windows, Macosx and Linux.
  Respective installation processes are described below.

  \subsection install_win Windows
	
  The standard MSVC 2005 workspace is provided in the root directory of the project. 
  To test the compilation result you can start it from inside the visual studio.
  Resulting executable is located under <voxbr_folder>/bin folder; 

  \subsection install_osx Mac Os X 

  Mac OS:
    install Development Kit which is provided with Mac OsX.
    open terminal and use the following commands:

	\code
	cd <voxbr_folder>/src
	make -f Makefile.osx install 
    \endcode

	The application will be compiled and placed into <voxbr_folder>/bin folder.

	\subsection install_lin Linux
    open terminal and use the following commands:

	\code
	cd <voxbr_folder>/src
	make -f Makefile.linux install 
    \endcode

	The application will be compiled and placed into <voxbr_folder>/bin folder.   
  
  \section Operation Overview

  Facilitating manual segmentation in 3D includes the following:
\li	Developing a software tool which would integrate into FreeSurfer pipeline and allow interactive MRI segmentation
\li	Finding optimal toolset for efficient manual segmentation in 3D.
\li	Exploration of methods to help user in interactive segmentation, i.e. semi-automatic segmentation methods.

Overall screen looks as follows:


  \subsection Visualization
  \subsection Point selection
  \subsection Editing
  
  \section Implementation details

 */

#include "main.h"

/*!
Strucrure for processing events

*/
struct event_processor{
	

	main_module & mm;  /// The reference to the main module; for use in the event processing.
	

    /*!
	   Constructor takes the main module to use in event processing.
	*/
	event_processor(main_module & host):mm(host){
	};


};

/***************************************************************************
 * 
 /////////////////////////////////////   MAIN   ////////////////////////////
 *
 **************************************************************************/

int main_module::start(int argc, char **argv) {

#if HAS_SDL
	/* ----- SDL init --------------- */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		//fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}


      
	atexit(SDL_Quit);

	//LOADING
        if(argc == 2){
	  try{
	  	  vol.load_mgh(argv[1]);
	  }catch(const char * in){
	    printf("Unable to load file: %s\n", in);
	    return -1;
	  };
	}else{

	 if (!volenv.parse(argc, argv)) {
		 printf(volenv.err.c_str());
		 exit(1);
	 }
	 vol.read_nifti_file(volenv.input_header.c_str(), volenv.input_data.c_str());
	};
	//array manipulation

	//vol.load(volenv.input_data.c_str());
	//Array<float, 3> vol(load_data_float("bSS081a_1_3.img", TinyVector<int, 3> (vol_data.dim[0], vol_data.dim[1], vol_data.dim[2]) ));

	sets.allPoints.clear();
	sets.allPointsToKill.clear();
	int was = SDL_GetTicks();
	//  printf("%d points found ", find_points(vol, allPoints));
	TestPredicate my_test(vol);
	printf("%d points found ", find_points_predicate(vol, my_test,
			sets.allPoints));
	//printf("in %f seconds.\n", (float)(SDL_GetTicks()-was)/1000.0);

	u = V3f(1.0f/(float)vol.dim[0], 1.0f/(float)vol.dim[0], 1.0f
			/(float)vol.dim[0]);
	grid = Vtr(V3f(-u.x*vol.dim[0]/2.0f, -u.y*vol.dim[1]/2.0f, -u.z
			*vol.dim[2]/2.0f), u);

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

	setup_screen();
	SDL_WM_SetCaption(SVN_REVISION, NULL);

	while (!quit) {
		//automated eroding
		if (do_erode) {
			int killed = erode_band(vol, sets.allPoints,
					sets.allPointsSelected, sets.allPointsToKill, (int)band[0],
					(int)band[1]);
			//printf("Eroded %d points.\n", killed);
			if (killed == 0)
				do_erode = false;
		};
		//		printf("Entering event loop...\n");
		while (SDL_PollEvent(&event)) {
		  //  printf("Got event %d\n", event.type);

			V3f cur_coords(point.x*vol.dim[0]+vol.dim[0]/2, point.y*vol.dim[0]
					+vol.dim[1]/2, point.z*vol.dim[0]+vol.dim[2]/2);

			int modDir = 0;
			section.show(true);

			/*****************************
			 * MEGASWITCH
			 * ***************************/

			switch (event.type) {
			//resize
			case SDL_VIDEORESIZE: width=event.resize.w; height=event.resize.h; setup_screen(); break;
			case SDL_QUIT: quit = 1; break;
			case SDL_MOUSEBUTTONDOWN:if(editing_mode == SEEDS_ADD)select_point(); break;
			case SDL_MOUSEMOTION:
			  /*if (do_erosion) { //propagate
					if (event.motion.yrel < 0) {
						propagator.undo_step();
					} else {
						for(int i = event.motion.yrel; i > 0; i--){
						   propagator.plan(vol);
						   propagator.act(vol);
						};
					};
					break;
					}*/
				coord_updated = true;
				if(do_zoom)zoom*=1.0f+0.01f*event.motion.yrel;
				if(do_pane_zoom){
				  paneSize=(int)((float)(paneSize)*(1.0f+0.01f*event.motion.yrel));
				  printf("Zooming panes...%d\n", paneSize);
				};
				if (!shift_pressed && editing_mode != SEEDS_ADD && event.motion.state
						& SDL_BUTTON(1)) { //update rotation of the brain
					//rotx+=event.motion.xrel;
				//	roty+=event.motion.yrel;
					dx = dx+(-0.03*event.motion.xrel)/2;
					dy = (dy+0.03*event.motion.yrel)/2;
					//view.eye.x+=0.01+event.motion.xrel;
					//view.eye.y+=0.01+event.motion.yrel;
					//inspect(view);
				}
				;
				mousex = event.motion.x;
				mousey = event.motion.y;
				
				if (editing_mode == SEEDS_ADD && event.motion.state
						& SDL_BUTTON(1)){
					if(!shift_pressed)select_point();
					catch_cursor(V3i(1,0,0), V3i(0,-1,0), 10+100, 10+100, 100);
					catch_cursor(V3i(1,0,0), V3i(0,0,1), 10+100, 10+210+100, 100);
					catch_cursor(V3i(0,1,0), V3i(0,0,-1), 10+100, 10+2*210+100, 100);
				};
				if (event.motion.state & SDL_BUTTON(4))
					//printf("Wheel up\n");
				if (event.motion.state & SDL_BUTTON(5))
					//printf("Wheel down\n");


				break;

				/*************************************
				 * Release Keys
				 * ***********************************/
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LALT: do_erosion = false; break;
				case SDLK_RALT: do_zoom=false;break;
				case SDLK_LSHIFT: shift_pressed = false; break;
				case SDLK_RSHIFT: do_pane_zoom = false; break;
				case SDLK_LCTRL: editing_mode = SEEDS_NOP;break;
				case SDLK_g: hide_selection = false;break;
				case SDLK_r: selection_run=false;break;
				}
				
				break;

				/**************************************
				 * Press Keys
				 * ************************************/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LSHIFT: shift_pressed = true; break;
				case SDLK_RSHIFT: do_pane_zoom = true; break;
				case SDLK_LALT:do_erosion=true;break;
				case SDLK_RALT:do_zoom=true;break;
				case SDLK_g:hide_selection=true;break;
				case SDLK_c:use_colors=!use_colors;break;
				case SDLK_i:propagator.do_internals=!propagator.do_internals; break;
				case SDLK_r:selection_run=true;break;
				case SDLK_j:proper_normal=!proper_normal;break;
				case SDLK_LCTRL:editing_mode = SEEDS_ADD;break;
				case SDLK_l:band_focus = 0;band[band_focus]=0;break;
				case SDLK_h:band_focus = 1;band[band_focus]=0;break;
				case SDLK_1:
				case SDLK_2:
				case SDLK_3:
				case SDLK_4:
				case SDLK_5:
				case SDLK_6:
				case SDLK_7:
				case SDLK_8:
				case SDLK_9:
				case SDLK_0:
					band[band_focus]*=10;
					band[band_focus]+=event.key.keysym.sym-SDLK_0;
					printf("Selected %d points", find_band(vol, sets.allPoints,
							sets.allPointsSelected, band[0], band[1]));
					break;
				case SDLK_s:vol.write_nifti_file(volenv.output_header.c_str(),volenv.output_data.c_str());break;
				case SDLK_w:vol.save_mgh("result.mgh");break;
				case SDLK_ESCAPE:
				case SDLK_q: quit = 1;break;
				case SDLK_F3: recenter_camera(); break;
				case SDLK_d:dump_test_data();break;
				case SDLK_PAGEUP:zoom*=1.1;break;
				case SDLK_PAGEDOWN:zoom*=0.9;break;
				case SDLK_b:background_color = (background_color == 0) ? ((255<<16)+(255<<8)+255) : 0;break;
				case SDLK_v:not_hidden = !not_hidden;break;
				case SDLK_o:disable_lighting = !disable_lighting;break;
				case SDLK_x:only_modified = !only_modified;break;
				case SDLK_n:only_points = !only_points;break;
				case SDLK_p:section.show(!section.shown());break;
				case SDLK_z:perform_undo();recalculate_points();break;
				case SDLK_SPACE:apply_modification();recalculate_points();break;
				case SDLK_m:predefined_shape_axes();recalculate_points();break;
				case SDLK_y:predefined_shape_sphere();recalculate_points();break;
				case SDLK_F1: SDL_WM_ToggleFullScreen(screen);break;
				}
			}
		}

		//FPS counting
		interval = FrameTiming();
		
		//if moving, move
		//if((ABS(dx) > 0.0001) && (ABS(dy) > 0.0001) ){
		if((!do_zoom) &&(!do_pane_zoom))move(view, dx, dy);
		  dx*=0.3; dy*=0.3;
		//};
		
		if(selection_run){
			if(!shift_pressed){
				float cur_threshold;
				for(int tries = 0; tries < 10; tries++){
			  propagator.plan(vol);
			  cur_threshold = propagator.act(vol);
			  if(cur_threshold < 0)break;
		      if(cur_threshold < propagation_threshold){
		    	  propagation_threshold = cur_threshold; break;
		      }
				};//
			}else{
				propagator.undo_step();
			}
		}
		if(do_band)ajust_band();
		
		
		/* ----- Blitting on the screen --------------- */
        begin_frame();

		glDisable (GL_BLEND);
		glDisable(GL_LIGHTING);
		glPointSize(POINTSIZE*(float)width/(float)850/zoom);
		render_selection();
		
		if (not_hidden && !only_modified) {
			if (!disable_lighting)
				glEnable(GL_LIGHTING);
			glDisable (GL_BLEND);
		} else {
			if (!disable_lighting)
				glEnable(GL_LIGHTING);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		};

		if (only_modified) {
			glDisable (GL_BLEND);
			glDisable(GL_LIGHTING);
		};


		sort_points();
		
		if (only_points) {
			glPointSize(1.0);
		} else {
			glPointSize(POINTSIZE*(float)width/(float)2050/zoom);
		};

		render_points();

		glDisable (GL_BLEND);
		glDisable(GL_LIGHTING);

		//mouse

		if (!do_erosion ) {
			if(!shift_pressed)point = GetOGLPos((int)mousex, (int)mousey);
			//grid.flip(pnt, ipos);
		};
		
	//	if(shift_pressed){
		  section.move(point);
		  section.draw();
	//	};
		///cur
		//print x,y,z
		V3f dir = GetOGLDirection();
		dir/=dir.length();
		bool not_found = true;
		
		if (coord_updated && !shift_pressed) {
			for (int i = 0; i<100; i++) {
				for (int sgn = -1; sgn <= 1; sgn+=2) {
					V3f cur_v = point+(dir*0.0001*i*sgn);
					V3i iv;
					grid.flip(iv, cur_v);
					if (iv.x>0&& iv.y>0&& iv.z>0&& iv.x<vol.dim[0]&& iv.y
							<vol.dim[1]&& iv.z<vol.dim[2]) //check that it is here.
					{
						if (vol(iv.x, iv.y, iv.z)>17.0&& is_border(vol, V3i(
								iv.x, iv.y, iv.z))) {
							printf("f(%d,%d,%d)=%f\n", iv.x, iv.y, iv.z, vol(iv.x, iv.y, iv.z)); //here is what we've got
							grid.flip(point, V3i(iv.x, iv.y, iv.z)); // actual point
							i_point = iv;
							not_found = false;
							goto found;
							
						};
					}; //if in
				}; //altering sign
			};
			printf("Cannot locate volume...\n");
			found:
			    
			    if(!not_found)cross_point = point;
				if (SEEDS_ADD==editing_mode) {
				//select_point();
				if (update_band_interactively) {
					if (sets.allPointsSelected.size()==1) { //ok, we are starting a new selection
						band[0]=vol(i_point)-10;
						band[1]=vol(i_point)+10;
					}
					if (band[0]>vol(i_point))
						band[0]=vol(i_point);
					if (band[1]<vol(i_point))
						band[1]=vol(i_point);
				}
			}
			coord_updated=false;
		};
		//      glCallList(Plane);
		glEnable(GL_LIGHTING);
		// glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.4f, 0.0f);
		gen_sphere(point, 0.009, 3);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		//}


		if (true /*section.shown()*/) {

		  /*	bool section_border;
			// area:
			int xraySize = 200;
			const int xrayBufLength = xraySize*xraySize*3;
			int buf[xrayBufLength*4+1]; */
		 
		  //paneSize = 100;

			glDisable(GL_DEPTH_TEST);
			crossection_plane(V3i(1,0,0), V3i(0,1,0), 10, 10, paneSize,0);
			crossection_plane(V3i(1,0,0), V3i(0,0,-1), 10, 10+10+paneSize, paneSize, 1);
			crossection_plane(V3i(0,1,0), V3i(0,0,1), 10, 10+2*(10+paneSize), paneSize, 2);
			glEnable(GL_DEPTH_TEST);
		};

		SDL_GL_SwapBuffers();
		/*		SDL_Delay(20); *//* Decomment this if you want 1/50th screen update */
	}

	/* ----- Quitting --------------- */
	SDL_Quit();

#endif //HAS_SDL
	return 0;
}


int main_module::start_glfw(int argc, char **argv) {

   // Initialise GLFW
    glfwInit();

    // Open OpenGL window
    if( !glfwOpenWindow( width, height, 0,0,0,0, 0,0, GLFW_WINDOW ) )
    {
        glfwTerminate();
        return 0;
    }

    // Enable sticky keys
    glfwEnable( GLFW_STICKY_KEYS );

    // Disable vertical sync (on cards that support it)
    glfwSwapInterval( 0 );

	//LOADING
        if(argc == 2){
	  try{
	  	  vol.load_mgh(argv[1]);
	  }catch(const char * in){
	    printf("Unable to load file: %s\n", in);
	    return -1;
	  };
	}else{

	 if (!volenv.parse(argc, argv)) {
		 printf(volenv.err.c_str());
		 exit(1);
	 }
	 vol.read_nifti_file(volenv.input_header.c_str(), volenv.input_data.c_str());
	};
	//array manipulation

	//vol.load(volenv.input_data.c_str());
	//Array<float, 3> vol(load_data_float("bSS081a_1_3.img", TinyVector<int, 3> (vol_data.dim[0], vol_data.dim[1], vol_data.dim[2]) ));

	sets.allPoints.clear();
	sets.allPointsToKill.clear();

	//  printf("%d points found ", find_points(vol, allPoints));
	TestPredicate my_test(vol);
	printf("%d points found ", find_points_predicate(vol, my_test,
			sets.allPoints));
	//printf("in %f seconds.\n", (float)(SDL_GetTicks()-was)/1000.0);
//TODO:deprecated
	u = V3f(1.0f/(float)vol.dim[0], 1.0f/(float)vol.dim[0], 1.0f
			/(float)vol.dim[0]);
	grid = Vtr(V3f(-u.x*vol.dim[0]/2.0f, -u.y*vol.dim[1]/2.0f, -u.z
			*vol.dim[2]/2.0f), u);


	setup_screen();
//	SDL_WM_SetCaption(SVN_REVISION, NULL);

	while (!quit) {
		//automated eroding
#ifdef HAS_SDL
		if (do_erode) {
			int killed = erode_band(vol, sets.allPoints,
					sets.allPointsSelected, sets.allPointsToKill, (int)band[0],
					(int)band[1]);
			//printf("Eroded %d points.\n", killed);
			if (killed == 0)
				do_erode = false;
		};
		//		printf("Entering event loop...\n");
		while (SDL_PollEvent(&event)) {
		  //  printf("Got event %d\n", event.type);

			V3f cur_coords(point.x*vol.dim[0]+vol.dim[0]/2, point.y*vol.dim[0]
					+vol.dim[1]/2, point.z*vol.dim[0]+vol.dim[2]/2);

			int modDir = 0;
			section.show(true);

			/*****************************
			 * MEGASWITCH
			 * ***************************/

			switch (event.type) {
			//resize
			case SDL_VIDEORESIZE: width=event.resize.w; height=event.resize.h; setup_screen(); break;
			case SDL_QUIT: quit = 1; break;
			case SDL_MOUSEBUTTONDOWN:if(editing_mode == SEEDS_ADD)select_point(); break;
			case SDL_MOUSEMOTION:
			  /*if (do_erosion) { //propagate
					if (event.motion.yrel < 0) {
						propagator.undo_step();
					} else {
						for(int i = event.motion.yrel; i > 0; i--){
						   propagator.plan(vol);
						   propagator.act(vol);
						};
					};
					break;
					}*/
				coord_updated = true;
				if(do_zoom)zoom*=1.0f+0.01f*event.motion.yrel;
				if(do_pane_zoom){
				  paneSize=(int)((float)(paneSize)*(1.0f+0.01f*event.motion.yrel));
				  printf("Zooming panes...%d\n", paneSize);
				};
				if (!shift_pressed && editing_mode != SEEDS_ADD && event.motion.state
						& SDL_BUTTON(1)) { //update rotation of the brain
					//rotx+=event.motion.xrel;
				//	roty+=event.motion.yrel;
					dx = dx+(-0.03*event.motion.xrel)/2;
					dy = (dy+0.03*event.motion.yrel)/2;
					//view.eye.x+=0.01+event.motion.xrel;
					//view.eye.y+=0.01+event.motion.yrel;
					//inspect(view);
				}
				;
				mousex = event.motion.x;
				mousey = event.motion.y;
				
				if (editing_mode == SEEDS_ADD && event.motion.state
						& SDL_BUTTON(1)){
					if(!shift_pressed)select_point();
					catch_cursor(V3i(1,0,0), V3i(0,-1,0), 10+100, 10+100, 100);
					catch_cursor(V3i(1,0,0), V3i(0,0,1), 10+100, 10+210+100, 100);
					catch_cursor(V3i(0,1,0), V3i(0,0,-1), 10+100, 10+2*210+100, 100);
				};
				if (event.motion.state & SDL_BUTTON(4))
					//printf("Wheel up\n");
				if (event.motion.state & SDL_BUTTON(5))
					//printf("Wheel down\n");


				break;

				/*************************************
				 * Release Keys
				 * ***********************************/
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LALT: do_erosion = false; break;
				case SDLK_RALT: do_zoom=false;break;
				case SDLK_LSHIFT: shift_pressed = false; break;
				case SDLK_RSHIFT: do_pane_zoom = false; break;
				case SDLK_LCTRL: editing_mode = SEEDS_NOP;break;
				case SDLK_g: hide_selection = false;break;
				case SDLK_r: selection_run=false;break;
				}
				
				break;

				/**************************************
				 * Press Keys
				 * ************************************/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LSHIFT: shift_pressed = true; break;
				case SDLK_RSHIFT: do_pane_zoom = true; break;
				case SDLK_LALT:do_erosion=true;break;
				case SDLK_RALT:do_zoom=true;break;
				case SDLK_g:hide_selection=true;break;
				case SDLK_c:use_colors=!use_colors;break;
				case SDLK_i:propagator.do_internals=!propagator.do_internals; break;
				case SDLK_r:selection_run=true;break;
				case SDLK_j:proper_normal=!proper_normal;break;
				case SDLK_LCTRL:editing_mode = SEEDS_ADD;break;
				case SDLK_l:band_focus = 0;band[band_focus]=0;break;
				case SDLK_h:band_focus = 1;band[band_focus]=0;break;
				case SDLK_1:
				case SDLK_2:
				case SDLK_3:
				case SDLK_4:
				case SDLK_5:
				case SDLK_6:
				case SDLK_7:
				case SDLK_8:
				case SDLK_9:
				case SDLK_0:
					band[band_focus]*=10;
					band[band_focus]+=event.key.keysym.sym-SDLK_0;
					printf("Selected %d points", find_band(vol, sets.allPoints,
							sets.allPointsSelected, band[0], band[1]));
					break;
				case SDLK_s:vol.write_nifti_file(volenv.output_header.c_str(),volenv.output_data.c_str());break;
				case SDLK_w:vol.save_mgh("result.mgh");break;
				case SDLK_ESCAPE:
				case SDLK_q: quit = 1;break;
				case SDLK_F3: recenter_camera(); break;
				case SDLK_d:dump_test_data();break;
				case SDLK_PAGEUP:zoom*=1.1;break;
				case SDLK_PAGEDOWN:zoom*=0.9;break;
				case SDLK_b:background_color = (background_color == 0) ? ((255<<16)+(255<<8)+255) : 0;break;
				case SDLK_v:not_hidden = !not_hidden;break;
				case SDLK_o:disable_lighting = !disable_lighting;break;
				case SDLK_x:only_modified = !only_modified;break;
				case SDLK_n:only_points = !only_points;break;
				case SDLK_p:section.show(!section.shown());break;
				case SDLK_z:perform_undo();recalculate_points();break;
				case SDLK_SPACE:apply_modification();recalculate_points();break;
				case SDLK_m:predefined_shape_axes();recalculate_points();break;
				case SDLK_y:predefined_shape_sphere();recalculate_points();break;
				case SDLK_F1: SDL_WM_ToggleFullScreen(screen);break;
				}
			}
		}

		//FPS counting
		interval = FrameTiming();
		
		//if moving, move
		//if((ABS(dx) > 0.0001) && (ABS(dy) > 0.0001) ){
		if((!do_zoom) &&(!do_pane_zoom))move(view, dx, dy);
		  dx*=0.3; dy*=0.3;
		//};
		
		if(selection_run){
			if(!shift_pressed){
				float cur_threshold;
				for(int tries = 0; tries < 10; tries++){
			  propagator.plan(vol);
			  cur_threshold = propagator.act(vol);
			  if(cur_threshold < 0)break;
		      if(cur_threshold < propagation_threshold){
		    	  propagation_threshold = cur_threshold; break;
		      }
				};//
			}else{
				propagator.undo_step();
			}
		}
		if(do_band)ajust_band();
#endif //HAS_SDL
		
		/* ----- Blitting on the screen --------------- */
        begin_frame();

		glDisable (GL_BLEND);
		glDisable(GL_LIGHTING);
		glPointSize(POINTSIZE*(float)width/(float)850/zoom);
		render_selection();
		
		if (not_hidden && !only_modified) {
			if (!disable_lighting)
				glEnable(GL_LIGHTING);
			glDisable (GL_BLEND);
		} else {
			if (!disable_lighting)
				glEnable(GL_LIGHTING);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		};

		if (only_modified) {
			glDisable (GL_BLEND);
			glDisable(GL_LIGHTING);
		};


		sort_points();
		
		if (only_points) {
			glPointSize(1.0);
		} else {
			glPointSize(POINTSIZE*(float)width/(float)2050/zoom);
		};

		render_points();

		glDisable (GL_BLEND);
		glDisable(GL_LIGHTING);

		//mouse

		if (!do_erosion ) {
			if(!shift_pressed)point = GetOGLPos((int)mousex, (int)mousey);
			//grid.flip(pnt, ipos);
		};
		
	//	if(shift_pressed){
		  section.move(point);
		  section.draw();
	//	};
		///cur
		//print x,y,z
		V3f dir = GetOGLDirection();
		dir/=dir.length();
		bool not_found = true;
		
		if (coord_updated && !shift_pressed) {
			for (int i = 0; i<100; i++) {
				for (int sgn = -1; sgn <= 1; sgn+=2) {
					V3f cur_v = point+(dir*0.0001*i*sgn);
					V3i iv;
					grid.flip(iv, cur_v);
					if (iv.x>0&& iv.y>0&& iv.z>0&& iv.x<vol.dim[0]&& iv.y
							<vol.dim[1]&& iv.z<vol.dim[2]) //check that it is here.
					{
						if (vol(iv.x, iv.y, iv.z)>17.0&& is_border(vol, V3i(
								iv.x, iv.y, iv.z))) {
							printf("f(%d,%d,%d)=%f\n", iv.x, iv.y, iv.z, vol(iv.x, iv.y, iv.z)); //here is what we've got
							grid.flip(point, V3i(iv.x, iv.y, iv.z)); // actual point
							i_point = iv;
							not_found = false;
							goto found;
							
						};
					}; //if in
				}; //altering sign
			};
			printf("Cannot locate volume...\n");
			found:
			    
			    if(!not_found)cross_point = point;
				if (SEEDS_ADD==editing_mode) {
				//select_point();
				if (update_band_interactively) {
					if (sets.allPointsSelected.size()==1) { //ok, we are starting a new selection
						band[0]=vol(i_point)-10;
						band[1]=vol(i_point)+10;
					}
					if (band[0]>vol(i_point))
						band[0]=vol(i_point);
					if (band[1]<vol(i_point))
						band[1]=vol(i_point);
				}
			}
			coord_updated=false;
		};
		//      glCallList(Plane);
		glEnable(GL_LIGHTING);
		// glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.4f, 0.0f);
		gen_sphere(point, 0.009, 3);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		//}


		if (true /*section.shown()*/) {

		  /*	bool section_border;
			// area:
			int xraySize = 200;
			const int xrayBufLength = xraySize*xraySize*3;
			int buf[xrayBufLength*4+1]; */
		 
		  //paneSize = 100;

			glDisable(GL_DEPTH_TEST);
			crossection_plane(V3i(1,0,0), V3i(0,1,0), 10, 10, paneSize,0);
			crossection_plane(V3i(1,0,0), V3i(0,0,-1), 10, 10+10+paneSize, paneSize, 1);
			crossection_plane(V3i(0,1,0), V3i(0,0,1), 10, 10+2*(10+paneSize), paneSize, 2);
			glEnable(GL_DEPTH_TEST);
		};

		//SDL_GL_SwapBuffers();
        // Swap buffers
        glfwSwapBuffers();
		/*		SDL_Delay(20); *//* Decomment this if you want 1/50th screen update */
	}

	/* ----- Quitting --------------- */
	//SDL_Quit();
	 glfwTerminate();


	return 0;
}



/***********************************************************
 *              ENTRY POINT                                *
 * *********************************************************/

///main() function. 
/*
Simply starts main_module structure.
 */ 

#include <stdio.h>
#include <GL/glfw.h>

int main_glfw(){


    int     width, height, running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];

    // Initialise GLFW
    glfwInit();

    // Open OpenGL window
    if( !glfwOpenWindow( 640, 480, 0,0,0,0, 0,0, GLFW_WINDOW ) )
    {
        glfwTerminate();
        return 0;
    }

    // Enable sticky keys
    glfwEnable( GLFW_STICKY_KEYS );

    // Disable vertical sync (on cards that support it)
    glfwSwapInterval( 0 );

    // Main loop
    running = GL_TRUE;
    frames = 0;
    t0 = glfwGetTime();
    while( running )
    {
        // Get time and mouse position
        t = glfwGetTime();
        glfwGetMousePos( &x, &y );

        // Calculate and display FPS (frames per second)
        if( (t-t0) > 1.0 || frames == 0 )
        {
            fps = (double)frames / (t-t0);
            sprintf( titlestr, "Spinning Triangle (%.1f FPS)", fps );
            glfwSetWindowTitle( titlestr );
            t0 = t;
            frames = 0;
        }
        frames ++;

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );
        height = height > 0 ? height : 1;

        // Set viewport
        glViewport( 0, 0, width, height );

        // Clear color buffer
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        // Select and setup the projection matrix
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f,
            100.0f );

        // Select and setup the modelview matrix
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        gluLookAt( 0.0f, 1.0f, 0.0f,    // Eye-position
                   0.0f, 20.0f, 0.0f,   // View-point
                   0.0f, 0.0f, 1.0f );  // Up-vector

        // Draw a rotating colorful triangle
        glTranslatef( 0.0f, 14.0f, 0.0f );
        glRotatef( 0.3*(GLfloat)x + (GLfloat)t*100.0f, 0.0f, 0.0f, 1.0f );
        glBegin( GL_TRIANGLES );
          glColor3f( 1.0f, 0.0f, 0.0f );
          glVertex3f( -5.0f, 0.0f, -4.0f );
          glColor3f( 0.0f, 1.0f, 0.0f );
          glVertex3f( 5.0f, 0.0f, -4.0f );
          glColor3f( 0.0f, 0.0f, 1.0f );
          glVertex3f( 0.0f, 0.0f, 6.0f );
        glEnd();

        // Swap buffers
        glfwSwapBuffers();

        // Check if the ESC key was pressed or the window was closed
        running = !glfwGetKey( GLFW_KEY_ESC ) &&
                  glfwGetWindowParam( GLFW_OPENED );
    }

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

int main(int argc, char ** argv) {
  main_module app;
  app.start_glfw(argc, argv);
  //	main_glfw();
	return 0;
}

#ifdef WIN32

#include <windows.h>
int WINAPI WinMain(      
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
	){
 main_module app;
	app.start_glfw(__argc, __argv);

//		main_glfw();

	return 0;
};

#endif //WIN32
