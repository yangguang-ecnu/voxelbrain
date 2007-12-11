#include <wx/wx.h>

class Simple : public wxFrame
{
public:
    Simple(const wxString& title);

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
    Simple *simple = new Simple(wxT("Simple"));
    simple->Show(true);

    return true;
}







