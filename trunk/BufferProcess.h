#ifndef __BUFFERPROCESS_CLASS__
#define __BUFFERPROCESS_CLASS__

#include <Windows.h>
#include <assert.h>
#include <vector>

#include "glm\glm.hpp"
#include "glm\ext.hpp"

#define MAXDISTANCE 64 // max distance between a marker and a pixel to be considered part of that area
					   // subject to change


// buffer process class does a series of computations over a buffer
class BufferProcess
{
public:
	BufferProcess(void);
	/* Reduce a color buffer to a simpler form, trying to find regions with the same color and grouping them together. Buffer must be in the RGBA format.
	Return the processed buffer. The pixel marks must be already defined. WARNING: it's your responsability to delete this memory*/
	BYTE* CategorizeObjects(BYTE* buffer, int size, int amountMarkers);
	// Add a marker to the list
	void AddMarker(glm::uvec2 &marker);
	// empty marker list
	void ClearMarkers();
	// Return the number of markers currently defined
	inline int const GetMarkersSize()const
	{
		return markers.size();
	}
	~BufferProcess(void);

private:
	// markers for objects generation
	std::vector<glm::uvec2> markers;
};


#endif // __BUFFERPROCESS_CLASS__

