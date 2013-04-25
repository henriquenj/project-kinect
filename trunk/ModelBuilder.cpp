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

	// call BuildPolygons 
	BuildPolygon(size,depthBuffer);

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

void ModelBuilder::BuildPolygon(glm::uvec2 size,short* depthBuffer)
{
	/* 
		ok, let's change things a bit, AGAIN

		IMPORTANT: now the program will connect pixels with more than one quad, that was the problem the whole time
		IMPORTANT 2: now the program will also use QUADS!

		now let's try code this
	*/

	// create int array to control how many quads belong a specific pixel 
	int * pixelQuad = new int[size.x * size.y];
	// fill with 0
	memset(pixelQuad,0,size.x * size.y * sizeof(int));


	//iterate through all the pixels
	for (int currentIndex = 0; currentIndex < size.x * size.y; currentIndex++)
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

		// now must iterate through all the neighbors searching for the one with most aproximate value
		for(int p = 0; p < 8; p++)
		{
			// rule out the ones with -1
			if (neighboursIndexes[p] != -1)
			{
				//difference must be smaller than MAXDIFFERENCE
				if (abs((depthBuffer[neighboursIndexes[p]] - depthBuffer[currentIndex])) < MAXDIFFERENCE)
				{
					// TODO: MAKE THE CONNECTIONS			
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
		for (int t = 0; t < 3; t++)
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
