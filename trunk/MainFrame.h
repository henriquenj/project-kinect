#ifndef __MAINFRAME_CLASS__
#define __MAINFRAME_CLASS__

#include "Main.h"
#include "wx\wx.h"
#include "GLViewport.h"
#include "AboutDialog.h"
#include "AppConfig.h"

class PanelMainFrame;

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
	PanelMainFrame* panel;
};


// class that represents the panel inside the main frame of the application
class PanelMainFrame : public wxPanel
{
public:
	PanelMainFrame(wxWindow* parent,wxSize size);

	// callback function to deal with resize events
	void OnSize(wxSizeEvent& event);
	
private:
	// viewport to draw the graphics
	GLViewport* viewport;
};

#endif // __MAINFRAME_CLASS__