
#ifndef __MODEL_BUILDER_HEADER__
#define __MODEL_BUILDER_HEADER__

#include <vector>
#include <fstream>
#include <iostream>

#include "glm\glm.hpp"
#include "KinectSensor.h"
#include "UtilitiesFunctions.h"

// max difference allowed by the alogorithm
#define MAXDIFFERENCE 500

// class that generate geometry data from Kinect buffers
class ModelBuilder
{
public:
	ModelBuilder();
	~ModelBuilder(void);

	// Generate points using the last depth buffer avaiable. WARNING: this will ERASE the previously created model 
	void GeneratePoints(short *depthBuffer, glm::uvec2 &size);
	// Write model on a .obj file using last model builded by GeneratePoints.
	void WriteModelOnFile(std::string &filename);

private:
	// vector of points
	std::vector<glm::uvec3> points;
	// vector of quads to hold the connectivity
	std::vector<glm::vec4> quads;
	/* build polygons recursively based on nearby points. Current index referes to the current pixel to be processed, 0 if it's the first. */
	void BuildPolygon(glm::uvec2 size, short* depthBuffer);
};


#endif // __MODEL_BUILDER_HEADER__

