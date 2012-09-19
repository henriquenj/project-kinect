#include "ModelBuilder.h"


ModelBuilder::ModelBuilder(KinectSensor* sensor)
{
	// to prevent silly errors
	assert (sensor != NULL);

	this->sensor = sensor;
}


ModelBuilder::~ModelBuilder(void)
{
}


void ModelBuilder::GeneratePoints()
{
	// erase previously
	points.clear();

	// retrieve depth buffer from kinect device
	int *depthBuffer = sensor->GetDepthBuffer();

	int max = 0,min = 0,bufferSize;
	bufferSize = sensor->GetHeightDepth() * sensor->GetWidthDepth();

	// find max distance and min distance
	for (int p = 0; p < bufferSize; p++)
	{
		if (depthBuffer[p] > max)
		{
			// new max
			max = depthBuffer[p];
		}
		if (depthBuffer[p] < min)
		{
			// new min
			min = depthBuffer[p];
		}
	}

	// points coordenates will be built using those values as reference
	// ...in the future


	for (int b = 0; b < bufferSize; b++)
	{
		// put points in vector
		Vertex tVertex;
		tVertex.x = b % sensor->GetWidthDepth();
		tVertex.y = b / sensor->GetWidthDepth();
		// make the max value being the X size of the buffer
		tVertex.z = (sensor->GetWidthDepth() * depthBuffer[b]) / max;
		// put on vector
		points.push_back(tVertex);
	}

	// delete memory
	delete depthBuffer;
}
