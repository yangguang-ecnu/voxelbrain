#include "wxgl.h"

#define ID_NEW_WINDOW 10000
#define ID_DEF_ROTATE_LEFT_KEY 10001
#define ID_DEF_ROTATE_RIGHT_KEY 10002


/*----------------------------------------------------------------
  Implementation of Test-GLCanvas
  -----------------------------------------------------------------*/

BEGIN_EVENT_TABLE(MainGLCanvas, wxGLCanvas)
EVT_SIZE(MainGLCanvas::OnSize)
EVT_PAINT(MainGLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(MainGLCanvas::OnEraseBackground)
EVT_KEY_DOWN( MainGLCanvas::OnKeyDown )
EVT_KEY_UP( MainGLCanvas::OnKeyUp )
EVT_ENTER_WINDOW( MainGLCanvas::OnEnterWindow )
EVT_MOTION( MainGLCanvas::OnMouseMove )
END_EVENT_TABLE()

unsigned long  MainGLCanvas::m_secbase = 0;
int            MainGLCanvas::m_TimeInitialized = 0;
unsigned long  MainGLCanvas::m_xsynct;
unsigned long  MainGLCanvas::m_gsynct;

MainGLCanvas::MainGLCanvas(wxWindow *parent, wxWindowID id,
			   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
  m_init = false;
  m_gllist = 0;
  m_rleft = WXK_LEFT;
  m_rright = WXK_RIGHT;
}

MainGLCanvas::MainGLCanvas(wxWindow *parent, const MainGLCanvas *other,
			   wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
			   const wxString& name )
  : wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
{
  m_init = false;
  m_gllist = other->m_gllist; // share display list
  m_rleft = WXK_LEFT;
  m_rright = WXK_RIGHT;
}

MainGLCanvas::~MainGLCanvas()
{
}

void MainGLCanvas::OnMouseMove(wxMouseEvent& event){
  printf("Trying to move mouse.\n");
  mm->coord_updated = true;
  mm->mousex = event.m_x;
  mm->mousey = event.m_y;

  Refresh();

#if 0 // legacy code
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
				  dy = dy+(0.03*event.motion.yrel)/2;
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

#endif //end legacy
};


void MainGLCanvas::Render()
{
  if(!mm)return; //nothing to display yet.
 //wxPaintDC dc(this);

#ifndef __WXMOTIF__
  if (!GetContext()) return;
#endif

  SetCurrent();
  // Init OpenGL once, but after SetCurrent
  if (!m_init)
    {
      InitGL();
      m_init = true;
    }

  mm->begin_frame();
  glDisable (GL_BLEND);
  glDisable(GL_LIGHTING);
  mm->render_selection();
  mm->sort_points();
  printf("Rendering points...\n");
  mm->render_points();

  /**** detecting the point *****/
  mm->point = GetOGLPos((int)mm->mousex, (int)mm->mousey);
  V3f dir = GetOGLDirection();
  dir/=dir.length();
  bool not_found = true;
  printf("point(%d,%d,%d);", mm->point.x, mm->point.y, mm->point.y); //here is what we've got
  printf("direction(%d,%d,%d);", dir.x, dir.y, dir.z); //here is what we've got
  
  if (mm->coord_updated /*&& !mm->shift_pressed*/) {
    for (int i = 0; i<100; i++) {
      for (int sgn = -1; sgn <= 1; sgn+=2) {
	V3f cur_v = mm->point+(dir*0.0001*i*sgn);
	V3i iv;
	mm->grid.flip(iv, cur_v);
	if (iv.x>0 && iv.y>0 && iv.z>0 && 
	    iv.x < mm->vol.dim[0] && 
	    iv.y < mm->vol.dim[1] && 
	    iv.z < mm->vol.dim[2]) //check that it is here.
	  {
	    if (mm->vol(iv.x, iv.y, iv.z)>1.0 && is_border(mm->vol, V3i(iv.x, iv.y, iv.z))) {
	      printf("f(%d,%d,%d)=%f\n", iv.x, iv.y, iv.z, mm->vol(iv.x, iv.y, iv.z)); //here is what we've got
	      mm->grid.flip(mm->point, V3i(iv.x, iv.y, iv.z)); // actual point
	      mm->i_point = iv;
	      not_found = false;
	      goto found;
							
	    };
	  }; //if in
      }; //altering sign
    };
    //printf("Cannot locate volume...\n");
  found:
			    
    if(!not_found)mm->cross_point = mm->point;
    /*    if (SEEDS_ADD==mm->editing_mode) {
      //select_point();
      if ( update_band_interactively ) {
	if (sets.allPointsSelected.size()==1) { //ok, we are starting a new selection
	  band[0]=vol(i_point)-10;
	  band[1]=vol(i_point)+10;
	}
	if (band[0]>vol(i_point))
	  band[0]=vol(i_point);
	if (band[1]<vol(i_point))
	  band[1]=vol(i_point);
      }
    }*/
    mm->coord_updated=false;
    
    glEnable(GL_LIGHTING);
    // glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.4f, 0.0f);
    gen_sphere(mm->point, 0.009, 3);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    printf("(%f, %f, %f)\n", mm->point.x,  mm->point.y,  mm->point.z);

  };

  /*** render cursor now ***/


  /************ end point detection ********/

#if 0 ///Legacy code
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
  glMatrixMode(GL_MODELVIEW);

  /* clear color and depth buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if( m_gllist == 0 )
    {
      m_gllist = glGenLists( 1 );
      glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
      /* draw six faces of a cube */
      glBegin(GL_QUADS);
      glNormal3f( 0.0f, 0.0f, 1.0f);
      glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
      glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);

      glNormal3f( 0.0f, 0.0f,-1.0f);
      glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
      glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);

      glNormal3f( 0.0f, 1.0f, 0.0f);
      glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
      glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);

      glNormal3f( 0.0f,-1.0f, 0.0f);
      glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
      glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);

      glNormal3f( 1.0f, 0.0f, 0.0f);
      glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
      glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);

      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
      glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
      glEnd();

      glEndList();
    }
  else
    {
      glCallList(m_gllist);
    }
