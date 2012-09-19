
#ifndef __MODEL_BUILDER_HEADER__
#define __MODEL_BUILDER_HEADER__

#include <vector>

#include "KinectSensor.h"

// TODO: start to use some real math api like GLM
struct Vertex
{
	float x,y,z;
};


// class that generate geometry data from Kinect buffers
class ModelBuilder
{
public:
	ModelBuilder(KinectSensor* sensor);
	~ModelBuilder(void);

	// Generate points using the last depth buffer avaiable. WARNING: this will ERASE the previously created model 
	void GeneratePoints();

private:
	KinectSensor* sensor; // local reference
	// vector of points
	std::vector<Vertex> points;
};


#endif // __MODEL_BUILDER_HEADER__

