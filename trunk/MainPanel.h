#ifndef __MAINPANEL_CLASS__
#define __MAINPANEL_CLASS__

#include "wx\wx.h"
#include "wx\glcanvas.h"
#include "wx\wfstream.h"
#include "KinectSensor.h"
#include "glm\glm.hpp"
#include "UtilitiesFunctions.h"
#include "ModelBuilder.h"


class RenderTimer;

/* MainPanel implements the panel interface that'll control the loop */
class MainPanel : public wxPanel
{
public:
	MainPanel(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~MainPanel();

	/* change loop mode: true for looping and false for non-loopig
		the default is FALSE*/
	void ChangeLoopMode(bool loopMode);
	
	// callback functions
	// render function
	void OnPaint(wxPaintEvent &event);
	// called each time someone resize the window
	void OnSize(wxSizeEvent &event);
	// event functions
	// turn of depth buffer drawning
	void OnShowDepthBuffer(wxCommandEvent &event);
	// called when load button is pressed
	void OnLoadImageButton(wxCommandEvent &event);
	// called to generate model
	void OnGenerateModel(wxCommandEvent &event);
	// called to change loop mode and transmit input from kinect
	void OnConnectKinect(wxCommandEvent &event);

private:
	// our timer
	RenderTimer* timer;
	// instance of kinect sensor, default is NULL (no device connected)
	KinectSensor* kinect;
	// loaded frames
	glm::uvec2 sizeColor; // color buffer size
	glm::uvec2 sizeDepth; // depth buffer size
	BYTE* colorBuffer;
	short* depthBuffer;
	wxString loadedImagePath; // the path of currently loaded picture

	// static bitmap will render the loaded image on the screen
	wxStaticBitmap* rgbBitmap;
	wxStaticBitmap* depthBitmap;

	// buttons of the interface
	wxButton* loadImageButton;
	wxCheckBox* showDepthButton;
	wxButton* generateModel;
	wxButton* connectKinect;

};

/*RenderTimer class implements the timer which will control the render loop */
class RenderTimer : public wxTimer
{
public:
	RenderTimer(MainPanel* viewport);
	//start the timer with interval of 10 miliseconds
	void Start();
	// functiona callback to becalled upon need to redraw
	void Notify();
private:
	MainPanel* viewport;
};



#endif //__MAINPANEL_CLASS__