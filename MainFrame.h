#ifndef __MAINFRAME_CLASS__
#define __MAINFRAME_CLASS__

#include "Main.h"
#include "wx\wx.h"
#include "GLViewport.h"
#include "AboutDialog.h"
#include "AppConfig.h"


// main frame of the application
class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual ~MainFrame();
	bool OnInit();
	

	// here comes the options processing
	void OnAbout(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);

private:
	// panel when everything will be put
	wxPanel* panel;
	// viewport to draw the graphics
	GLViewport* viewport;
};

#endif // __MAINFRAME_CLASS__