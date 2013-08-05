#ifndef __MAINFRAME_CLASS__
#define __MAINFRAME_CLASS__

#define APPNAME "Kinect3DBuilder"
#include "wx\wx.h"

// to control the main frame user interface
enum IDsMainFrame
{
    // menu items
    K_Quit = wxID_EXIT,
    K_About = wxID_ABOUT
};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame();
	bool OnInit();
	

	// here comes the options processing
	void OnAbout(wxCommandEvent& event);

private:
	wxPanel* panel;
};

#endif // __MAINFRAME_CLASS__