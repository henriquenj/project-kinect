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
BYTE* image = NULL;
glm::uvec2 size;

void RenderCallback()
{
	Sleep(5);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// define position
	glRasterPos2i(0,WINDOWHEIGHT-1);

	// draw on screen buffer from kinect sensor
	//glDrawPixels(kinect->GetWidthColor(),kinect->GetHeightColor(),GL_BGRA_EXT,GL_UNSIGNED_BYTE,kinect->GetUnreliableColorBuffer());


	//glDrawPixels(kinect->GetWidthDepth(),kinect->GetHeightDepth(),GL_LUMINANCE,GL_BYTE,kinect->GetDepthBufferToRender());
	glDrawPixels(size.x,size.y,GL_RGB,GL_UNSIGNED_BYTE,image);
	
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
	if (option == 0)
	{
		// call generate points to build a model
		model->GeneratePoints();
		// ask to the user to point a file 
		std::string s_filename;
		const char * filename = ShowFileDialog(0,DialogSave,"OBJ Files (*.obj)","*.obj");
		if (filename != NULL) {s_filename = filename;}
		// write on a file
		model->WriteModelOnFile(s_filename);
	}
}
void InitApp()
{

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0, WINDOWWIDTH, WINDOWHEIGHT,0,0,1);
	glMatrixMode(GL_MODELVIEW);

	//kinect = new KinectSensor(RESOLUTION_640X480,RESOLUTION_640X480);
	//model = new ModelBuilder(kinect);

	const char * filename = ShowFileDialog(0,DialogOpen,"PNG Files (*.png)","*.png");
	if (filename == NULL){exit(0);}
	else
	{
		//load png file
		image = LoadPng(filename,size);
	}

	// menus
	int menu = glutCreateMenu(Menu);
	glutAddMenuEntry("Generate Model",0);
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