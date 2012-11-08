#include "BufferProcess.h"


BufferProcess::BufferProcess(void)
{
}


BufferProcess::~BufferProcess(void)
{
}

BYTE* BufferProcess::CategorizeObjects(BYTE* buffer, int size,int amountMarkers)
{
	// can't be NULL
	assert(buffer != NULL);
	// must have at least amountMarkers markers
	assert(markers.size() == amountMarkers);
	// at least two markers
	assert(markers.size() > 1);

	// alloc memory
	BYTE* processedBuffer = new BYTE[size];

	int markersSize = markers.size();
	// must have a local reference containing the color of each pixel that point to a marker
	std::vector<glm::u8vec3> markerColors;
	markerColors.resize(markersSize);
	for (int p = 0; p < markersSize; p++)
	{
		// look for the colors in the buffer using the adresses provided by the markers
		markerColors[p].r = buffer[markers[p].x * markers[p].y * 4];
		markerColors[p].g = buffer[markers[p].x * markers[p].y * 4 + 1];
		markerColors[p].b = buffer[markers[p].x * markers[p].y * 4 + 2];
		markerColors[p].b = buffer[markers[p].x * markers[p].y * 4 + 3];
	}
	

	// this function will pick individual pixels of a image and use them to compute  major areas
	// using euclidian distance to mesuare difference between pixels

	// first we need a color for each marker, each marker will generate a different "zone"
	std::vector<glm::u8vec3> zoneColors;
	zoneColors.resize(markersSize);
	for (int j = 0; j < markersSize; j++)
	{
		zoneColors[j].r = rand() % 256;
		zoneColors[j].g = rand() % 256;
		zoneColors[j].b = rand() % 256;
	}

	// the max distance between a given marker and a given pixel to be considered part of that area is defined by MAXDISTANCE
	
	// process all pixels
	for(int i = 0; i < size; i++)
	{

	}

	return processedBuffer;
}

void BufferProcess::AddMarker(glm::uvec2 &marker)
{
	// just add
	markers.push_back(marker);
}

void BufferProcess::ClearMarkers()
{
	markers.clear();
}