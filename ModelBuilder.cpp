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
	quads.clear();

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

	//iterate through all the pixels
	for (int currentIndex = 0; currentIndex < size.x * size.y; currentIndex++)
	{
		int neighboursIndexes[3] = {-1,-1,-1};
		int temp_index;
		glm::ivec2 currentPosition;

		/*
		table of indexes on neighbours array
		0 - center right
		1 - botton center
		2 - botton right
		*/

		// get position of the pixel in the matrix
		currentPosition.x = currentIndex % size.x;
		currentPosition.y = currentIndex / size.x;

		// find indexes

		// center right
		// check if isn't on the last row
		if (currentPosition.x+1 <= size.x)
		{
			neighboursIndexes[0] = (currentPosition.x+1) + (currentPosition.y) * size.x;
		}
		// botton center
		// check if isn't on the last line
		if (currentPosition.y+1 < size.y)
		{
			neighboursIndexes[1] = (currentPosition.x) + (currentPosition.y+1) * size.x;
		}
		// botton right
		// check if isn't on the botton right corner
		if (currentPosition.y+1 < size.y && currentPosition.x+1 < size.x)
		{
			neighboursIndexes[2] = (currentPosition.x+1) + (currentPosition.y+1) * size.x;
		}

		// control how many realible neibhours exist, must be at least 3 to make a quad
		int neighboursHit = 0;
		// now must iterate through the botton neighbors (except botton right) searching for the one with most aproximate value

		/*
		I'm gonna draw to make it clear
		
				.    .   .   .
				.    o   o   .
				.    o   o   .
				.    .   .   .

		*/
		// temporary quad
		glm::uvec4 tQuad;
		for(int p = 0; p < 3; p++)
		{
			// rule out the ones with -1
			if (neighboursIndexes[p] != -1)
			{
				//difference must be smaller than MAXDIFFERENCE
				if (abs((depthBuffer[neighboursIndexes[p]] - depthBuffer[currentIndex])) < MAXDIFFERENCE)
				{
					// hit!
					neighboursHit++;
					// first vertex? add the own pixel
					if (neighboursHit == 1) {tQuad[0] = currentIndex;}
					// put neighbor
					tQuad[neighboursHit] = neighboursIndexes[p];
				}
			}
		}

		//TODO: MAKE THE PROGRAM BUILD TRIANGLES TOO
		// does this have enough vertex?
		if (neighboursHit == 3)
		{
			// put on the quads vector
			quads.push_back(tQuad);
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

	// put quads sinformation
	for (int b = 0; b < quads.size(); b++)
	{
		fprintf(modelFile,"%s","f");
		for (int t = 0; t < 4; t++)
		{
			fprintf(modelFile,"%c", ' ');
			fprintf(modelFile,"%d",quads[b][t]+1);
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
