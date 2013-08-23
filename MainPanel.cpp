#include "MainPanel.h"
#include "Main.h"

MainPanel::MainPanel(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxPanel(parent,id,pos,size)
{
	// create context
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(MainPanel::OnPaint));
	this->Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(MainPanel::OnSize));

	// we can assume NO_LOOP just checkin if this is null
	timer = NULL;

	// assuming no Kinect device connected
	kinect = NULL;
	
	colorBuffer = NULL;
	depthBuffer = NULL;
	
	// image to store the data
	wxImage image;

	// temporary stuff
	// load color buffer
	const char* filepath = ShowFileDialog(0,DialogOpen,"PNG Files","*.png");
	if (filepath != NULL)
	{
		/*bool hasAlpha;
		colorBuffer = LoadPng(filepath,sizeColor,hasAlpha);*/
		wxString string = wxString::FromUTF8(filepath);
		image.LoadFile(string,wxBITMAP_TYPE_PNG);
	}
	const char* imagePath = filepath;
	// load depth buffer file
	filepath = ShowFileDialog(0,DialogOpen,".dep files","*.dep*");
	if (filepath != NULL)
	{
		depthBuffer = ReadDepthBuffer(sizeDepth,filepath);
	}
	
	// here ends temporary stuff

	// put image on the panel
	wxStaticBitmap* sbitmap = new wxStaticBitmap(this,wxID_ANY,wxBitmap(image));

}


MainPanel::~MainPanel(void)
{
	if (timer != NULL)
	{
		timer->Stop();
		delete timer;
	}
}

void MainPanel::OnSize(wxSizeEvent &event)
{
	
}

void MainPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// somehow this solve the problem with the render loop (ask the wx guys for more info)
	wxPaintDC(this);
}

void MainPanel::ChangeLoopMode(bool loopMode)
{
	// change loop mode based on parameter
	if (!loopMode)
	{
		if (timer != NULL)
		{
			delete timer;
			timer = NULL;
		}
	}
	else if (loopMode)
	{
		// create timer
		timer = new RenderTimer(this);
		timer->Start();
	}
}



/* RENDERTIMER class */

RenderTimer::RenderTimer(MainPanel* viewport)
{
	// copy pointer to call drawn inside notify
	this->viewport = viewport;
}

void RenderTimer::Start()
{
	wxTimer::Start(10);
}

void RenderTimer::Notify()
{
	// update OpenGL view
	viewport->Refresh(false);
}