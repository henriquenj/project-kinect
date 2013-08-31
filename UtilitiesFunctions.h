// Utilities functions header contains some useful stuff, duh

#ifndef __UTILITIES_FUNCIONS_HEADER__
#define __UTILITIES_FUNCIONS_HEADER__

#define _CRT_SECURE_NO_WARNINGS //disable this shit

#include <Windows.h>
#include <commdlg.h> // For GetSaveFileName(), GetOpenFileName(), Etc.
#include <stdio.h>
#include <fstream>
#include <setjmp.h>
#include "glm\glm.hpp"

// File Dialog Types:
enum FileDialogType { DialogOpen, DialogSave };

// ShowFileDialog function written thousand of years ago by Guilherme Lampert

/// Shows a file dialog window using a specific system service. Once the user selects a file from
/// the window and clicks the ok button, the function returns with the full name of the selected file.
/// The returned string should not be used directly, instead copy it to some other buffer, since it exists as
/// a per-thread static variable. In the case of the user closing the dialog window before selecting a file,
/// zero is returned. Zero is also returned if an error occurs.
/// You can pass many file filters as arguments. Use the '|' symbol to combine them. Example:
/// Parameter 'filterDesc': "Text Files (*.txt) | Bitmap Files (*.bmp)"
/// Parameter 'extFilter':  "*.txt | *.bmp"

const char * ShowFileDialog(/* Optional */void * window, int type, const char * filterDesc, const char * extFilter);

///////////////// A SIMPLE EXAMPLE /////////////////

//const char * filePath = ShowFileDialog(0, DialogOpen, "TGA Images (*.tga) | JPGE Images (*.jpg)", "*.tga | *.jpg");
//
//if (filePath != 0)
//{
//	// All ok, found file, no errors
//}
//else
//{
//	// User probably clicked the cancel button, no file returned...
//}



/* Load a png file given a file name, return buffer and fills the size vector and the hasAlpha bool. WARNING: you must delete this memory */
BYTE* LoadPng(const char * file_name, glm::uvec2 &size,bool &hasAlpha);
// courtesy from this website http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures#A_simple_libpng_example

/*Save a png file given a buffer*/
bool SavePng(const char *savefile, BYTE* data_in, int w, int h);
// courtesy from this user http://stackoverflow.com/questions/12061331/libpng-writing-a-png-fails-stops-at-header-write-error

/*Swap values between two channels*/
void BGRAtoRGBA(BYTE *buffer,int x, int y);

BYTE* InvertLines(BYTE *buffer,int width, int height,int bpc);

/*Dump the depth buffer onto a TXT file */
void DumpDepthBuffer(short *buffer, int width, int height, const char* filename);

// Read depth buffer saved on a file, fills the size structure with the size of the buffer. WARNING: you must delete this memory!
short* ReadDepthBuffer(glm::uvec2 &size, const char* filename);

/* add extension to a filename if there isn't one
	extension parameter refer to the extension to be checked
	newExtension parameter refer to the extension to be added*/
void AddExtensionWithChecking(std::string &filepath, std::string &extension,std::string &newExtension);

/*Remove absolute path from a string, returning only the file name*/
void RemoveAbsolutePath(std::string &filepath);

/*Find maximum value inside a buffer*/
int FindMaxValue(short* buffer, int size);
#endif // __UTILITIES_FUNCIONS_HEADER__