
#ifndef __MODEL_BUILDER_HEADER__
#define __MODEL_BUILDER_HEADER__

#include <vector>

#include "glm\glm.hpp"
#include "KinectSensor.h"



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
	std::vector<glm::vec3> points;
};


#endif // __MODEL_BUILDER_HEADER__

