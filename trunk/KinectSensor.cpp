#include "KinectSensor.h"

KinectSensor::KinectSensor(_NUI_IMAGE_RESOLUTION colorBufferResolution,_NUI_IMAGE_RESOLUTION depthBufferResolution)
{
        // executes setup with the NUI api
        colorVideoStreamHandle = depthStreamHandle = kinectThreadSignalStop = kinectThread = nextVideoFrameEvent = nextDepthFrameEvent = NULL;

        // check for programmer error in the setup
        assert((colorBufferResolution == RESOLUTION_1280X960) || (colorBufferResolution == RESOLUTION_640X480) &&
                        "Wrong resolution set up for the color buffer!");
        assert((depthBufferResolution == RESOLUTION_320X240) || (depthBufferResolution == RESOLUTION_640X480)
                        && "Wrong resolution set up for the depth buffer!");

        // create events
        nextVideoFrameEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        nextDepthFrameEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

        sensor = NULL;

        //create kinect sensor
        if(FAILED (NuiCreateSensorByIndex(0, &sensor)))
        {
                MessageBoxA(0,"Failed to init NUI library, check if the Kinect device is connected","Error",(MB_OK | MB_ICONEXCLAMATION));
                isReady = false;
                return;
        }

        // init NUI
        if(FAILED(sensor->NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR )))
        {
                MessageBoxA(0,"Failed to init NUI library, check if the Kinect device is connected","Error",(MB_OK | MB_ICONEXCLAMATION));
                isReady = false;
                return;
        }

        // opem stream to color buffer
        if(FAILED(sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,colorBufferResolution,0,2,nextVideoFrameEvent,&colorVideoStreamHandle )))
        {
                MessageBoxA(0,"Failed to open color buffer.","Error",(MB_OK | MB_ICONEXCLAMATION));
                isReady = false;
                return;
        }
        unsigned long BufferWidth;
        unsigned long BufferHeight;
        // store resolution
        NuiImageResolutionToSize(colorBufferResolution,BufferWidth,BufferHeight);
        colorBufferHeight = BufferHeight;
        colorBufferWidth = BufferWidth;

        // opem stream to depth buffer
        if(FAILED(sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH,depthBufferResolution,0,2, nextDepthFrameEvent,&depthStreamHandle )))
        {
                MessageBoxA(0,"Failed to open depth buffer.","Error",(MB_OK | MB_ICONEXCLAMATION));
                isReady = false;
                return;
        }

        // store resolution
        NuiImageResolutionToSize(depthBufferResolution,BufferWidth,BufferHeight);
        depthBufferHeight = BufferHeight;
        depthBufferWidth = BufferWidth;

        // init color buffer with black screen
        colorBuffer = new BYTE[colorBufferWidth * colorBufferHeight * 4];
        memset(colorBuffer,0,colorBufferWidth * colorBufferHeight * 4);
        // ...as well the depth buffer
        depthBuffer = new short[depthBufferWidth * depthBufferHeight];
        memset(depthBuffer,0,depthBufferWidth * depthBufferHeight * sizeof(short));

        //init critical section
        InitializeCriticalSectionAndSpinCount(&criticalSection,4096);
        // create kinect thread
        kinectThreadSignalStop=CreateEvent(NULL,FALSE,FALSE,NULL);
        kinectThread=CreateThread(NULL,0,KinectThread,this,0,NULL);

        // all fine
        isReady = true;

}


KinectSensor::~KinectSensor(void)
{
        // dealloc everything related with the kinect device

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

        if (sensor)
        {
                sensor->NuiShutdown();
        }

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

        if (isReady)
        {
                // delete memory
                delete colorBuffer;
                delete depthBuffer;
                DeleteCriticalSection(&criticalSection);
        }
       
}

void KinectSensor::NewVideoFrame()
{

        // create struct to hold frame information
        NUI_IMAGE_FRAME pImageFrame;

        HRESULT hr = sensor->NuiImageStreamGetNextFrame(colorVideoStreamHandle,500,&pImageFrame);

        if (FAILED(hr))
        {
                MessageBoxA(0,"Failed to load Kinect streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
                return;
        }

        // get pixel buffer
        INuiFrameTexture* pBuffer = pImageFrame.pFrameTexture;
       
        NUI_LOCKED_RECT LockedRect;
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

        sensor->NuiImageStreamReleaseFrame( colorVideoStreamHandle, &pImageFrame );
}

void KinectSensor::NewDepthFrame()
{
        //create struct to hold frame data
        const NUI_IMAGE_FRAME * pImageFrame = NULL;

        // retrieve frame
        HRESULT hr = NuiImageStreamGetNextFrame(depthStreamHandle,500,&pImageFrame);

        if (FAILED(hr))
        {
                MessageBoxA(0,"Failed to load Kinect streams, check if your Kinect ir working properly", "Error", (MB_OK | MB_ICONEXCLAMATION));
                return;
        }

        INuiFrameTexture* pTexture = pImageFrame->pFrameTexture;
        NUI_LOCKED_RECT LockedRect;

        pTexture->LockRect(0,&LockedRect, NULL,0);

        if( LockedRect.Pitch != 0 )
    {
                // critical section
                EnterCriticalSection(&criticalSection);
                //copy to local data and invert
                InvertBuffer((short*)LockedRect.pBits,depthBuffer,depthBufferWidth,depthBufferHeight,1);

                LeaveCriticalSection(&criticalSection);
        }
        else
        {
                MessageBoxA(0,"Buffer length of received texture is bogus\r\n","Error",MB_OK | MB_ICONEXCLAMATION);
        }

        NuiImageStreamReleaseFrame(depthStreamHandle,pImageFrame);

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
        //memcpy(newBuffer,colorBuffer,colorBufferWidth*colorBufferHeight*4);
		for (int a =  colorBufferWidth * colorBufferHeight * 3, b = 0; a >= 0 ;a-=3,b+=4)
        {
                newBuffer[a] = colorBuffer[b+2];
                newBuffer[a+1] = colorBuffer[b+1];
                newBuffer[a+2] = colorBuffer[b];
        }


        LeaveCriticalSection(&criticalSection);

        return newBuffer;
}

short* KinectSensor::GetDepthBuffer()
{
        // lock thread
        EnterCriticalSection(&criticalSection);
        // copy data
        short* newBuffer = new short[depthBufferWidth * depthBufferHeight];
        memcpy(newBuffer,depthBuffer,depthBufferWidth * depthBufferHeight * sizeof(short));

        LeaveCriticalSection(&criticalSection);

        return newBuffer;
}

void KinectSensor::InvertBuffer(short* rawBuffer, short* newBuffer,int width, int height, int bpc)
{
        for (int p = 0 ,u = width * height * bpc; u != 0; p++, u--)
        {
                newBuffer[p] = rawBuffer[u];
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
