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
	generateModel = new wxButton(this,K_GenerateModel,_("Generate Model"),wxPoint(660,60));
	generateModel->Disable();
	// add static box to load image interface portion
	wxStaticBox* loadImageBox = new wxStaticBox(this,wxID_ANY,_("Work with previously saved images"),wxPoint(650,0),wxSize(250,100));

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

			// show on the screen
			renderPanel->depthBitmap->SetBitmap(depthFrame);
			renderPanel->rgbBitmap->SetBitmap(rgbFrame);
			// NO MEMORY LEAKS!
			delete depthBufferRender;

			loadedImagePath = filepathRGB;
			showDepthButton->Enable();
			generateModel->Enable();

			this->Refresh();
			renderPanel->Refresh();
			
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

	// disable staticbitmaps
	renderPanel->rgbBitmap->Hide();
	renderPanel->depthBitmap->Hide();

	// continue to init routine
	renderPanel->ChangeLoopMode(true);

	// set global option
	AppConfig::SetKinectConnected(true);
}



