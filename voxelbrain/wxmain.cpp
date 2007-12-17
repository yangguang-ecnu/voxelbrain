/////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: cube.cpp 35650 2005-09-23 12:56:45Z MR $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/stream.h>
#include <wx/zstream.h>
#include <wx/wfstream.h>


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "wxmain.h"
//#include "../../sample.xpm"

#ifndef __WXMSW__     // for StopWatch, see remark below
  #if defined(__WXMAC__) && !defined(__DARWIN__)
    #include <utime.h>
    #include <unistd.h>
  #else
    #include <sys/time.h>
    #include <sys/unistd.h>
  #endif
#else
#include <sys/timeb.h>
#endif


/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(Event_Load, MainFrame::OnLoadFile)
EVT_MENU(Event_Test, MainFrame::OnTest)
EVT_MENU(Event_Save, MainFrame::OnSaveFile)
END_EVENT_TABLE()

// My frame constructor
MainFrame::MainFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(parent, wxID_ANY, title, pos, size, style)
{
    m_canvas = NULL;
    //SetIcon(wxIcon(sample_xpm));
}

// Intercept menu commands
void MainFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

void MainFrame::load_file(const wxChar * name){
  wxFileInputStream input_file(name);
    wxZlibInputStream decompress(input_file);
    const int buff_size = 1000;
    char buff[buff_size];
    while(true){ //until nothing more is read
      decompress.Read((void *)buff,buff_size); 
      if(decompress.LastRead()==0)break; //eof, nothing to do
      //copying
      for(int i = 0; i < decompress.LastRead(); i++){
	mm.vol.m_data.push_back(buff[i]);
      };
    };
    //mm.vol.load_mgh((char *)name.c_str());
    printf("Uncompressed size is: %d", mm.vol.m_data.size());
    mm.vol.load_mgh_data(); //actual parsing happens here

    //setting proper size
    int w,h; m_canvas->GetSize(&w,&h);
    mm.width=w;
    mm.height=h;

    m_canvas->InitGL(); //re-init gl for this thing.

};

void MainFrame::OnLoadFile(wxCommandEvent& event){
  wxString name = ::wxFileSelector(_T("Load data"), 
                                   _T(""),
				   _T(""),
				   _T(""),
                                   _T("*.mgz")); //TODO: add decompression 
  if(!name.empty()){
    //printf(name.c_str());
    load_file(name);
  }else{
    printf("Ok, next time.\n");
  };
};

void MainFrame::OnTest(wxCommandEvent & event){
  try{
    load_file(_T("test.mgz"));
  }catch(char const * i){
    printf("Unable to load test file; reason: %s\n", i);
  };
};

void MainFrame::OnSaveFile(wxCommandEvent& event){
  wxString name = ::wxFileSelector(_T("Save data"), 
                                   _T(""),
				   _T(""),
				   _T(""),
                                   _T("FreeSurfer volume(*.mgz)")); 
  if(!name.empty()){
    //printf(name.c_str());
  }else{
    printf("Ok, next time.\n");
  };
};


/*static*/ MainFrame *MainFrame::Create(MainFrame *parentFrame, bool isCloneWindow)
{
    wxString str = wxT("wxWidgets OpenGL Cube Sample");
    if (isCloneWindow) str += wxT(" - Clone");

    MainFrame *frame = new MainFrame(NULL, str, wxDefaultPosition,
        wxSize(400, 300));

    // Make a menubar
    wxMenu *winMenu = new wxMenu;
    winMenu->Append(Event_Load, _T("&Load MGZ"));
    winMenu->Append(Event_Save, _T("&Save MGZ"));

    // Make a test bar
    wxMenu *testMenu = new wxMenu;
    testMenu->Append(Event_Test, _T("&Sample file"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(winMenu, _T("&File"));
    menuBar->Append(testMenu, _T("&Test"));

    frame->SetMenuBar(menuBar);

    if (parentFrame)
    {
        frame->m_canvas = new MainGLCanvas( frame, parentFrame->m_canvas,
            wxID_ANY, wxDefaultPosition, wxDefaultSize );
    }
    else
    {
        frame->m_canvas = new MainGLCanvas(frame, wxID_ANY,
            wxDefaultPosition, wxDefaultSize);
    }

    //setting core
    frame->m_canvas->mm = &(frame->mm);
    
    // Show the frame
    frame->Show(true);

    return frame;
}

/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // Create the main frame window
    (void) MainFrame::Create(NULL);

    return true;
}
