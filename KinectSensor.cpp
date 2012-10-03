#include "KinectSensor.h"


KinectSensor::KinectSensor(int colorBufferResolution,int depthBufferResolution)
{
	// executes setup with the NUI api
	
	// create events
	nextVideoFrameEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	nextDepthFrameEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	// init NUI library
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to init NUI library, check if the Kinect device is connected","Error",(MB_OK | MB_ICONEXCLAMATION));
		exit(0);
	}

	// check for programmer error in the setup
	assert((colorBufferResolution == RESOLUTION_1280X1024) || (colorBufferResolution == RESOLUTION_640X480) &&
			"Wrong resolution set up for the color buffer!");
	assert((depthBufferResolution == RESOLUTION_320X240) || (depthBufferResolution == RESOLUTION_640X480)
			&& "Wrong resolution set up for the depth buffer!");

	// open streams
	if (colorBufferResolution == RESOLUTION_640X480)
	{
		hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,NUI_IMAGE_RESOLUTION_640x480,0,2,nextVideoFrameEvent,&colorVideoStreamHandle);
		// update member variables
		colorBufferWidth = 640;
		colorBufferHeight = 480;
	}
	else 
	{
		hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,NUI_IMAGE_RESOLUTION_1280x1024,0,2,nextVideoFrameEvent,&colorVideoStreamHandle);
		colorBufferWidth = 1280;
		colorBufferHeight = 1024;
	}

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to load Kinect color streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
		exit(0);
	}

	// open depth buffer
	if (depthBufferResolution == RESOLUTION_320X240)
	{
		hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH,NUI_IMAGE_RESOLUTION_320x240,0,2,nextDepthFrameEvent,&depthStremHandle);
		depthBufferWidth = 320;
		depthBufferHeight = 240;
	}
	else // so must be 640x480
	{
		hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH,NUI_IMAGE_RESOLUTION_640x480,0,2,nextDepthFrameEvent,&depthStremHandle);
		depthBufferWidth = 640;
		depthBufferHeight = 480;
	}

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to load Kinect depth stream, check if your Kinect ir working properly","Error",(MB_OK | MB_ICONEXCLAMATION));
		exit(0);
	}

	// init color buffer with black screen
	colorBuffer = new BYTE[colorBufferWidth * colorBufferHeight * 4];
	memset(colorBuffer,0,colorBufferWidth * colorBufferHeight * 4);
	// ...as well the depth buffer
	depthBuffer = new BYTE[depthBufferWidth * depthBufferHeight * 2];
	memset(depthBuffer,0,depthBufferWidth * depthBufferHeight * 2);
	//... and the other depth buffes
	processedBuffer = new int[depthBufferWidth * depthBufferHeight];
	memset(processedBuffer,0,depthBufferWidth * depthBufferHeight * sizeof(int));
	depthBufferToRender = new BYTE[depthBufferWidth * depthBufferHeight];
	memset(depthBufferToRender,0,depthBufferWidth * depthBufferHeight);

	//init critical section
	InitializeCriticalSectionAndSpinCount(&criticalSection,4096);
	// create kinect thread
	kinectThreadSignalStop=CreateEvent(NULL,FALSE,FALSE,NULL);
	kinectThread=CreateThread(NULL,0,KinectThread,this,0,NULL);

}


