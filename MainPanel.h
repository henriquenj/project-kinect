#ifndef __MAINPANEL_CLASS__
#define __MAINPANEL_CLASS__

#include "wx\wx.h"
#include "wx\glcanvas.h"
#include "wx\wfstream.h"
#include "KinectSensor.h"
#include "glm\glm.hpp"
#include "UtilitiesFunctions.h"
#include "ModelBuilder.h"
#include "RenderPanel.h"

/* MainPanel implements the panel interface that'll control most part of the interface */
class MainPanel : public wxPanel
{
public:
	MainPanel(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~MainPanel();
	
	// callback functions
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
	// our friendly render panel
	RenderPanel* renderPanel;
	// instance of kinect sensor, default is NULL (no device connected)
	KinectSensor* kinect;
	// loaded frames
	glm::uvec2 sizeColor; // color buffer size
	glm::uvec2 sizeDepth; // depth buffer size
	BYTE* colorBuffer;
	short* depthBuffer;
	wxString loadedImagePath; // the path of currently loaded picture

	// buttons of the interface
	wxButton* loadImageButton;
	wxCheckBox* showDepthButton;
	wxButton* generateModel;
	wxButton* connectKinect;

	friend class RenderPanel;

};





#endif //__MAINPANEL_CLASS__