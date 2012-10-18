#ifndef __BUFFERPROCESS_CLASS__
#define __BUFFERPROCESS_CLASS__

#include <Windows.h>
#include <assert.h>
#include <vector>

#include "glm\glm.hpp"

#define MAXDISTANCE 32 // max distance between a marker and a pixel to be considered part of that area
					   // subject to change


// buffer process class does a series of computations over a buffer
class BufferProcess
{
public:
	BufferProcess(void);
	/* Reduce a color buffer to a simpler form, trying to find regions with the same color and grouping them together. 
	Return the processed buffer. The pixel marks must be already defined. WARNING: it's your responsability to delete this memory*/
	BYTE* CategorizeObjects(BYTE* buffer, int size, int amountMarkers);
	// Add a marker to the list
	void AddMarker(glm::vec3 marker);
	// empty marker list
	void ClearMarkers();
	~BufferProcess(void);

private:
	// markers for objects generation
	std::vector<glm::vec3> markers;
};


#endif // __BUFFERPROCESS_CLASS__

