#ifndef __ABOUT_DIALOG__
#define __ABOUT_DIALOG__

#include "wx\wx.h"
#include "Main.h"

class AboutDialog : public wxDialog
{
public:
		AboutDialog(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
						const wxString &title = _("Kinect3DBuilder"), /*TODO: put APPNAME macro here */
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_DIALOG_STYLE);

};

#endif // __ABOUT_DIALOG__