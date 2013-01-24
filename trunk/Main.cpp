/* Simple program that uses the NUI API to render images from Kinect sensor using OpenGL and Glut */

#include <Windows.h>
#include <gl\glut.h>

#include "KinectSensor.h"
#include "ModelBuilder.h"
#include "BufferProcess.h"
#include "UtilitiesFunctions.h"

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 960

KinectSensor* kinect = NULL;
BufferProcess* bProcess = NULL;
ModelBuilder* builder = NULL;

// loaded frames
glm::uvec2 sizeColor; // color buffer size
glm::uvec2 sizeDepth; // depth buffer size
BYTE* colorBuffer = NULL;
short* depthBuffer = NULL;

// markers
std::vector<glm::uvec2> markers;

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

	// draw markers
	glBegin(GL_POINTS);
	for (int p = 0; p < markers.size(); p++)
	{
		glVertex2i(markers[p].x,markers[p].y);
	}
	glEnd();


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
			short* depthbuffer = kinect->GetDepthBuffer();

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
			MessageBoxA(0,"Your kinect isn't ready to retrieve frames.", "Error", (MB_OK | MB_ICONEXCLAMATION));
		}
	}
	else if (option == 1)
	{
		// load color buffer
		filepath = ShowFileDialog(0,DialogOpen,"PNG Files","*.png");
		if (filepath != NULL)
		{
			bool hasAlpha;
			colorBuffer = LoadPng(filepath,sizeColor,hasAlpha);
		}
		// load depth buffer file
		filepath = ShowFileDialog(0,DialogOpen,".dep files","*.dep*");
		if (filepath != NULL)
		{
			depthBuffer = ReadDepthBuffer(sizeDepth,filepath);
		}
	}
	else if (option == 2)
	{
		if (colorBuffer == NULL || depthBuffer == NULL)
		{
			// error! one or two buffers aren't loaded!
			MessageBoxA(0,"Cannot process buffers. There is no loaded buffer!","Error",(MB_OK | MB_ICONEXCLAMATION));
		}
		else
		{
			// check if there are enough markers
			if (markers.size() == 0)
			{
				MessageBoxA(0,"You must mark at least one marker","Error",(MB_OK | MB_ICONEXCLAMATION));
			}
			else
			{
				// now execute the actual processing
				BYTE* tempColor;
				tempColor = bProcess->CategorizeObjects(colorBuffer,sizeColor.x * sizeColor.y * 4,markers.size());
				delete colorBuffer;
				colorBuffer = tempColor;
			}
		}
	}
	else if (option == 3)
	{
		if (depthBuffer)
		{
			builder->GeneratePoints(depthBuffer,sizeDepth);
			const char * filepath = ShowFileDialog(0,DialogSave,"OBJ Files","*.obj");
			std::string path(filepath);
			builder->WriteModelOnFile(path);
		}
		else
		{
			MessageBoxA(0,"Cannot process buffers. There is no loaded buffer!","Error",(MB_OK | MB_ICONEXCLAMATION));
		}
	}
}
void InitApp()
{

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0, WINDOWWIDTH, WINDOWHEIGHT,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glPointSize(5);

	kinect = new KinectSensor(RESOLUTION_1280X960,RESOLUTION_640X480);
	bProcess = new BufferProcess();
	builder = new ModelBuilder();

	// menus
	int menu = glutCreateMenu(Menu);
	glutAddMenuEntry("Capture Buffers",0);
	glutAddMenuEntry("Load Buffers From File",1);
	glutAddMenuEntry("Process buffers",2);
	glutAddMenuEntry("Generate model",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void MouseCallback(int button, int state, int x, int y)
{
	// process click events
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (colorBuffer == NULL)
		{
			// error!
			MessageBoxA(0,"Cannot add marker. There is no loaded buffer!","Error",(MB_OK | MB_ICONEXCLAMATION));
		}
		else
		{
			glm::uvec2 tMarker(x,y);
			// upon click, create a marker
			markers.push_back(tMarker);
			// add in the process buffer
			bProcess->AddMarker(tMarker);
		}
	}
	
}

void IdleCallback()
{
	glutPostRedisplay();
}
int main()
{

	// setup this for pseudo-random number generatiom
	srand(time(NULL));

	// setup Glut and OpenGL

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(WINDOWWIDTH,WINDOWHEIGHT);

	glutCreateWindow("OpenGL Kinect");

	InitApp();

	// register callback function
	glutReshapeFunc(ReshapeCallback);
	glutDisplayFunc(RenderCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutMouseFunc(MouseCallback);
	glutIdleFunc(IdleCallback);
	
	
	glutMainLoop();

	return 0;
}