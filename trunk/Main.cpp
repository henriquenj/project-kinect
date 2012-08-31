/* Simple program that uses the NUI API to render images from Kinect sensor using OpenGL and Glut */

#include <Windows.h>
#include <gl\glut.h>

#include "KinectSensor.h"

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 1024

KinectSensor* kinect;

void RenderCallback()
{
	Sleep(5);
	glClear(GL_COLOR_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();


	// define position
	//glRasterPos2i(-1,-1);
	//glRasterPos2f(-1.0f,-1.0f);
	glRasterPos2i(0,WINDOWHEIGHT);

	// get kinect buffer
	//BYTE* colorBuffer = kinect->GetColorBuffer();
	// draw on screen buffer from kinect sensor
	glDrawPixels(WINDOWWIDTH,WINDOWHEIGHT,GL_BGRA_EXT,GL_UNSIGNED_BYTE,kinect->GetUnreliableColorBuffer());

	//delete colorBuffer;

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
		exit(0);
		break;
	}
}

void InitApp()
{

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0, WINDOWWIDTH, WINDOWHEIGHT,0,0,1);
	glMatrixMode(GL_MODELVIEW);


	kinect = new KinectSensor(RESOLUTION_1280X1024);
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