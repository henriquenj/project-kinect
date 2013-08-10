#include "GLViewport.h"


GLViewport::GLViewport(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxGLCanvas(parent,id,pos,size)
{
	// create context
	m_context = new wxGLContext(this);
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(GLViewport::OnPaint));

	// we can assume NO_LOOP just checkin if this is null
	timer = NULL;
}


GLViewport::~GLViewport(void)
{
	delete m_context;
	timer->Stop();
	delete timer;
}

void GLViewport::OnPaint(wxPaintEvent& WXUNUSED(event))
{

	// somehow this solve the problem with the render loop (ask the wx guys for more info)
	wxPaintDC dc(this);
	
	// set the current OpenGL state machine
	this->SetCurrent();

	 // white background
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);
    glVertex3f(10,0,0);
    glVertex3f(10,10,0);
    glVertex3f(0,10,0);
    glEnd();
	

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