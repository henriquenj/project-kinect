/* Simple program that uses the NUI API to render images from Kinect sensor using OpenGL and Glut */

#include <Windows.h>
#include <gl\glut.h>

#include "KinectSensor.h"
#include "ModelBuilder.h"
#include "UtilitiesFunctions.h"

#define WINDOWWIDTH 640
#define WINDOWHEIGHT 480

KinectSensor* kinect = NULL;
ModelBuilder* model = NULL;

// loaded frames
glm::uvec2 sizeColor; // color buffer size
glm::uvec2 sizeDepth; // depth buffer size
BYTE* colorBuffer = NULL;
BYTE* depthBufferToRender = NULL;

void RenderCallback()
{
	Sleep(5);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	// define position
	glRasterPos2i(0,WINDOWHEIGHT-1);

	if (kinect->GetReady())
	{
		// draw on screen buffer from kinect sensor
		glDrawPixels(kinect->GetWidthColor(),kinect->GetHeightColor(),GL_BGRA_EXT,GL_UNSIGNED_BYTE,kinect->GetUnreliableColorBuffer());
		glDrawPixels(kinect->GetWidthDepth(),kinect->GetHeightDepth(),GL_LUMINANCE,GL_UNSIGNED_BYTE,kinect->GetDepthBufferToRender());
	}

	if (colorBuffer != NULL)
	{
		glDrawPixels(sizeColor.x,sizeColor.y,GL_RGBA,GL_UNSIGNED_BYTE,colorBuffer);
	}
	if (depthBufferToRender != NULL)
	{
		glDrawPixels(sizeDepth.x,sizeDepth.y,GL_LUMINANCE,GL_UNSIGNED_BYTE,depthBufferToRender);
	}

	glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	// block window reshape
	glutReshapeWindow(WINDOWWIDTH,WINDOWHEIGHT);

	glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // esc key
		delete kinect;
		exit(0);
		break;
	}
}

void Menu(int option)
{
	const char * filepath;
	if (option == 0)
	{
		if(kinect->GetReady())
		{
			//first get the buffers
			BYTE* colorbuffer = kinect->GetColorBuffer();
			int* depthbuffer = kinect->GetDepthBuffer();

			filepath = ShowFileDialog(0,DialogSave,"PNG Files (*.png)","*.png");
			if (filepath != NULL)
			{
				// swap channel
				BGRAtoRGBA(colorbuffer,kinect->GetWidthColor(),kinect->GetHeightColor());
				//invert image
				colorbuffer = InvertLines(colorbuffer,kinect->GetWidthColor(),kinect->GetHeightColor());
				// dump color buffer to a PNG file
				SavePng(filepath,colorbuffer,kinect->GetWidthColor(),kinect->GetHeightColor());
			}
			filepath = ShowFileDialog(0,DialogSave,".dep files","*.dep*");
			if (filepath != NULL)
			{
				// dump depth buffer to a file
				DumpDepthBuffer(depthbuffer,kinect->GetWidthDepth(),kinect->GetHeightDepth(),filepath);
			}
			delete colorbuffer;
			delete depthbuffer;
		}
		else
		{
			MessageBoxA(0,"Your kinect isn't ready for retrieve frames.", "Error", (MB_OK | MB_ICONEXCLAMATION));
		}
	}
	else if (option == 1)
	{
		// load depth buffer file
		filepath = ShowFileDialog(0,DialogOpen,".dep files","*.dep*");
		if (filepath != NULL)
		{
			int* rawDepthBuffer = ReadDepthBuffer(sizeDepth,filepath);
			if(rawDepthBuffer)
			{
				// convert to render
				depthBufferToRender = new BYTE[sizeDepth.x * sizeDepth.y];
				for (int i = 0; i < sizeDepth.x * sizeDepth.y; i++)
				{
					BYTE l = 255 - (BYTE)(256*rawDepthBuffer[i]/0x0fff);
					depthBufferToRender[i] = l / 2;
				}
				delete rawDepthBuffer;
			}
		}
		// load color buffer
		filepath = ShowFileDialog(0,DialogOpen,"PNG Files","*.png");
		if (filepath != NULL)
		{
			bool hasAlpha;
			colorBuffer = LoadPng(filepath,sizeColor,hasAlpha);
		}
	}
}
void InitApp()
{

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0, WINDOWWIDTH, WINDOWHEIGHT,0,0,1);
	glMatrixMode(GL_MODELVIEW);

	kinect = new KinectSensor(RESOLUTION_1280X1024,RESOLUTION_640X480);
	model = new ModelBuilder(kinect);

	// menus
	int menu = glutCreateMenu(Menu);
	glutAddMenuEntry("Capture Buffers",0);
	glutAddMenuEntry("Load Buffers From File",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void IdleCallback()
{
	glutPostRedisplay();
}
int main()
{
	// setup Glut and OpenGL

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(WINDOWWIDTH,WINDOWHEIGHT);

	glutCreateWindow("OpenGL Kinect");

	InitApp();

	// register callback function
	glutReshapeFunc(ReshapeCallback);
	glutDisplayFunc(RenderCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutIdleFunc(IdleCallback);
	
	
	glutMainLoop();

	return 0;
}