#include "MainPanel.h"
#include "Main.h"

MainPanel::MainPanel(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxPanel(parent,id,pos,size)
{
	// connect events
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(MainPanel::OnPaint));
	this->Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(MainPanel::OnSize));
	this->Connect(K_ShowDepth,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(MainPanel::OnShowDepthBuffer));

	// we can assume NO_LOOP just checkin if this is null
	timer = NULL;

	// assuming no Kinect device connected
	kinect = NULL;
	
	colorBuffer = NULL;
	depthBuffer = NULL;
	
	// image to store the data
	wxImage rgbFrame;
	wxImage depthFrame;


	// TEMPORARY stuff
	// load color buffer
	const char* filepath = ShowFileDialog(0,DialogOpen,"PNG Files","*.png");
	if (filepath != NULL)
	{
		/*bool hasAlpha;
		colorBuffer = LoadPng(filepath,sizeColor,hasAlpha);*/
		wxString string = wxString::FromUTF8(filepath);
		rgbFrame.LoadFile(string,wxBITMAP_TYPE_PNG);
	}
	const char* imagePath = filepath;
	// load depth buffer file
	filepath = ShowFileDialog(0,DialogOpen,".dep files","*.dep*");
	if (filepath != NULL)
	{
		depthBuffer = ReadDepthBuffer(sizeDepth,filepath);
		// find highest
		int highestPoint = 0;
		for (int a = 0; a < sizeDepth.x * sizeDepth.y;a++)
		{
			if (depthBuffer[a] > highestPoint)
			{
				highestPoint = depthBuffer[a];
			}
		}
		unsigned char* depthBufferRender = (unsigned char*)malloc(sizeDepth.x * sizeDepth.y * 3);
		// convert depth buffer to an drawnable format
		for (int a = 0, b = 0; a < sizeDepth.x * sizeDepth.y; a++,b+=3 )
		{
			unsigned char value = (depthBuffer[a] * 255) / highestPoint;
			// compute new value

			depthBufferRender[b] = value;
			depthBufferRender[b+1] = value;
			depthBufferRender[b+2] = value;
		}
		depthBufferRender = InvertLines(depthBufferRender,sizeDepth.x,sizeDepth.y,3);
		depthFrame.Create(sizeDepth.x,sizeDepth.y,depthBufferRender);
	}
	
	// here ends TEMPORARY stuff

	// put image on the panel
	//use half of the size
	rgbFrame.Rescale(rgbFrame.GetWidth()/2,rgbFrame.GetHeight()/2);
	rgbBitmap = new wxStaticBitmap(this,wxID_ANY,wxBitmap(rgbFrame));
	depthBitmap = new wxStaticBitmap(this,wxID_ANY,wxBitmap(depthFrame));

	/* Add button */
	wxButton* loadImageButton = new wxButton(this,K_LoadImage,_("Load Saved Kinect Image"),wxPoint(rgbFrame.GetWidth(),0));
	wxCheckBox* showDepth = new wxCheckBox(this,K_ShowDepth,_("Show Depth Buffer"),wxPoint(rgbFrame.GetWidth(),40));
	showDepth->SetValue(true);
	
	// sizer stuff
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(rgbBitmap,0,wxALL);
	sizer->Add(depthBitmap);
	sizer->Add(loadImageButton);

	this->SetAutoLayout(true);
	this->SetSizer(sizer);

	sizer->SetSizeHints(this);

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

void MainPanel::OnShowDepthBuffer(wxCommandEvent &event)
{
	// turn of or on depth buffer drawning
	if (event.IsChecked())
	{
		depthBitmap->Show();
	}
	else
	{
		depthBitmap->Hide();
	}
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