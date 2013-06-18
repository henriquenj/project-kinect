#ifndef __MAINFRAME_CLASS__
#define __MAINFRAME_CLASS__

#include "wx\wx.h"
class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	bool OnInit();
};

#endif // __MAINFRAME_CLASS__