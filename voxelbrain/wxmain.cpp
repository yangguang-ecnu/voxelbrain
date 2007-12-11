#include <wx/wx.h>

class Simple : public wxFrame
{
public:
    Simple(const wxString& title);
    wxMenu * m_menu;
};

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};


Simple::Simple(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
  Centre();
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    Simple *simple = new Simple(wxT("Voxbr (WX)"));
    simple->Show(true);

    simple->m_menu = new wxMenu();
    simple->m_menu->Append(1, wxT("Open"), wxT("Operations with file."));
    simple->m_menu->Append(2, wxT("Close"), wxT("Operations with file."));

    return true;
}





