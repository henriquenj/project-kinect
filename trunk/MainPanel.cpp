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
	this->Connect(K_GenerateModel,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnGenerateModel));
	this->Connect(K_ConnectKinect,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnConnectKinect));

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


	// load image interface area 
	/* Add button */
	loadImageButton = new wxButton(this,K_LoadImage,_("Load Saved Kinect Image"),wxPoint(660,20));
	generateModel = new wxButton(this,K_GenerateModel,_("Generate Model"),wxPoint(660,90));
	generateModel->Disable();
	// add static box to load image interface portion
	wxStaticBox* loadImageBox = new wxStaticBox(this,wxID_ANY,_("Work with previously saved images"),wxPoint(650,0),wxSize(250,130));

	// work with kinect area
	/* Add buttons */
	connectKinect = new wxButton(this,K_ConnectKinect,_("Connect Kinect"),wxPoint(660,160));
	// add statc box to that area
	wxStaticBox* connecKinnectArea = new wxStaticBox(this,wxID_ANY,_("Work with Kinect"),wxPoint(650,140),wxSize(250,130));


	// other
	showDepthButton = new wxCheckBox(this,K_ShowDepth,_("Show Depth Buffer"),wxPoint(660,400));
	showDepthButton->Disable();
	showDepthButton->SetValue(false);
	
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
	wxPaintDC dc(this);

	// if the kinnect is connected, show stream
	if (AppConfig::GetKinectConnected)
	{
		// update frame
		wxImage rgbFrame;
		wxImage depthFrame;

		// ask the kinect the new frame
		rgbFrame.Create(kinect->GetWidthColor(),kinect->GetHeightColor(),kinect->GetUnreliableColorBuffer());
		// send to bitmap
		rgbBitmap->SetBitmap(rgbFrame);
	}
}

void MainPanel::OnShowDepthBuffer
	(wxCommandEvent &event)
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

			// delete previously loaded buffer
			if (depthBuffer != NULL)
			{
				delete depthBuffer;
			}
			// set global opton
			AppConfig::SetLoadedImageOption(true);

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

			loadedImagePath = filepathRGB;
			showDepthButton->Enable();
			generateModel->Enable();

			this->Refresh();
		}	
	}

}

void MainPanel::OnGenerateModel(wxCommandEvent& WXUNUSED(event))
{
	ModelBuilder builder;
	builder.GeneratePoints(depthBuffer,sizeDepth);
	const char * filepath = ShowFileDialog(0,DialogSave,"OBJ Files","*.obj");
	if (filepath != NULL)
	{
		std::string path(filepath);
		builder.WriteModelOnFile(path,std::string(loadedImagePath.mb_str()));
	}
}

void MainPanel::OnConnectKinect(wxCommandEvent& WXUNUSED(event))
{
	// tries to connect kinect, if it failed for some reason, return
	kinect = new KinectSensor(RESOLUTION_1280X960,RESOLUTION_640X480);
	if (!kinect->GetReady())
	{
		delete kinect;
		return;
	}

	// delete previous buffers
	delete depthBuffer;
	delete colorBuffer;

	// disable other interface
	loadImageButton->Disable();
	generateModel->Disable();
	connectKinect->Disable();
	// enable this
	showDepthButton->Enable();

	// continue to init routine
	this->ChangeLoopMode(true);

	// set global option
	AppConfig::SetKinectConnected(true);
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
	wxTimer::Start(30);
}

void RenderTimer::Notify()
{
	// update panel view
	viewport->Refresh(false);
}