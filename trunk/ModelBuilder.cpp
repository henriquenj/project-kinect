#include "ModelBuilder.h"

ModelBuilder::ModelBuilder()
{
	
}


ModelBuilder::~ModelBuilder(void)
{
}


void ModelBuilder::GeneratePoints(short *depthBuffer,glm::uvec2 &size)
{
	//compute time that the computer took to build the polygons
	clock_t begin = clock();

	// erase previously
	points.clear();
	quads.clear();
	texturesCoord.clear();

	int bufferSize;
	float max = 0,min = 0;
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


	// array that will store the amount of null pixels at each given point of the depth buffer
	// this will be need when the algotithm create the faces
	int* nullPixels = new int[bufferSize];
	memset(nullPixels,0,bufferSize * sizeof(int));

	// the total amount of null pixels
	int amountNullPixels = 0;

	for (int b = 0; b < bufferSize; b++)
	{
		// check if it's one of the pixels in the dead area (null pixel)
		if (depthBuffer[b] == 0)
		{
			amountNullPixels++;
			// update nullpixels
			nullPixels[b] = amountNullPixels;
			// do not process this
			continue;
		}
		// put points in vector
		glm::vec3 tVertex;
		tVertex.x = b % size.x;
		tVertex.y = b / size.y;
		// make the max value being the X size of the buffer
		tVertex.z = (size.x * depthBuffer[b]) / max;
		// put on vector
		points.push_back(tVertex);
		nullPixels[b] = amountNullPixels;
	}

	// must group nearby pixels together

	//call buildPlanes
	BuildPlans(size,depthBuffer);
	// call BuildPolygons 
	BuildPolygon(size,depthBuffer,nullPixels);

	delete nullPixels;

	// finish counting time
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << std::endl << "Time to precess the points (seconds): " << elapsed_secs << std::endl;
}

