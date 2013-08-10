#ifndef __GLVIEWPORT_CLASS__
#define __GLVIEWPORT_CLASS__

#include "wx\wx.h"
#include "wx\glcanvas.h"

// GLViewport implements the glcanvas interface provided by wxWidgets
class GLViewport : public wxGLCanvas
{
public:
	GLViewport(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~GLViewport();


	// render function
	void OnPaint(wxPaintEvent &event);

private:
	// context for OpenGL commands
	wxGLContext* m_context;
};



#endif //__GLVIEWPORT_CLASS__