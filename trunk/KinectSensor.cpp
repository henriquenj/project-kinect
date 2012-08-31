#include "KinectSensor.h"


KinectSensor::KinectSensor(int colorBufferResolution)
{

	// executes setup with the NUI api
	
	// create events
	nextVideoFrameEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	// init NUI library
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);

	if (FAILED(hr))
	{
		MessageBoxA(0,"Failed to init NUI library, check if the Kinect device is connected","Error",(MB_OK | MB_ICONEXCLAMATION));
		exit(0);
	}

	// check for programmer error in the setup
	assert((colorBufferResolution == RESOLUTION_1280X1024) || (colorBufferResolution == RESOLUTION_640X480));

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
		MessageBoxA(0,"Failed to load Kinect streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
		exit(0);
	}

	// init the buffer with black screen
	colorBuffer = new BYTE[colorBufferWidth * colorBufferHeight * 4];
	memset(colorBuffer,0,colorBufferWidth * colorBufferHeight * 4);

	//init critical section
	InitializeCriticalSectionAndSpinCount(&criticalSection,4096);
	// create kinect thread
	kinectThreadSignalStop=CreateEvent(NULL,FALSE,FALSE,NULL);
	kinectThread=CreateThread(NULL,0,KinectThread,this,0,NULL);

}


KinectSensor::~KinectSensor(void)
{
	DeleteCriticalSection(&criticalSection);

	// TODO: make all the NUI api finalization bullshit
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
		InvertColorBuffer((BYTE*)LockedRect.pBits);
		// unlock thread
		LeaveCriticalSection(&criticalSection);
	}
	else
	{
		MessageBoxA(0,"Buffer length of received texture is bogus\r\n","Error",MB_OK | MB_ICONEXCLAMATION);
	}

	NuiImageStreamReleaseFrame( colorVideoStreamHandle, pImageFrame );
}

DWORD KinectSensor::KinectThread(LPVOID pParam)
{
	// convert pointer type
	KinectSensor* pThis = (KinectSensor*)pParam;
	// put events in a array to be used by WaitForMultipleObjects function
	HANDLE events[2];
	events[0] = pThis->kinectThreadSignalStop;
	events[1] = pThis->nextVideoFrameEvent;

	int nEventIdx;

	// thread loop
	while(true)
	{
		// wait for an event
		nEventIdx = WaitForMultipleObjects(sizeof(events)/sizeof(events[0]),events,false,100);
		
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
	}
	return 0;
}

BYTE* KinectSensor::GetColorBuffer() 
{
	//lock thread
	EnterCriticalSection(&criticalSection);
	// copy data
	BYTE* newBuffer = new BYTE[colorBufferWidth * colorBufferHeight * 4];
	memcpy(newBuffer,colorBuffer,colorBufferWidth*colorBufferHeight*4*sizeof(BYTE));

	LeaveCriticalSection(&criticalSection);

	return newBuffer;
}

BYTE* KinectSensor::GetUnreliableColorBuffer()
{
	return colorBuffer;
}

void KinectSensor::InvertColorBuffer(BYTE* rawBuffer)
{

	int lineSize = colorBufferWidth * 4;
	// swap lines
	for (int p = 0, u = colorBufferHeight-1; p < colorBufferHeight-1; p++, u--)
	{
		memcpy(&(colorBuffer[u * colorBufferWidth * 4]),&(rawBuffer[p * colorBufferWidth * 4]),lineSize);
	}
}

