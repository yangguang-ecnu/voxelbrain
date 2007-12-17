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

#if 0
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


