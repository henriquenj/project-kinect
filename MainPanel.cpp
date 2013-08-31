#include "MainPanel.h"
#include "Main.h"

MainPanel::MainPanel(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxPanel(parent,id,pos,size)
{
	// connect events
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(MainPanel::OnPaint));
	this->Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(MainPanel::OnSize));
	this->Connect(K_ShowDepth,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(MainPanel::OnShowDepthBuffer));
	this->Connect(K_LoadImage,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnLoadImageButton));

	// we can assume NO_LOOP just checkin if this is null
	timer = NULL;

	// assuming no Kinect device connected
	kinect = NULL;
	
	// raw buffers
	colorBuffer = NULL;
	depthBuffer = NULL;

	// create static images to be filled later
	rgbBitmap = new wxStaticBitmap(this,wxID_ANY,wxBitmap(wxNullBitmap));
	depthBitmap = new wxStaticBitmap(this,wxID_ANY,wxBitmap(wxNullBitmap));
	depthBitmap->Hide();

	/* Add button */
	loadImageButton = new wxButton(this,K_LoadImage,_("Load Saved Kinect Image"),wxPoint(700,0));
	showDepthButton = new wxCheckBox(this,K_ShowDepth,_("Show Depth Buffer"),wxPoint(700,40));
	showDepthButton->Disable();
	showDepthButton->SetValue(false);
	
	// sizer stuff
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(rgbBitmap,wxSizerFlags().Border(wxALL, 7));
	mainSizer->Add(depthBitmap,wxSizerFlags().Border(wxALL, 7));
	mainSizer->Add(loadImageButton, wxSizerFlags().Border(wxALL, 7));

	this->SetAutoLayout(true);
	this->SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);
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

void MainPanel::OnLoadImageButton(wxCommandEvent& WXUNUSED(event))
{
	// the two images must be select to that work
	wxString filepathRGB = wxString::FromUTF8(ShowFileDialog(0,DialogOpen,"PNG Files","*.png"));
	if (!filepathRGB.IsEmpty())
	{
		wxString filepathDepth = wxString::FromUTF8(ShowFileDialog(0,DialogOpen,".dep files","*.dep*"));
		if (!filepathDepth.IsEmpty())
		{

			// set global opton
			AppConfig::SetKinectConnected(true);

			wxImage rgbFrame;
			wxImage depthFrame;

			rgbFrame.LoadFile(filepathRGB,wxBITMAP_TYPE_PNG);
			//use half of the size
			rgbFrame.Rescale(rgbFrame.GetWidth()/2,rgbFrame.GetHeight()/2);


			// load depth buffer file
			depthBuffer = ReadDepthBuffer(sizeDepth,filepathDepth.mb_str());
			// find highest point
			int highestPoint = FindMaxValue(depthBuffer,sizeDepth.x * sizeDepth.y);

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

			// show on the screen
			depthBitmap->SetBitmap(depthFrame);
			rgbBitmap->SetBitmap(rgbFrame);
			// NO MEMORY LEAKS!
			delete depthBufferRender;
			delete depthBuffer;

			showDepthButton->Enable();

			this->Refresh();
		}	
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
	// update panel view
	viewport->Refresh(false);
}