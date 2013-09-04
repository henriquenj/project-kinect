#include "MainPanel.h"
#include "Main.h"

MainPanel::MainPanel(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxPanel(parent,id,pos,size)
{
	// connect events
	this->Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(MainPanel::OnSize));
	this->Connect(K_ShowDepth,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(MainPanel::OnShowDepthBuffer));
	this->Connect(K_LoadImage,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnLoadImageButton));
	this->Connect(K_GenerateModel,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnGenerateModel));
	this->Connect(K_ConnectKinect,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnConnectKinect));
	this->Connect(K_SaveBuffer,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(MainPanel::OnSaveFrame));

	// assuming no Kinect device connected
	kinect = NULL;
	
	// raw buffers
	colorBuffer = NULL;
	depthBuffer = NULL;

	// create render panel
	renderPanel = new RenderPanel(this,wxID_ANY,wxPoint(0,0),wxSize(640,480));

	// create static images to be filled later
	renderPanel->rgbBitmap = new wxStaticBitmap(renderPanel,wxID_ANY,wxBitmap(wxNullBitmap));
	renderPanel->depthBitmap = new wxStaticBitmap(renderPanel,wxID_ANY,wxBitmap(wxNullBitmap));
	renderPanel->depthBitmap->Hide();


	// load image interface area 
	/* Add button */
	loadImageButton = new wxButton(this,K_LoadImage,_("Load Saved Kinect Image"),wxPoint(660,20));
	generateModelButton = new wxButton(this,K_GenerateModel,_("Generate Model"),wxPoint(660,60));
	generateModelButton->Disable();
	// add static box to load image interface portion
	wxStaticBox* loadImageBox = new wxStaticBox(this,wxID_ANY,_("Work with previously saved images"),wxPoint(650,0),wxSize(250,100));

	// work with kinect area
	/* Add buttons */
	connectKinectButton = new wxButton(this,K_ConnectKinect,_("Connect Kinect"),wxPoint(660,160));
	saveBufferButton = new wxButton(this,K_SaveBuffer,_("Save frame"),wxPoint(660,200));
	saveBufferButton->Disable();
	// add statc box to that area
	wxStaticBox* connecKinnectArea = new wxStaticBox(this,wxID_ANY,_("Work with Kinect"),wxPoint(650,140),wxSize(250,130));


	// other
	showDepthButton = new wxCheckBox(this,K_ShowDepth,_("Show Depth Buffer"),wxPoint(660,400));
	showDepthButton->Disable();
	showDepthButton->SetValue(false);
	
}


MainPanel::~MainPanel(void)
{
}

void MainPanel::OnSize(wxSizeEvent &event)
{
	
}

void MainPanel::OnShowDepthBuffer (wxCommandEvent &event)
{
	// this concerns only the loaded images
	if (!AppConfig::GetKinectConnected())
	{
		// turn of or on depth buffer drawning
		if (event.IsChecked())
		{
			renderPanel->depthBitmap->Show();
			renderPanel->rgbBitmap->Hide();
		}
		else
		{
			renderPanel->depthBitmap->Hide();
			renderPanel->rgbBitmap->Show();
		}

		renderPanel->Refresh();
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
			// cancel connection with the kinect if there's one
			if (AppConfig::GetKinectConnected())
			{
				//cancel connection
				AppConfig::SetKinectConnected(false);
				// remove kinect object
				delete kinect;
				// change render loop mode
				renderPanel->ChangeLoopMode(false);
				// and finally change the buttons again
				connectKinectButton->Enable();
				saveBufferButton->Disable();
			}
			

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

			//depthBufferRender = InvertLines(depthBufferRender,sizeDepth.x,sizeDepth.y,3);
			depthFrame.Create(sizeDepth.x,sizeDepth.y,depthBufferRender);
			depthFrame = depthFrame.Rotate(3.14,wxPoint(depthFrame.GetWidth()/2,depthFrame.GetHeight()/2));

			// show on the screen
			renderPanel->depthBitmap->SetBitmap(depthFrame);
			renderPanel->rgbBitmap->SetBitmap(rgbFrame);
			renderPanel->rgbBitmap->Show();
			// NO MEMORY LEAKS!
			//delete depthBufferRender;

			loadedImagePath = filepathRGB;
			showDepthButton->Enable();
			generateModelButton->Enable();

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
	generateModelButton->Disable();
	connectKinectButton->Disable();
	// enable this
	showDepthButton->Enable();
	saveBufferButton->Enable();

	// disable staticbitmaps
	renderPanel->rgbBitmap->Hide();
	renderPanel->depthBitmap->Hide();

	// continue to init routine
	renderPanel->ChangeLoopMode(true);

	// set global option
	AppConfig::SetKinectConnected(true);
}

void MainPanel::OnSaveFrame(wxCommandEvent& WXUNUSED(event))
{
	// save current frame on disk, first the rgb file and them the depth file
	//first get the buffers
	BYTE* colorbuffer_l = kinect->GetColorBuffer();
	short* depthbuffer_l = kinect->GetDepthBuffer();
	
	const char* filepath = ShowFileDialog(0,DialogSave,"PNG Files (*.png)","*.png");
	if (filepath != NULL)
	{
		// convert to image
		wxImage rgbFrame;
		rgbFrame.Create(kinect->GetWidthColor(),kinect->GetHeightColor(),colorbuffer_l);
		//colorbuffer = InvertLines(colorbuffer,kinect->GetWidthColor(),kinect->GetHeightColor(),4);
		// dump color buffer to a PNG file
		std::string filepath_str(filepath);
		AddExtensionWithChecking(filepath_str,std::string("png"),std::string("png"));
		// dump to hard drive
		rgbFrame.SaveFile(wxString(filepath_str.c_str(), wxConvUTF8));
	}
	filepath = ShowFileDialog(0,DialogSave,".dep files","*.dep*");
	if (filepath != NULL)
	{
		std::string filepath_str(filepath);
		AddExtensionWithChecking(filepath_str,std::string("dep"),std::string("dep"));
		// dump depth buffer to a file
		DumpDepthBuffer(depthbuffer_l,kinect->GetWidthDepth(),kinect->GetHeightDepth(),filepath_str.c_str());
	}
	delete colorbuffer_l;
	delete depthbuffer_l;
}



