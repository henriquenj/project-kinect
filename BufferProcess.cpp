#include "BufferProcess.h"


BufferProcess::BufferProcess(void)
{
}


BufferProcess::~BufferProcess(void)
{
}

BYTE* BufferProcess::CategorizeObjects(BYTE* buffer, int size)
{
	// can't be NULL
	assert(buffer != NULL);
	// at least one marker
	assert(markers.size() > 0);

	// alloc memory
	BYTE* processedBuffer = new BYTE[size];
	memset(processedBuffer,0,size);

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
	}
	

	// this function will pick individual pixels of a image and use them to compute  major areas
	// using euclidian distance to mesuare difference between pixels

	// first we need a color for each marker, each marker will generate a different "zone"
	std::vector<glm::u8vec3> zoneColors;
	zoneColors.resize(markersSize);
	for (int j = 0; j < markersSize; j++)
	{
		zoneColors[j].r = (rand() % 100) + 155;
		zoneColors[j].g = (rand() % 100) + 155;
		zoneColors[j].b = (rand() % 100) + 155;
	}

	// the max distance between a given marker and a given pixel to be considered part of that area is defined by MAXDISTANCE
	
	// process all pixels
	for(int i = 0; i < size; i+=4)
	{
		int smallerDistance = MAXDISTANCE;
		int markerNumber = 99999; // store the index of the marker
		// current pixel
		glm::vec3 color;
		color.r = buffer[i];
		color.g = buffer[i+1];
		color.b = buffer[i+2];
		// for each pixel, check which of the marks are closest
		for (int p = 0; p < markersSize; p++)
		{
			// must convert this to float, I must figure out a better approach
			glm::vec3 marker;
			marker.r = markerColors[p].r;
			marker.g = markerColors[p].g;
			marker.b = markerColors[p].b;
			// test all markers
			float distance = glm::distance(color,marker);
			// check if it's the smaller distance so far
			if (distance < smallerDistance)
			{
				// update number
				distance = smallerDistance;
				// update index
				markerNumber = p;
			}
		}

		if (markerNumber != 99999)
		{
			//at the end, put color zone on that pixel
			processedBuffer[i] = zoneColors[markerNumber].r;
			processedBuffer[i+1] = zoneColors[markerNumber].g;
			processedBuffer[i+2] = zoneColors[markerNumber].b;
		}
	}

	return processedBuffer;
}

void BufferProcess::AddMarker(glm::uvec2 &marker)
{
	// put a limit of five markers
	if (markers.size() == 5)
	{
		MessageBoxA(0,"Max of five markers.","Error",(MB_OK | MB_ICONEXCLAMATION));
		return;
	}
	// just add
	markers.push_back(marker);
}

void BufferProcess::DrawMarkers()
{
	// draw markers
	glBegin(GL_POINTS);
	for (int p = 0; p < markers.size(); p++)
	{
		glVertex2i(markers[p].x,markers[p].y);
	}
	glEnd();
}

void BufferProcess::ClearMarkers()
{
	markers.clear();
}