
#ifndef __MODEL_BUILDER_HEADER__
#define __MODEL_BUILDER_HEADER__

#include <vector>
#include <fstream>

#include "glm\glm.hpp"
#include "KinectSensor.h"


// class that generate geometry data from Kinect buffers
class ModelBuilder
{
public:
	ModelBuilder();
	~ModelBuilder(void);

	// Generate points using the last depth buffer avaiable. WARNING: this will ERASE the previously created model 
	void GeneratePoints(int *depthBuffer, glm::uvec2 size);
	// Write model on a .obj file using last model builded by GeneratePoints.
	void WriteModelOnFile(std::string &filename);

private:
	// vector of points
	std::vector<glm::vec3> points;
};


#endif // __MODEL_BUILDER_HEADER__
