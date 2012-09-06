/* Simple program that uses the NUI API to render images from Kinect sensor using OpenGL and Glut */

#include <Windows.h>
#include <gl\glut.h>

#include "KinectSensor.h"

#define WINDOWWIDTH 640
#define WINDOWHEIGHT 480

KinectSensor* kinect;

void RenderCallback()
{
	Sleep(5);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// define position
	glRasterPos2i(0,WINDOWHEIGHT-1);

	// draw on screen buffer from kinect sensor
	glDrawPixels(kinect->GetWidthColor(),kinect->GetHeightColor(),GL_BGRA_EXT,GL_UNSIGNED_BYTE,kinect->GetUnreliableColorBuffer());

	// get depth data
	//BYTE* depthBuffer = kinect->GetDepthBuffer();
	//// process data to be ready to render
	//for (int i = 0; i < kinect->GetWidthDepth() * kinect->GetHeightDepth(); i++)
	//{
	//	USHORT RealDepth = ((USHORT)depthBuffer[i] & 0xfff8) >> 3;
	//	//depthBuffer[i] = (255 - (BYTE)(256*RealDepth/0x0fff)) / 2;
	//	depthBuffer[i] = RealDepth * 2;
	//}

	glDrawPixels(kinect->GetWidthDepth(),kinect->GetHeightDepth(),GL_LUMINANCE,GL_INT,kinect->GetUnreliableDepthBuffer());

	//delete depthBuffer;

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


	kinect = new KinectSensor(RESOLUTION_640X480,RESOLUTION_320X240);
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