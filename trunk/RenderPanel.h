#ifndef __RENDERPANEL_CLASS__
#define __RENDERPANEL_CLASS__


#include "wx\wx.h"
#include "AppConfig.h"

class RenderTimer;

// class that'll control the render loop separeted from the interface
class RenderPanel : public wxPanel
{
public:
	RenderPanel(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	~RenderPanel(void);

	/* change loop mode: true for looping and false for non-loopig
		the default is FALSE*/
	void ChangeLoopMode(bool loopMode);

	// callback function
	// on paint is called at each frame
	void OnPaint(wxPaintEvent &event);

private:
	// our timer
	RenderTimer* timer;

	// static bitmap will render the loaded image on the screen
	wxStaticBitmap* rgbBitmap;
	wxStaticBitmap* depthBitmap;

	friend class MainPanel; // make communication easier
};


/*RenderTimer class implements the timer which will control the render loop */
class RenderTimer : public wxTimer
{
public:
	RenderTimer(RenderPanel* viewport);
	//start the timer with interval of 10 miliseconds
	void Start();
	// functiona callback to becalled upon need to redraw
	void Notify();
private:
	RenderPanel* viewport;
};


#endif //__RENDERPANEL_CLASS__