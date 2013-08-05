#ifndef __MAIN_HEADER__
#define __MAIN_HEADER__


/* Program that uses the NUI API to render images from Kinect sensor using OpenGL and wxWidgets */

// commom includes and general defines
#include <Windows.h>
#include "wx\wx.h"
#include "MainFrame.h"
#include "svnrev.h" // generated at pre-build event by SubWCRev


#define APPNAME "Kinect3DBuilder"

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 720

// menu ids for this application
enum IDsMenu
{
    // menu items
    K_Quit = wxID_EXIT,
    K_About = wxID_ABOUT
};



#endif // __MAIN_HEADER__