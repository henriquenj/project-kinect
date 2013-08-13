#include "GLViewport.h"
#include "Main.h"

GLViewport::GLViewport(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxGLCanvas(parent,id,pos,size)
{
	// create context
	m_context = new wxGLContext(this);
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(GLViewport::OnPaint));

	// we can assume NO_LOOP just checkin if this is null
	timer = NULL;

	// assuming no Kinect device connected
	kinect = NULL;

	colorBuffer = NULL;
	depthBuffer = NULL;

	// temporary stuff
	// load color buffer
	const char* filepath = ShowFileDialog(0,DialogOpen,"PNG Files","*.png");
	if (filepath != NULL)
	{
		bool hasAlpha;
		colorBuffer = LoadPng(filepath,sizeColor,hasAlpha);
	}
	const char* imagePath = filepath;
	// load depth buffer file
	filepath = ShowFileDialog(0,DialogOpen,".dep files","*.dep*");
	if (filepath != NULL)
	{
		depthBuffer = ReadDepthBuffer(sizeDepth,filepath);
	}

	// here ends temporary stuff


	/* Init OpenGL stuff*/
	this->SetCurrent();
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0, WINDOWWIDTH, WINDOWHEIGHT,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glPointSize(5);
}


GLViewport::~GLViewport(void)
{
	delete m_context;
	if (timer != NULL)
	{
		timer->Stop();
		delete timer;
	}
}

void GLViewport::OnPaint(wxPaintEvent& WXUNUSED(event))
{

	// somehow this solve the problem with the render loop (ask the wx guys for more info)
	wxPaintDC dc(this);

	// set the current OpenGL state machine
	this->SetCurrent();

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// define position
	glRasterPos2i(0,WINDOWHEIGHT-1);

	if (AppConfig::GetKinectConnected())
	{
		// draw on screen buffer from kinect sensor
		glDrawPixels(kinect->GetWidthColor(),kinect->GetHeightColor(),GL_BGRA_EXT,GL_UNSIGNED_BYTE,kinect->GetUnreliableColorBuffer());
		glDrawPixels(kinect->GetWidthDepth(),kinect->GetHeightDepth(),GL_LUMINANCE,GL_SHORT,kinect->GetUnreliableDepthBuffer());
	}

	if (colorBuffer != NULL)
	{
		glDrawPixels(sizeColor.x,sizeColor.y,GL_RGBA,GL_UNSIGNED_BYTE,colorBuffer);
	}
	if (depthBuffer != NULL)
	{
		glDrawPixels(sizeDepth.x,sizeDepth.y,GL_LUMINANCE,GL_SHORT,depthBuffer);
	}

	//bProcess->DrawMarkers();

	glFlush();

	this->SwapBuffers();
}

void GLViewport::ChangeLoopMode(bool loopMode)
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

RenderTimer::RenderTimer(GLViewport* viewport)
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