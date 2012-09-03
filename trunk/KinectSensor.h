
#ifndef __KINECT_SENSOR_CLASS__
#define __KINECT_SENSOR_CLASS__

#include <Windows.h>
#include <assert.h>

#include "MSR_NuiApi.h"

//defines to configure kinect device
#define RESOLUTION_640X480 NUI_IMAGE_RESOLUTION_640x480
#define RESOLUTION_1280X1024 NUI_IMAGE_RESOLUTION_1280x1024

// class that manages the kinect sensor
// also take care of the kinect-thread
class KinectSensor
{
public:
	// Init the kinect device passing one of the two resolutions avaiable for each buffer
	KinectSensor(int colorBufferResolution);
	~KinectSensor(void);

	
	/*Get color buffer from Kinect device. WARNING: it's your responsability to delete this memory!
	*/
	BYTE* GetColorBuffer();
	// Get pointer to color buffer without alloc new memory. But could show half-rendered frames.
	BYTE* GetUnreliableColorBuffer();
	// Return width of color buffer in pixels
	inline int GetWidthColor() const
	{
		return colorBufferWidth;
	}
	// Return Height of color buffer in pixels
	inline int GetHeightColor()const
	{
		return colorBufferHeight;
	}

private:
	// handle to control kinect's buffers
	HANDLE colorVideoStreamHandle;
	// handles to control signals and the kinect-thread
	HANDLE kinectThreadSignalStop, kinectThread;
	// kinect related events
	HANDLE nextVideoFrameEvent; // when a next frame is ready to be retrieved

	// main function of the kinect thread
	static DWORD WINAPI KinectThread(LPVOID pParam);
	// to be called when there's a new video frame to render
	void NewVideoFrame();

	void InvertColorBuffer(BYTE* rawBuffer);

	// the buffes
	BYTE* colorBuffer;
	int colorBufferWidth, colorBufferHeight;

	// critical section
	CRITICAL_SECTION criticalSection;
};

#endif // __KINECT_SENSOR_CLASS__

