#ifndef __GLVIEWPORT_CLASS__
#define __GLVIEWPORT_CLASS__

#include "wx\wx.h"
#include "wx\glcanvas.h"


class RenderTimer;

/* GLViewport implements the glcanvas interface provided by wxWidgets */
class GLViewport : public wxGLCanvas
{
public:
	GLViewport(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~GLViewport();


	// render function
	void OnPaint(wxPaintEvent &event);
	
	/* change loop mode: true for looping and false for non-loopig
		the default is FALSE
	*/
	void ChangeLoopMode(bool loopMode);

private:
	// context for OpenGL commands
	wxGLContext* m_context;
	// our timer
	RenderTimer* timer;
};



/*RenderTimer class implements the timer which will control the render loop */
class RenderTimer : public wxTimer
{
public:
	RenderTimer(GLViewport* viewport);
	//start the timer with interval of 10 miliseconds
	void Start();
	// functiona callback to becalled upon need to redraw
	void Notify();
private:
	GLViewport* viewport;
};



#endif //__GLVIEWPORT_CLASS__