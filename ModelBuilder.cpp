#include "ModelBuilder.h"


ModelBuilder::ModelBuilder()
{
	currentDepth = 0;
}


ModelBuilder::~ModelBuilder(void)
{
}


void ModelBuilder::GeneratePoints(short *depthBuffer,glm::uvec2 &size)
{
	// erase previously
	points.clear();
	triangles.clear();

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

	//compute time that the computer took to build the polygons
	clock_t begin = clock();
	
	// must group nearby pixels together
	// first, we need a bool array that can store if a given pixel has already been categorized
	bool *isGrouped = new bool[bufferSize];
	memset(isGrouped,0,bufferSize);

	// call BuildPolygons on any uncategorized vertex
	for (int p = 0; p < bufferSize; p++)
	{
		if (!isGrouped[p])
		{
			BuildPolygon(isGrouped,size,p,depthBuffer);
			currentDepth = 0;
		}
	}
	delete isGrouped;

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

	// finish counting time
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << std::endl << "Time to precess the points (seconds): " << elapsed_secs << std::endl;
}

void ModelBuilder::BuildPolygon(bool * isGrouped, glm::uvec2 size, int currentIndex,short* depthBuffer)
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
	if (currentPosition.y+1 < size.y)
	{
		neighboursIndexes[6] = (currentPosition.x) + (currentPosition.y+1) * size.x;
	}
	// botton right
	// check if isn't on the botton right corner
	if (currentPosition.y+1 < size.y && currentPosition.x+1 < size.x)
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
					// here is the CORE
					// check if this pixel deserve to be grouped with the previous one
					// difference cannot be larger than MAXDIFFERENCE
					if (abs(depthBuffer[currentIndex] - depthBuffer[neighboursIndexes[p]]) < MAXDIFFERENCE)
					{
						bool newTriangle = true;
						//ok, got there, se we need to put this vertex inside a triangle
						// first is to check if the previous triangle is full OR if the triangle belongs to a different zone
						int t_index = triangles.size()-1;
						if (triangles.size() > 0 && triangles[t_index].size() < 3)
						{
							// that's a fairly complex line
							if (abs(depthBuffer[currentIndex] - depthBuffer[triangles[t_index][triangles[t_index].size()-1]]) < MAXDIFFERENCE)
							{
								bool alreadyExist = false;
								// check if the vertex isn't already on this triangle
								for (int b = 0; b < triangles[t_index].size(); b++)
								{
									if(triangles[t_index][b] == neighboursIndexes[p])
									{
										alreadyExist = true;
									}
								}
								// get out of this vertex
								if (alreadyExist){continue;}
								//case not, put vertex in this triangle
								triangles[t_index].push_back(neighboursIndexes[p]);
								newTriangle = false;
							}
						}
						// if not, create new one
						if (newTriangle == true)
						{
							std::vector<int> n_triangle;
							// put vertex index AND previous index
							n_triangle.push_back(currentIndex);
							n_triangle.push_back(neighboursIndexes[p]);
							// put in the main vector
							triangles.push_back(n_triangle);
						}

						// set pixel as grouped
						isGrouped[neighboursIndexes[p]] = true;
						currentDepth++;
						// call function again
						this->BuildPolygon(isGrouped,size,neighboursIndexes[p],depthBuffer);
					}
				}
			}
		}
	}
}

void ModelBuilder::WriteModelOnFile(std::string &filename)
{

	//compute time that the computer took to save the model on hard drive
	clock_t begin = clock();

	// first check if the model has been built
	if (points.empty())
	{
		MessageBoxA(0,"Cannot write a file because the model hasn't been build yet.","Error",(MB_OK | MB_ICONEXCLAMATION));
		return;
	}

	FILE* modelFile = NULL;
	std::string m_fileName = filename; // copy information
	// check for null name
	if (m_fileName.empty())
	{
		MessageBoxA(0,"No file selected, the model will be build on Model.obj","Warning",(MB_OK | MB_ICONEXCLAMATION));
		// fill file name
		m_fileName = "Model.obj";
		modelFile =  fopen(m_fileName.c_str(),"w");
	}
	else
	{
		AddExtensionWithChecking(filename,std::string("obj"));

		// ok, there's a name on the file, must check if it's a real one
		modelFile =  fopen(filename.c_str(),"w");
		if (modelFile == NULL)
		{
			MessageBoxA(0,"Cannot acess this file! Check if you have write access on this folder.","Error",(MB_OK | MB_ICONEXCLAMATION));
			return;
		}
	}

	fprintf(modelFile,"%s \n","#Obj generated by OGL_Kinect");
	fprintf(modelFile,"%s \n","#Avaiable at http://code.google.com/p/project-kinect/");
	// put a group
	fprintf(modelFile,"%s \n","g defaultGroup");


	// put vertices information
	for (int p = 0; p < points.size(); p++)
	{
		fprintf(modelFile,"%s","v ");
		fprintf(modelFile,"%d",points[p].x);
		fprintf(modelFile,"%c",' ');
		fprintf(modelFile,"%d",points[p].y);
		fprintf(modelFile,"%c",' ');
		fprintf(modelFile,"%d \n",points[p].z);
	}

	// put triangle sinformation
	for (int b = 0; b < triangles.size(); b++)
	{
		fprintf(modelFile,"%s","f");
		for (int t = 0; t < triangles[b].size(); t++)
		{
			fprintf(modelFile,"%c", ' ');
			fprintf(modelFile,"%d",triangles[b][t]+1);
		}
		fprintf(modelFile,"\n");
	}

	// close file
	fclose(modelFile);

	// finish counting time
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << std::endl << "Time to save model on file (seconds): " << elapsed_secs << std::endl;
}
