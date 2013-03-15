#include "ModelBuilder.h"


ModelBuilder::ModelBuilder()
{
}


ModelBuilder::~ModelBuilder(void)
{
}


void ModelBuilder::GeneratePoints(short *depthBuffer,glm::uvec2 &size)
{
	// erase previously
	points.clear();

	int max = 0,min = 0,bufferSize;
	bufferSize = size.x * size.y;

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
	
	// must group nearby pixels together
	// first, we need a bool array that can store if a given pixel has already been categorized
	bool *isGrouped = new bool[bufferSize];
	memset(isGrouped,0,bufferSize);

	// reserve space
	points.resize(bufferSize);
	for (int b = 0; b < bufferSize; b++)
	{
		// put points in vector
		glm::vec3 tVertex;
		tVertex.x = b % size.x;
		tVertex.y = b / size.y;
		// make the max value being the X size of the buffer
		tVertex.z = (size.x * depthBuffer[b]) / max;
		// put on vector
		points[b] = tVertex;
	}

	// call BuildPolygons on any uncategorized vertex
	for (int p = 0; p < bufferSize; p++)
	{
		currentDepth = 0;
		if (!isGrouped[p])
		{
			BuildPolygon(isGrouped,size,0,depthBuffer);
		}
	}

	delete isGrouped;
}

void ModelBuilder::BuildPolygon(bool * isGrouped, glm::uvec2 &size, int currentIndex,short* depthBuffer)
{

	int neighboursIndexes[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
	int temp_index;
	glm::ivec2 currentPosition;
	/*
		table of indexes on neighbours array
		0 - top left
		1 - top center
		2 - top right
		3 - center left
		4 - center right
		5 - botton left
		6 - botton center
		7 - botton right
	*/

	// get position of the pixel in the matrix
	currentPosition.x = currentIndex % size.x;
	currentPosition.y = currentIndex / size.x;

	// find indexes

	// top left
	// check if isn't on top left corner
	if (currentPosition.x-1 >= 0 && currentPosition.y-1 >= 0)
	{
		// grab index
		neighboursIndexes[0] = (currentPosition.x-1) + (currentPosition.y-1) * size.x;
	}
	// top center
	// check if isn't on the top line
	if (currentPosition.y-1 >= 0)
	{
		neighboursIndexes[1] = (currentPosition.x) + (currentPosition.y-1) * size.x;
	}
	// top right
	// check if isn't on the top rigt corner
	if (currentPosition.x+1 <= size.x && currentPosition.y-1 <= size.x)
	{
		neighboursIndexes[2] = (currentPosition.x+1) + (currentPosition.y-1) * size.x;
	}
	// center left
	// check if isn't on the first row
	if (currentPosition.x-1 >= 0)
	{
		neighboursIndexes[3] = (currentPosition.x-1) + (currentPosition.y) * size.x;
	}
	// center right
	// check if isn't on the last row
	if (currentPosition.x+1 <= size.x)
	{
		neighboursIndexes[4] = (currentPosition.x+1) + (currentPosition.y) * size.x;
	}
	// botton left
	// check if isn't on the botton left corner
	if (currentPosition.x-1 >= 0 && currentPosition.y-1 >= 0)
	{
		neighboursIndexes[5] = (currentPosition.x-1) + (currentPosition.y-1) * size.x;
	}
	// botton center
	// check if isn't on the last line
	if (currentPosition.y+1 <= size.y)
	{
		neighboursIndexes[6] = (currentPosition.x) + (currentPosition.y+1) * size.x;
	}
	// botton right
	// check if isn't on the botton right corner
	if (currentPosition.y+1 <= size.y && currentPosition.x+1 <= size.x)
	{
		neighboursIndexes[7] = (currentPosition.x+1) + (currentPosition.y+1) * size.x;
	}
	

	//iterate through all the pixels
	for (int p = 0; p < 8; p++)
	{
		// -1 means that there are no neighbor
		if (neighboursIndexes[p] != -1)
		{
			// only process pixels that weren't processed yet
			if (!isGrouped[neighboursIndexes[p]])
			{
				if (currentDepth < MAXDEPTH) // to prevent stack overflow
				{
					depthBuffer[neighboursIndexes[p]] = 255;
					// set pixel as grouped
					isGrouped[neighboursIndexes[p]] = true;
					// call function again
					currentDepth++;
					this->BuildPolygon(isGrouped,size,neighboursIndexes[p],depthBuffer);
				}
			}
		}
	}

}

void ModelBuilder::WriteModelOnFile(std::string &filename)
{
	// first check if the model has been built
	if (points.empty())
	{
		MessageBoxA(0,"Cannot write a file because the model hasn't been build yet.","Error",(MB_OK | MB_ICONEXCLAMATION));
	}

	std::ofstream modelFile;
	std::string m_fileName = filename; // copy information
	// check for null name
	if (m_fileName.empty())
	{
		MessageBoxA(0,"No file selected, the model will be build on Model.obj","Warning",(MB_OK | MB_ICONEXCLAMATION));
		// fill file name
		m_fileName = "Model.obj";
		modelFile.open(m_fileName.c_str(),std::ios_base::out);
	}
	else
	{
		AddExtensionWithChecking(filename,std::string("obj"));

		// ok, there's a name on the file, must check if it's a real one
		modelFile.open(filename.c_str());
	}

	modelFile << "#Obj generated by OGL_Kinect" << std::endl;
	modelFile << "#Avaiable at http://code.google.com/p/project-kinect/" << std::endl;
	// put a group
	modelFile << "g defaultGroup" << std::endl;

	// put vertices information
	for (int p = 0; p < points.size(); p++)
	{
		if ((p % 3) == 0 && p > 2) // every 3 verties, a face
		{
			modelFile << "f " << p << " " << p-1 << " " << p-2 << std::endl;
		}
		modelFile << "v " << points[p].x << " " << points[p].y << " " << points[p].z << std::endl;
		//modelFile << "f " << p+1 << " " << p+1 << " " << p+1 << std::endl;
	}

	// close file
	modelFile.close();
}