void ModelBuilder::BuildPolygon(glm::uvec2 &size,short* depthBuffer, int* nullPixels)
{
	/* 
		ok, let's change things a bit, AGAIN

		IMPORTANT: now the program will connect pixels with more than one quad, that was the problem the whole time
		IMPORTANT 2: now the program will also use QUADS!

		now let's try code this
	*/

	// fraction of a pixel on this particular image
	const float fractionOfPixelX = 1.0f / size.x;
	const float fractionOfPixelY = 1.0f / size.y;

	//iterate through all the pixels
	for (int currentIndex = 0; currentIndex < size.x * size.y; currentIndex++)
	{

		// check if it's one of the pixels in the dead area (no reliable values)
		if (depthBuffer[currentIndex] == 0){continue;} // do not process this
		int neighboursIndexes[3] = {-1,-1,-1};
		glm::ivec2 currentPosition;

		/*
		table of indexes on neighbours array
		0 - center right
		1 - botton right
		2 - botton center
		*/

		// get position of the pixel in the matrix
		currentPosition.x = currentIndex % size.x;
		currentPosition.y = currentIndex / size.x;

		// compute texture coordinates for this pixel
		float vtx, vty;
		vtx = currentPosition.x * fractionOfPixelX;
		vty = currentPosition.y * fractionOfPixelY;
		texturesCoord.push_back(glm::vec2(vtx,vty));

		// find indexes

		// center right
		// check if isn't on the last row
		if (currentPosition.x+1 <= size.x)
		{
			neighboursIndexes[0] = (currentPosition.x+1) + (currentPosition.y) * size.x;
		}
		// botton right
		// check if isn't on the botton right corner
		if (currentPosition.y+1 < size.y && currentPosition.x+1 < size.x)
		{
			neighboursIndexes[1] = (currentPosition.x+1) + (currentPosition.y+1) * size.x;
		}
		// botton center
		// check if isn't on the last line
		if (currentPosition.y+1 < size.y)
		{
			neighboursIndexes[2] = (currentPosition.x) + (currentPosition.y+1) * size.x;
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
					if (neighboursHit == 1) {tQuad[0] = currentIndex - nullPixels[currentIndex];} //subtract by null pixels at this index
					// put neighbor
					tQuad[neighboursHit] = neighboursIndexes[p] - nullPixels[neighboursIndexes[p]]; //same here
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

void ModelBuilder::BuildPlans(glm::uvec2 &size, short* depthBuffer)
{
	/* this implementation is based on the paper "Obtaining a best fitting plane through 3D georeferenced data"
		by O. Fernández
	*/
	
	// the distance between a given point and the center of mass
	std::vector<glm::vec3> vectorDistance;
	int bufferSize = size.x * size.y;
	// get center of mass
	glm::vec3 centerMass;
	// X and Y are easy to compute because it's simply the middle of the image
	centerMass.x = size.x / 2;
	centerMass.y = size.y / 2;
	int totalSum = 0;
	for(int u = 0; u < points.size(); u++)
	{
		totalSum += points[u].z;
	}
	// divide by size
	centerMass.z = totalSum / points.size();
	// compute distance between center of mass and the rest of the points
	for(int p = 0; p < points.size(); p++)
	{
		glm::vec3 tVectorDistance = points[p] - centerMass;
		vectorDistance.push_back(tVectorDistance);
	}

	// now we begin to compute the orientation matrix
	glm::dmat3 m_orientation(0.0);
	// now we must sum based on vector distance that we just computed
	for (int b = 0; b < vectorDistance.size(); b++)
	{
		glm::dmat3 t_mOrientation;
		// first line
		t_mOrientation[0][0] = vectorDistance[b].x * vectorDistance[b].x;
		t_mOrientation[0][1] = vectorDistance[b].x * vectorDistance[b].y;
		t_mOrientation[0][2] = vectorDistance[b].x * vectorDistance[b].z;
		// second line
		t_mOrientation[1][0] = vectorDistance[b].y * vectorDistance[b].x;
		t_mOrientation[1][1] = vectorDistance[b].y * vectorDistance[b].y;
		t_mOrientation[1][2] = vectorDistance[b].y * vectorDistance[b].z;
		// third line
		t_mOrientation[2][0] = vectorDistance[b].z * vectorDistance[b].x;
		t_mOrientation[2][1] = vectorDistance[b].z * vectorDistance[b].y;
		t_mOrientation[2][2] = vectorDistance[b].z * vectorDistance[b].z;

		// not sum
		m_orientation += t_mOrientation;

		//SOLVE PROBLEM WITH OVERFLOW
	}

	int thisp = 0;
}		

void ModelBuilder::WriteModelOnFile(std::string &filename,std::string &imagePath)
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
		AddExtensionWithChecking(filename,std::string("obj"),std::string("obj"));

		// ok, there's a name on the file, must check if it's a real one
		modelFile =  fopen(filename.c_str(),"w");
		if (modelFile == NULL)
		{
			MessageBoxA(0,"Cannot acess this file! Check if you have write access on this folder.","Error",(MB_OK | MB_ICONEXCLAMATION));
			return;
		}
	}

	// try to open mtl file
	FILE* materialFile = NULL;
	AddExtensionWithChecking(m_fileName,std::string("obj"),std::string("mtl"));
	materialFile = fopen(m_fileName.c_str(),"w");

	fprintf(modelFile,"%s \n","#Obj generated by OGL_Kinect");
	fprintf(modelFile,"%s \n","#Avaiable at http://code.google.com/p/project-kinect/");
	// put material file
	fprintf(modelFile,"%s","mtllib ");

	RemoveAbsolutePath(m_fileName);

	// now puts on the file
	fprintf(modelFile,"%s\n",m_fileName.c_str());

	// put plane geometry if there's one
	if (!planes.empty())
	{
		// put new group
		fprintf(modelFile,"%s \n","g planesGroup");
		for (int u = 0; u < planes.size(); u++)
		{
			fprintf(modelFile,"%s","v ");
			fprintf(modelFile,"%d",planes[u].x);
			fprintf(modelFile,"%c",' ');
			fprintf(modelFile,"%d",planes[u].y);
			fprintf(modelFile,"%c",' ');
			fprintf(modelFile,"%d \n",planes[u].z);
		}
	}

	// put a group
	fprintf(modelFile,"%s \n","g defaultGroup");

	// put vertices information
	for (int p = 0; p < points.size(); p++)
	{
		fprintf(modelFile,"%s","v ");
		fprintf(modelFile,"%f",points[p].x);
		fprintf(modelFile,"%c",' ');
		fprintf(modelFile,"%f",points[p].y);
		fprintf(modelFile,"%c",' ');
		fprintf(modelFile,"%f \n",points[p].z);
	}

	//put texture coordinates information
	for (int p = 0; p < texturesCoord.size(); p++)
	{	
		fprintf(modelFile,"%s","vt ");
		fprintf(modelFile,"%f",texturesCoord[p].x);
		fprintf(modelFile,"%c",' ');
		fprintf(modelFile,"%f\n",texturesCoord[p].y);
	}

	// put default material
	fprintf(modelFile,"%s\n","usemtl defaultMaterial");
	// put faces information
	for (int b = 0; b < quads.size(); b++)
	{
		fprintf(modelFile,"%s","f");
		for (int t = 0; t < 4; t++)
		{
			fprintf(modelFile,"%c", ' ');
			fprintf(modelFile,"%d",quads[b][t]+1);
			fprintf(modelFile,"%c",'/');
			fprintf(modelFile,"%d",quads[b][t]+1);
		}
		fprintf(modelFile,"\n");
	}

	// isolate image name from absolute image path to refer texture name

	// close model file
	fclose(modelFile);

	// write material file
	fprintf(materialFile,"%s\n","newmtl defaultMaterial");
	fprintf(materialFile,"%s\n","Kd 1.00 1.00 1.00");
	fprintf(materialFile,"%s\n","Ns 0.000000");
	fprintf(materialFile,"%s","map_Kd ");
	// clean image path
	RemoveAbsolutePath(imagePath);
	// write as texture name
	fprintf(materialFile,"%s",imagePath.c_str());


	// close material file
	fclose(materialFile);

	// finish counting time
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << std::endl << "Time to save model on file (seconds): " << elapsed_secs << std::endl;
}