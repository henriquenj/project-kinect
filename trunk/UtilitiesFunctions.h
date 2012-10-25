// Utilities functions header contains some useful stuff, duh

#ifndef __UTILITIES_FUNCIONS_HEADER__
#define __UTILITIES_FUNCIONS_HEADER__

#define _CRT_SECURE_NO_WARNINGS //disable this shit

// defines for libpng
#define PNG_STDIO_SUPPORTED
#define PNG_SEQUENTIAL_READ_SUPPORTED

#include <Windows.h>
#include <commdlg.h> // For GetSaveFileName(), GetOpenFileName(), Etc.
#include <stdio.h>
#include <setjmp.h>
#include "png.h"

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

// Load a png file given a file name, return buffer. WARNING: you must delete this memory
int* LoadPng(const char * file_name);
// courtesy from this website http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures#A_simple_libpng_example


#endif // __UTILITIES_FUNCIONS_HEADER__