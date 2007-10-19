#include "main.h"



/**************************************************************************
 * 
 /////////////////////////////////////   MAIN   ////////////////////////////
 *
 **************************************************************************/

int main_module::start(int argc, char **argv) {

	/* ----- SDL init --------------- */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		//fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}

	atexit(SDL_Quit);

	if (!volenv.parse(argc, argv)) {
		printf(volenv.err.c_str());
		exit(1);
	}

	//array manipulation

	vol.read_nifti_file(volenv.input_header.c_str(), volenv.input_data.c_str());
	//vol.load(volenv.input_data.c_str());
	//Array<float, 3> vol(load_data_float("bSS081a_1_3.img", TinyVector<int, 3> (vol_data.dim[0], vol_data.dim[1], vol_data.dim[2]) ));

	sets.allPoints.clear();
	sets.allPointsToKill.clear();
	int was = SDL_GetTicks();
	//  printf("%d points found ", find_points(vol, allPoints));
	TestPredicate my_test(vol);
	printf("%d points found ", find_points_predicate(vol, my_test,
			sets.allPoints));
	printf("in %f seconds.\n", (float)(SDL_GetTicks()-was)/1000.0);

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
			printf("Eroded %d points.\n", killed);
			if (killed == 0)
				do_erode = false;
		};
		while (SDL_PollEvent(&event)) {

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
				if (do_erosion) { //propagate
					if (event.motion.yrel < 0) {
						propagator.undo_step();
					} else {
						for(int i = event.motion.yrel; i > 0; i--){
						   propagator.plan(vol);
						   propagator.act(vol);
						};
					};
					break;
				}
				coord_updated = true;
				if(do_zoom)zoom*=1.0f+0.01f*event.motion.yrel;
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
					printf("Wheel up\n");
				if (event.motion.state & SDL_BUTTON(5))
					printf("Wheel down\n");


				break;

				/*************************************
				 * Release Keys
				 * ***********************************/
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LALT: do_erosion = false; break;
				case SDLK_RALT:do_zoom=false;break;
				case SDLK_LSHIFT: shift_pressed = false; break;
				case SDLK_LCTRL: editing_mode = SEEDS_NOP;break;
				case SDLK_g:hide_selection = false;break;
				case SDLK_r:selection_run=false;break;
				}
				
				break;

				/**************************************
				 * Press Keys
				 * ************************************/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LSHIFT: shift_pressed = true; break;
				case SDLK_LALT:do_erosion=true;break;
				case SDLK_RALT:do_zoom=true;break;
				case SDLK_g:hide_selection=true;break;
				case SDLK_c:use_colors=!use_colors;break;
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
		if((ABS(dx) > 0.0001) && (ABS(dy) > 0.0001) ){
		  move(view, dx, dy);
		  dx*=0.3; dy*=0.3;
		};
		
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
		render_selection();
		
		if (not_hidden && !only_modified) {
			if (!disable_lighting)
				glEnable(GL_LIGHTING);
			//glPointSize(POINTSIZE*(float)width/(float)850/zoom);
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
			glPointSize(POINTSIZE*(float)width/(float)850/zoom);
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

		if (coord_updated && !shift_pressed) {
			for (int i = 0; i<100; i++) {
				for (int sgn = -1; sgn <= 1; sgn+=2) {
					V3f cur_v = point+(dir*0.0001*i*sgn);
					V3i iv;
					grid.flip(iv, cur_v);
					if (iv.x>0&& iv.y>0&& iv.z>0&& iv.x<vol.dim[0]&& iv.y
							<vol.dim[1]&& iv.z<vol.dim[2]) //check that it is here.
					{
						if (vol(iv.x, iv.y, iv.z)>1.0&& is_border(vol, V3i(
								iv.x, iv.y, iv.z))) {
							//printf("f(%d,%d,%d)=%f\n", iv.x, iv.y, iv.z, vol(iv.x, iv.y, iv.z)); //here is what we've got
							grid.flip(point, V3i(iv.x, iv.y, iv.z)); // actual point
							i_point = iv;
							goto found;
						};
					}; //if in
				}; //altering sign
			};
			printf("Cannot locate volume...\n");
			found: if (SEEDS_ADD==editing_mode) {
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


		if (section.shown()) {

			bool section_border;
			// area:
			const int xraySize = 200;
			const int xrayBufLength = xraySize*xraySize*3;
			int buf[xrayBufLength*4+1];
			glDisable(GL_DEPTH_TEST);
			crossection_plane(V3i(1,0,0), V3i(0,1,0), 10, 10, 0);
			crossection_plane(V3i(1,0,0), V3i(0,0,-1), 10, 10+210, 1);
			crossection_plane(V3i(0,1,0), V3i(0,0,1), 10, 10+2*210, 2);
		};
		glEnable(GL_DEPTH_TEST);

		SDL_GL_SwapBuffers();
		/*		SDL_Delay(20); *//* Decomment this if you want 1/50th screen update */
	}

	/* ----- Quitting --------------- */
	SDL_Quit();
	return 0;
}


/***********************************************************
 *              ENTRY POINT                                *
 * *********************************************************/

int main(int argc, char ** argv) {
	main_module app;
	app.start(argc, argv);
}

