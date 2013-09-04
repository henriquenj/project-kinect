#include "RenderPanel.h"
#include "MainPanel.h"

RenderPanel::RenderPanel(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxPanel(parent,id,pos,size)
{
	// connect events
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(RenderPanel::OnPaint));
	

	this->SetDoubleBuffered(true);

	timer = NULL;
}


RenderPanel::~RenderPanel(void)
{
	if (timer != NULL)
	{
		timer->Stop();
		delete timer;
	}
}

void RenderPanel::ChangeLoopMode(bool loopMode)
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
	else
	{
		// create timer
		timer = new RenderTimer(this);
		timer->Start();
	}
}

void RenderPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// somehow this solve the problem with the render loop (ask the wx guys for more info)
	wxPaintDC dc(this);

	// if the kinect is connected, draw kinect input
	if (AppConfig::GetKinectConnected())
	{

		// get kinect object
		KinectSensor* kinect = ((MainPanel*)this->GetParent())->kinect;
		bool showDepth = ((MainPanel*)this->GetParent())->showDepthButton->GetValue();

		// create images
		wxImage finalFrame;// can be either rgb frame or the depth frame


		// only show rgb stream if we aren't showing the depth frame
		if (!showDepth)
		{
			finalFrame.Create(kinect->GetWidthColor(),kinect->GetHeightColor(),kinect->GetColorBuffer());
			//use half of the size
			finalFrame = finalFrame.Rescale(finalFrame.GetWidth()/2,finalFrame.GetHeight()/2);
		}
		else
		{
			// create depth frame object
			// convert depth frame to RGB format
			short* depthBuffer = kinect->GetDepthBuffer();
			BYTE* depthFrameRGB = new BYTE[kinect->GetWidthDepth() * kinect->GetHeightDepth() * 3];

			// find highest point
			int highestPoint = FindMaxValue(depthBuffer,kinect->GetWidthDepth() * kinect->GetHeightDepth());

			for (int a = 0, b = 0; b < kinect->GetWidthDepth() * kinect->GetHeightDepth();a+=3,b++)
			{
				unsigned char value = (depthBuffer[b] * 255) / highestPoint;
				depthFrameRGB[a] = value;
				depthFrameRGB[a+1] = value;
				depthFrameRGB[a+2] = value;
			}

			delete depthBuffer;
			finalFrame.Create(kinect->GetWidthDepth(),kinect->GetHeightDepth(),depthFrameRGB);
			finalFrame = finalFrame.Rotate(3.14,wxPoint(finalFrame.GetWidth()/2,finalFrame.GetHeight()/2));
		}
		// put on the screen
		dc.DrawBitmap(finalFrame,wxPoint(0,0));
	}
}



/* RENDERTIMER class */

RenderTimer::RenderTimer(RenderPanel* viewport)
{
	// copy pointer to call drawn inside notify
	this->viewport = viewport;
}

void RenderTimer::Start()
{
	wxTimer::Start(50);
}

void RenderTimer::Notify()
{
	// update panel view
	viewport->Refresh(true);
}