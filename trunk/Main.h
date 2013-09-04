#ifndef __MAIN_HEADER__
#define __MAIN_HEADER__
/* Program that uses the NUI API to render images from Kinect sensor using OpenGL and wxWidgets */


// commom includes and general defines

#define APPNAME "Kinect3DBuilder"
#define APPSITE "http://code.google.com/p/project-kinect/"

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 960

#include <Windows.h>
#include "wx\wx.h"
#include "MainFrame.h"
#include "svnrev.h" // generated at pre-build event by SubWCRev


// menu ids for this application
enum IDsMenu
{
    // menu items
    K_Quit = wxID_EXIT,
    K_About = wxID_ABOUT,
	// interface options
	K_LoadImage = wxID_HIGHEST+1, // load image button
	K_ShowDepth, // show or not depth buffer radio button
	K_GenerateModel, // generate model based on a loaded image
	K_ConnectKinect, //change the program state to render input from Kinect
	K_SaveBuffer // save current frame being displayed

};



#endif // __MAIN_HEADER__