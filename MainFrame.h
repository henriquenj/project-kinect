#ifndef __MAINFRAME_CLASS__
#define __MAINFRAME_CLASS__

#include "Main.h"
#include "wx\wx.h"
#include "AboutDialog.h"


// main frame of the application
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