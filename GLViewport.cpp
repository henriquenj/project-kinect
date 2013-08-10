#include "GLViewport.h"


GLViewport::GLViewport(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
		:wxGLCanvas(parent,id,pos,size)
{
	// create context
	m_context = new wxGLContext(this);
	this->Connect(wxID_ANY,wxEVT_PAINT,wxPaintEventHandler(GLViewport::OnPaint));
	// TODO: PREVENT SUPER LOOP
}


GLViewport::~GLViewport(void)
{
	delete m_context;
}

void GLViewport::OnPaint(wxPaintEvent& WXUNUSED(event))
{

	// somehow this solve the problem with the render loop
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