KinectSensor::~KinectSensor(void)
{
	// dealloc everything related with the kinect device

	DeleteCriticalSection(&criticalSection);

	// stop the kinect thread
	if (kinectThreadSignalStop != NULL)
	{
		SetEvent(kinectThreadSignalStop);

		// Wait for thread to stop
		if(kinectThread!=NULL)
        {
			WaitForSingleObject(kinectThread,INFINITE);
            CloseHandle(kinectThread);
        }

		CloseHandle(kinectThreadSignalStop);
	}

	NuiShutdown();

	// "close" events
	if( nextDepthFrameEvent && ( nextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( nextDepthFrameEvent );
        nextDepthFrameEvent = NULL;
    }
	if( nextVideoFrameEvent && ( nextVideoFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( nextVideoFrameEvent );
        nextVideoFrameEvent = NULL;
    }

	// delete memory 
	delete colorBuffer;
	delete depthBuffer;
	delete processedBuffer;
	delete depthBufferToRender;
}

void KinectSensor::NewVideoFrame()
{

	// create struct to hold frame information
	const NUI_IMAGE_FRAME * pImageFrame = NULL;

	HRESULT hr = NuiImageStreamGetNextFrame(colorVideoStreamHandle,0,&pImageFrame);

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to load Kinect streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
	}

	// get pixel buffer
	NuiImageBuffer* pBuffer = pImageFrame->pFrameTexture;
	
	KINECT_LOCKED_RECT LockedRect;
	pBuffer->LockRect(0,&LockedRect,NULL,0);
	if(LockedRect.Pitch != 0)
	{
		// lock thread
		EnterCriticalSection(&criticalSection);
		// copy to local data
		// and also invert data
		InvertBufferBGRA((BYTE*)LockedRect.pBits,colorBuffer,colorBufferWidth,colorBufferHeight);
		// unlock thread
		LeaveCriticalSection(&criticalSection);
	}
	else
	{
		MessageBoxA(0,"Buffer length of received texture is bogus\r\n","Error",MB_OK | MB_ICONEXCLAMATION);
	}

	NuiImageStreamReleaseFrame( colorVideoStreamHandle, pImageFrame );
}

void KinectSensor::NewDepthFrame()
{
	//create struct to hold frame data
	const NUI_IMAGE_FRAME * pImageFrame = NULL;

	// retrieve frame
	HRESULT hr = NuiImageStreamGetNextFrame(depthStremHandle,0,&pImageFrame);

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to load Kinect streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
	}

	NuiImageBuffer* pTexture = pImageFrame->pFrameTexture;
	KINECT_LOCKED_RECT LockedRect;

	pTexture->LockRect(0,&LockedRect, NULL,0);

	if( LockedRect.Pitch != 0 )
    {
		// critical section
		EnterCriticalSection(&criticalSection);
		//copy to local data
		InvertBufferLines((BYTE*) LockedRect.pBits,(BYTE*)depthBuffer,depthBufferWidth,depthBufferHeight,2);
		// convert to milimiter and put in the processed buffer
		for (int i = 0; i < depthBufferWidth * depthBufferHeight * 2; i+=2)
		{
			int d = depthBuffer[i+1];
			d = d << 8;
			processedBuffer[i/2] = d;
		}

		LeaveCriticalSection(&criticalSection);
	}
	else 
	{
		MessageBoxA(0,"Buffer length of received texture is bogus\r\n","Error",MB_OK | MB_ICONEXCLAMATION);
	}

	NuiImageStreamReleaseFrame(depthStremHandle,pImageFrame);

}
DWORD KinectSensor::KinectThread(LPVOID pParam)
{
	// convert pointer type
	KinectSensor* pThis = (KinectSensor*)pParam;
	// put events in a array to be used by WaitForMultipleObjects function
	HANDLE events[3];
	events[0] = pThis->kinectThreadSignalStop;
	events[1] = pThis->nextVideoFrameEvent;
	events[2] = pThis->nextDepthFrameEvent;

	int nEventIdx;

	// thread loop
	while(true)
	{
		// wait for an event
		nEventIdx = WaitForMultipleObjects(sizeof(events)/sizeof(events[0]),events,FALSE,100);
		
		if (nEventIdx == 0)
		{
			// stop singnal, must get out of the application
			break;
		}

		// process other signals
		if (nEventIdx == 1)
		{
			// this means that there's a new video frame for us to render
			pThis->NewVideoFrame();
		}
		
		if (nEventIdx == 2)
		{
			// this means that there's a new depth frame for us
			pThis->NewDepthFrame();
		}
	}
	return 0;
}

BYTE* KinectSensor::GetColorBuffer() 
{
	//lock thread
	EnterCriticalSection(&criticalSection);
	// copy data
	BYTE* newBuffer = new BYTE[colorBufferWidth * colorBufferHeight * 4];
	memcpy(newBuffer,colorBuffer,colorBufferWidth*colorBufferHeight*4);

	LeaveCriticalSection(&criticalSection);

	return newBuffer;
}

int* KinectSensor::GetDepthBuffer()
{
	// lock thread
	EnterCriticalSection(&criticalSection);
	// copy data
	int* newBuffer = new int[depthBufferWidth * depthBufferHeight];
	memcpy(newBuffer,processedBuffer,depthBufferWidth * depthBufferHeight * sizeof(int));

	LeaveCriticalSection(&criticalSection);

	return newBuffer;
}

void KinectSensor::InvertBufferLines(BYTE* rawBuffer, BYTE* newBuffer,int width, int height, int bpc)
{

	int lineSize = width * bpc;
	// swap lines only
	for (int p = 0, u = height-1; p < height-1; p++, u--)
	{
		memcpy(&(newBuffer[u * width * bpc]),&(rawBuffer[p * width * bpc]),lineSize);
	}
}

void KinectSensor::InvertBufferBGRA(BYTE* rawBuffer, BYTE* newBuffer,int width, int height)
{
	// invert data
	for (int j = 0, p = width * height * 4; j < width * height * 4, p != 0; j += 4, p -= 4)
	{
		newBuffer[j] = rawBuffer[p];
		newBuffer[j+1] = rawBuffer[p+1];
		newBuffer[j+2] = rawBuffer[p+2];
		newBuffer[j+3] = rawBuffer[p+3];
	}
}

BYTE* KinectSensor::GetDepthBufferToRender()
{
	// copy values only if this funcion is actually called
	for (int i = 0; i < depthBufferWidth * depthBufferHeight; i++)
	{
		BYTE l = 255 - (BYTE)(256*processedBuffer[i]/0x0fff);
		depthBufferToRender[i] = l / 2;
	}

	return depthBufferToRender;
}