#endif //comment out legacy code
  glFlush();
  SwapBuffers();
}

void MainGLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
  SetFocus();
}

void MainGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  Render();
}

void MainGLCanvas::OnSize(wxSizeEvent& event)
{
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
#ifndef __WXMOTIF__
  if (GetContext())
#endif
    {
      SetCurrent();
      //  glViewport(0, 0, (GLint) w, (GLint) h);
      mm->width = w; mm->height = h;
      mm->wx_setup_screen();
    }
}

void MainGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void MainGLCanvas::InitGL()
{
  if(!mm)return;
  SetCurrent();

  mm->sets.allPoints.clear();
  mm->sets.allPointsToKill.clear();
  TestPredicate my_test(mm->vol);
  printf("%d points found ", 
	 find_points_predicate(mm->vol, 
			       my_test,		
			       mm->sets.allPoints));
  mm->u = V3f(1.0f/(float)mm->vol.dim[0], 
	      1.0f/(float)mm->vol.dim[0], 
	      1.0f/(float)mm->vol.dim[0]);
  mm->grid = Vtr(V3f(-mm->u.x*mm->vol.dim[0]/2.0f, 
		     -mm->u.y*mm->vol.dim[1]/2.0f, 
		     -mm->u.z*mm->vol.dim[2]/2.0f), 
		 mm->u);

  mm->wx_setup_screen();


#if 0 // do not use old code
  
  /* set viewing projection */
  glMatrixMode(GL_PROJECTION);
  glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

  /* position viewer */
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0.0f, 0.0f, -2.0f);

  /* position object */
  glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
#endif //comment out old code
}

GLfloat MainGLCanvas::CalcRotateSpeed( unsigned long acceltime )
{
  GLfloat t,v;

  t = ((GLfloat)acceltime) / 1000.0f;

  if( t < 0.5f )
    v = t;
  else if( t < 1.0f )
    v = t * (2.0f - t);
  else
    v = 0.75f;

  return(v);
}

GLfloat MainGLCanvas::CalcRotateAngle( unsigned long lasttime,
				       unsigned long acceltime )
{
  GLfloat t,s1,s2;

  t = ((GLfloat)(acceltime - lasttime)) / 1000.0f;
  s1 = CalcRotateSpeed( lasttime );
  s2 = CalcRotateSpeed( acceltime );

  return( t * (s1 + s2) * 135.0f );
}

void MainGLCanvas::Action( long code, unsigned long lasttime,
                           unsigned long acceltime )
{
  GLfloat angle = CalcRotateAngle( lasttime, acceltime );

  if (code == m_rleft)
    Rotate( angle );
  else if (code == m_rright)
    Rotate( -angle );
}

void MainGLCanvas::OnKeyDown( wxKeyEvent& event )
{
  long evkey = event.GetKeyCode();
  if (evkey == 0) return;

  if (!m_TimeInitialized)
    {
      m_TimeInitialized = 1;
      m_xsynct = event.GetTimestamp();
      m_gsynct = 0;//StopWatch(&m_secbase);

      m_Key = evkey;
      m_StartTime = 0;
      m_LastTime = 0;
      m_LastRedraw = 0;
    }

  unsigned long currTime = event.GetTimestamp() - m_xsynct;

  if (evkey != m_Key)
    {
      m_Key = evkey;
      m_LastRedraw = m_StartTime = m_LastTime = currTime;
    }

  if (currTime >= m_LastRedraw)      // Redraw:
    {
      Action( m_Key, m_LastTime-m_StartTime, currTime-m_StartTime );

#if defined(__WXMAC__) && !defined(__DARWIN__)
      m_LastRedraw = currTime;    // StopWatch() doesn't work on Mac...
#else
      m_LastRedraw = 0;//StopWatch(&m_secbase) - m_gsynct;
#endif
      m_LastTime = currTime;
    }

  event.Skip();
}

void MainGLCanvas::OnKeyUp( wxKeyEvent& event )
{
  m_Key = 0;
  m_StartTime = 0;
  m_LastTime = 0;
  m_LastRedraw = 0;

  event.Skip();
}

void MainGLCanvas::Rotate( GLfloat deg )
{
  SetCurrent();

  glMatrixMode(GL_MODELVIEW);
  glRotatef((GLfloat)deg, 0.0f, 0.0f, 1.0f);
  Refresh(false);
}


