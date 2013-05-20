
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
	void WriteModelOnFile(std::string &filename,std::string &imagePath);
	// Get points
	//std::vector<glm::uvec3>* points

private:
	// vector of points
	std::vector<glm::uvec3> points;
	// vector of texture coordinates
	std::vector<glm::vec2> texturesCoord;
	// vector of quads to hold the connectivity
	std::vector<glm::uvec4> quads;
	/* build polygons based on nearby points. */
	void BuildPolygon(glm::uvec2 &size, short* depthBuffer, int* nullVertex);
};


#endif // __MODEL_BUILDER_HEADER__

