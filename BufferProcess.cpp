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

	// this function will pick individual pixels of a image and use them to compute  major areas
	// using euclidian distance to mesuare difference between pixels

	// the max distance between a given marker and a given pixel to be considered part of that area is defined by MAXDISTANCE
	//CONTINUAR SEMANA QUE VEM
}

void BufferProcess::AddMarker(glm::vec3 marker)
{
	// just add
	markers.push_back(marker);
}

void BufferProcess::ClearMarkers()
{
	markers.clear();
}