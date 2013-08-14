#ifndef __GLVIEWPORT_CLASS__
#define __GLVIEWPORT_CLASS__

#include "wx\wx.h"
#include "wx\glcanvas.h"
#include "KinectSensor.h"
#include "glm\glm.hpp"
#include "UtilitiesFunctions.h"


class RenderTimer;

/* GLViewport implements the glcanvas interface provided by wxWidgets */
class GLViewport : public wxGLCanvas
{
public:
	GLViewport(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~GLViewport();
	
	// callback functions
	// render function
	void OnPaint(wxPaintEvent &event);
	// called each time someone resize the window
	void OnSize(wxSizeEvent &event);
	/* change loop mode: true for looping and false for non-loopig
		the default is FALSE
	*/
	void ChangeLoopMode(bool loopMode);

private:
	// context for OpenGL commands
	wxGLContext* m_context;
	// our timer
	RenderTimer* timer;
	// instance of kinect sensor, default is NULL (no device connected)
	KinectSensor* kinect;

	// loaded frames
	glm::uvec2 sizeColor; // color buffer size
	glm::uvec2 sizeDepth; // depth buffer size
	BYTE* colorBuffer;
	short* depthBuffer;
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