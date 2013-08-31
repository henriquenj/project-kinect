#ifndef __APP_CONFIG_CLASS__
#define __APP_CONFIG_CLASS__

/*Singleton class that provide access to the options upon which the application is currently running */
class AppConfig
{
public:
	// return TRUE if the app is running in device connected mode and FALSE otherwise
	static inline const bool GetKinectConnected()
	{
		return kinectConnected;
	}
	// set the current program mode, only GUI widgets will get this information
	static inline void SetKinectConnected(const bool _kinectConnected)
	{
		kinectConnected = _kinectConnected;
	}
	// return TRUE if the program is showing a loaded image from a file, FALSE othereise
	static inline const bool GetLoadedImage()
	{
		return loadedImage;
	}
	// set option if the program is showing a loadead image
	static inline void SetLoadedImageOption(const bool _loadedImage)
	{
		loadedImage = _loadedImage;
	}
	~AppConfig(void);

private:
	AppConfig(void);
	// kinnect connected option, program init in FALSE
	static bool kinectConnected;
	// true if there's a loaded image on the program
	static bool loadedImage;
};



#endif // __APP_CONFIG_CLASS__