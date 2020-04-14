#include "../gl/glew.h"
#include <iostream>
#include <vector>
using namespace std;

#include "Octree.h"
#include "../3DStruct/threeDModel.h"
#include <math.h>

const int FACE_LIMIT = 1;

Octree::Octree()
{
	minX = 0;
	minY = 0;
	minZ = 0;

	maxX = 0;
	maxY = 0;
	maxZ = 0;

	PrimitiveListSize=-1;
	PrimitiveList = NULL;

	VertexListSize = -1;
	VertexList = NULL;

	Level = 0;
	maxLevel = 4;

	for(int i=0; i<8; i++)
	{
		children[i] = NULL;
	}

	box = NULL;
}

Octree::~Octree()
{
}

void Octree::Delete()
{
	if(Level >= maxLevel)
	{
		delete[] PrimitiveList;
		PrimitiveListSize = 0;

		delete[] VertexList;
		VertexListSize = 0;
	}
	else
	{
		for(int i=0; i<8; i++)
		{
			if(children[i] != NULL)
			{
				children[i]->Delete();
				delete children[i];
			}
		}
		delete[] PrimitiveList;
		delete[] VertexList;
	}
}

void Octree::set(int L, double x, double y, double z, double X, double Y, double Z, int* PrimList, int PrimListSize, int* VertList, int VertListSize)
{
	Level = L;

	minX = x;
	minY = y;
	minZ = z;

	maxX = X;
	maxY = Y;
	maxZ = Z;

	glm::vec3 center;
	center.x = (maxX + minX) / 2.0;
	center.y = (maxY + minY) / 2.0;
	center.z = (maxZ + minZ) / 2.0;
	radius = sqrt((center.x - x)*(center.x - x) + (center.y - y)*(center.y - y) + (center.z - z)*(center.z - z));

	if(PrimListSize>0)
	{
		PrimitiveListSize = PrimListSize;

		PrimitiveList = new int[PrimitiveListSize];

		memcpy(PrimitiveList, PrimList, sizeof(int)*PrimitiveListSize);
	}
	else
		PrimitiveListSize = 0;

	if(VertListSize>0)
	{
		VertexListSize = VertListSize;

		VertexList = new int[VertexListSize];

		memcpy(VertexList, VertList, sizeof(int)*VertexListSize);
	}
	else
		VertexListSize = 0;
}

void Octree::setMaxLevel(int max)
{
	if (max < 0) return;
	else maxLevel = max;
}

void Octree::start(int L, double x, double y, double z, double X, double Y, double Z, ThreeDModel* obj)
{
	Level = L;

	minX = x;
	minY = y;
	minZ = z;

	maxX = X;
	maxY = Y;
	maxZ = Z;

	glm::vec3 center;
	center.x = (maxX + minX) / 2.0;
	center.y = (maxY + minY) / 2.0;
	center.z = (maxZ + minZ) / 2.0;
	radius = sqrt((center.x - x)*(center.x - x) + (center.y - y)*(center.y - y) + (center.z - z)*(center.z - z));

	PrimitiveListSize = obj->getOctreePrimitiveListSize();

	if(PrimitiveListSize>0)
	{
		PrimitiveList = new int[PrimitiveListSize];

		//memcpy(PrimitiveList, obj->getOctreePrimitiveList(), sizeof(int)*PrimitiveListSize);
		for(int i=0;i<PrimitiveListSize; i++)
		{
			PrimitiveList[i] = i;
		}
	}

	VertexListSize = obj->getOctreeVertexListSize();

	if(VertexListSize>0)
	{
		VertexList = new int[VertexListSize];

		for(int i=0;i<VertexListSize; i++)
		{
			VertexList[i] = i;
		}
	}
}

void Octree::CreateChildren(vector<Octree*> &stackOctree, ThreeDModel* obj)
{
	if(Level >= maxLevel)
	{
		return;
	}
	int PrimitiveSetsCount[8];
	int** PrimitiveSets = new int*[8];
	for(int i=0; i<8; i++)
	{
		PrimitiveSets[i] = new int[PrimitiveListSize];
		PrimitiveSetsCount[i] = 0;
		children[i] = NULL;
	}

	int VertexSetsCount[8];
	int** VertexSets = new int*[8];
	for(int i=0; i<8; i++)
	{
		VertexSets[i] = new int[VertexListSize];
		VertexSetsCount[i] = 0;
	}

	
	for(int i=0; i<VertexListSize; i++)
	{
		CalculateVerticesWithinSubdividedOctreeCells(obj, VertexList[i], VertexSets, VertexSetsCount);
	}

	for(int i=0; i<PrimitiveListSize; i++)
	{
		CalculatePrimitiveWithinSubdividedOctreeCells(obj, PrimitiveList[i], PrimitiveSets, PrimitiveSetsCount);
	}

	if(PrimitiveSetsCount[0] >0)
	{
		children[0] = new Octree();
		children[0]->setMaxLevel(maxLevel);
		children[0]->set(Level+1, minX, minY, minZ, (maxX+minX)/2.0, (maxY+minY)/2.0, (minZ+maxZ)/2.0, PrimitiveSets[0], PrimitiveSetsCount[0], VertexSets[0], VertexSetsCount[0]);
		
		stackOctree.push_back(children[0]);
	}

	if(PrimitiveSetsCount[1] >0)
	{
		children[1] = new Octree();
		children[1]->setMaxLevel(maxLevel);
		children[1]->set(Level+1, minX, minY, (minZ+maxZ)/2.0, (maxX+minX)/2.0, (maxY+minY)/2.0, maxZ, PrimitiveSets[1], PrimitiveSetsCount[1], VertexSets[1], VertexSetsCount[1]);
		stackOctree.push_back(children[1]);
	}

	if(PrimitiveSetsCount[2] >0)
	{
		children[2] = new Octree();
		children[2]->setMaxLevel(maxLevel);
		children[2]->set(Level+1, minX, (maxY+minY)/2.0, minZ, (maxX+minX)/2.0, maxY, (minZ+maxZ)/2.0, PrimitiveSets[2], PrimitiveSetsCount[2], VertexSets[2], VertexSetsCount[2]);
		stackOctree.push_back(children[2]);
	}

	if(PrimitiveSetsCount[3] >0)
	{
		children[3] = new Octree();
		children[3]->setMaxLevel(maxLevel);
		children[3]->set(Level+1, minX, (maxY+minY)/2.0, (minZ+maxZ)/2.0, (maxX+minX)/2.0, maxY, maxZ, PrimitiveSets[3], PrimitiveSetsCount[3], VertexSets[3], VertexSetsCount[3]);
		stackOctree.push_back(children[3]);
	}

	if(PrimitiveSetsCount[4] >0)
	{
		children[4] = new Octree();
		children[4]->setMaxLevel(maxLevel);
		children[4]->set(Level+1, (maxX+minX)/2.0, minY, minZ, maxX, (maxY+minY)/2.0, (minZ+maxZ)/2.0, PrimitiveSets[4], PrimitiveSetsCount[4], VertexSets[4], VertexSetsCount[4]);
		stackOctree.push_back(children[4]);
	}

	if(PrimitiveSetsCount[5] > 0)
	{
		children[5] = new Octree();
		children[5]->setMaxLevel(maxLevel);
		children[5]->set(Level+1, (maxX+minX)/2.0, minY, (minZ+maxZ)/2.0, maxX, (maxY+minY)/2.0, maxZ, PrimitiveSets[5], PrimitiveSetsCount[5], VertexSets[5], VertexSetsCount[5]);
		stackOctree.push_back(children[5]);
	}

	if(PrimitiveSetsCount[6] > 0)
	{
		children[6] = new Octree();
		children[6]->setMaxLevel(maxLevel);
		children[6]->set(Level+1, (maxX+minX)/2.0, (maxY+minY)/2.0, minZ, maxX, maxY, (minZ+maxZ)/2.0, PrimitiveSets[6], PrimitiveSetsCount[6], VertexSets[6], VertexSetsCount[6]);
		stackOctree.push_back(children[6]);
	}

	if(PrimitiveSetsCount[7] > 0)
	{
		children[7] = new Octree();
		children[7]->setMaxLevel(maxLevel);
		children[7]->set(Level+1, (maxX+minX)/2.0, (maxY+minY)/2.0, (minZ+maxZ)/2.0, maxX, maxY, maxZ, PrimitiveSets[7], PrimitiveSetsCount[7], VertexSets[7], VertexSetsCount[7]);
		stackOctree.push_back(children[7]);
	}

	for(int i=0; i<8; i++)
	{
		delete [] PrimitiveSets[i];
	}

	delete [] PrimitiveSets;

	for(int i=0; i<8; i++)
	{
		delete [] VertexSets[i];
	}

	delete [] VertexSets;
}

void Octree::CalculateVerticesWithinSubdividedOctreeCells(ThreeDModel* obj, int vertexIndex, int** vertexSets, int vertexSetsCount[8])
{
	//determine which vertices are in which boxes
	double min[3];
	double max[3];

	min[0] = minX; 
	min[1] = minY; 
	min[2] = minZ; 
	max[0] = (minX+maxX)/2.0; 
	max[1] = (maxY+minY)/2.0; 
	max[2] = (minZ+maxZ)/2.0;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[0][vertexSetsCount[0]] = vertexIndex;
		vertexSetsCount[0]++;
	}

	min[0] = minX; 
	min[1] = minY; 
	min[2] = (minZ+maxZ)/2.0; 
	max[0] = (minX+maxX)/2.0; 
	max[1] = (maxY+minY)/2.0; 
	max[2] = maxZ;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[1][vertexSetsCount[1]] = vertexIndex;
		vertexSetsCount[1]++;
	}

	min[0] = minX; 
	min[1] = ((maxY+minY)/2.0); 
	min[2] = minZ; 
	max[0] = (minX+maxX)/2.0; 
	max[1] = maxY; 
	max[2] = (minZ+maxZ)/2.0;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[2][vertexSetsCount[2]] = vertexIndex;
		vertexSetsCount[2]++;
	}

	min[0] = minX; 
	min[1] = ((maxY+minY)/2.0); 
	min[2] = (maxZ+minZ)/2.0; 
	max[0] = (minX+maxX)/2.0; 
	max[1] = maxY; 
	max[2] = maxZ;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[3][vertexSetsCount[3]] = vertexIndex;
		vertexSetsCount[3]++;
	}

	min[0] = (minX+maxX)/2.0; 
	min[1] = minY; 
	min[2] = minZ; 
	max[0] = maxX; 
	max[1] = ((maxY+minY)/2.0); 
	max[2] = (maxZ+minZ)/2.0;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[4][vertexSetsCount[4]] = vertexIndex;
		vertexSetsCount[4]++;
	}

	min[0] = (minX+maxX)/2.0; 
	min[1] = minY; 
	min[2] = (maxZ+minZ)/2.0; 
	max[0] = maxX; 
	max[1] = ((maxY+minY)/2.0); 
	max[2] = maxZ;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[5][vertexSetsCount[5]] = vertexIndex;
		vertexSetsCount[5]++;
	}

	min[0] = (minX+maxX)/2.0; 
	min[1] =((maxY+minY)/2.0); 
	min[2] = minZ; 
	max[0] = maxX; 
	max[1] = maxY; 
	max[2] = (minZ+maxZ)/2.0;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[6][vertexSetsCount[6]] = vertexIndex;
		vertexSetsCount[6]++;
	}

	min[0] = (minX+maxX)/2.0; 
	min[1] =((maxY+minY)/2.0); 
	min[2] = (minZ+maxZ)/2.0; 
	max[0] = maxX; 
	max[1] = maxY; 
	max[2] = maxZ;
	
	if(obj->isVertexIntersectingOctreeCell(min, max, vertexIndex))
	{
		vertexSets[7][vertexSetsCount[7]] = vertexIndex;
		vertexSetsCount[7]++;
	}
}

void Octree::CalculatePrimitiveWithinSubdividedOctreeCells(ThreeDModel* obj, int primitiveIndex, int** primitiveSets, int primitiveSetsCount[8])
{
	double boxCenter[3];
	double boxHalfSize[3];

	double inc = 1.001;

	boxCenter[0] = ((maxX+minX)/2.0 + minX)/2.0;
	boxCenter[1] = ((maxY+minY)/2.0 + minY)/2.0;
	boxCenter[2] = ((minZ+maxZ)/2.0 + minZ)/2.0;
	boxHalfSize[0] = (boxCenter[0] - minX);
	boxHalfSize[1] = (boxCenter[1] - minY);
	boxHalfSize[2] = (boxCenter[2] - minZ);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[0][primitiveSetsCount[0]] = primitiveIndex;
		primitiveSetsCount[0]++;
	}

	boxCenter[0] = ((maxX+minX)/2.0 + minX)/2.0;
	boxCenter[1] = ((maxY+minY)/2.0 + minY)/2.0;
	boxCenter[2] = (maxZ + ((minZ+maxZ)/2.0))/2.0;
	boxHalfSize[0] = boxCenter[0] - minX;
	boxHalfSize[1] = boxCenter[1] - minY;
	boxHalfSize[2] = boxCenter[2] - (minZ+maxZ)/2.0;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[1][primitiveSetsCount[1]] = primitiveIndex;
		primitiveSetsCount[1]++;
	}

	boxCenter[0] = ((maxX+minX)/2.0 + minX)/2.0;
	boxCenter[1] = (maxY + ((maxY+minY)/2.0))/2.0;
	boxCenter[2] = ((minZ+maxZ)/2.0 + minZ)/2.0;
	boxHalfSize[0] = boxCenter[0] - minX;
	boxHalfSize[1] = boxCenter[1] - ((maxY+minY)/2.0);
	boxHalfSize[2] = boxCenter[2] - minZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[2][primitiveSetsCount[2]] = primitiveIndex;
		primitiveSetsCount[2]++;
	}

	boxCenter[0] = ((maxX+minX)/2.0 + minX)/2.0;
	boxCenter[1] = (maxY + ((maxY+minY)/2.0))/2.0;
	boxCenter[2] = (maxZ + ((maxZ+minZ)/2.0))/2.0;
	boxHalfSize[0] = boxCenter[0] - minX;
	boxHalfSize[1] = boxCenter[1] - ((maxY+minY)/2.0);
	boxHalfSize[2] = boxCenter[2] - ((maxZ+minZ)/2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[3][primitiveSetsCount[3]] = primitiveIndex;
		primitiveSetsCount[3]++;
	}

	boxCenter[0] = (maxX + ((maxX+minX)/2.0))/2.0;
	boxCenter[1] = ((maxY+minY)/2.0 + minY)/2.0;
	boxCenter[2] = ((minZ+maxZ)/2.0 + minZ)/2.0;
	boxHalfSize[0] = boxCenter[0] - ((maxX+minX)/2.0);
	boxHalfSize[1] = boxCenter[1] - minY;
	boxHalfSize[2] = boxCenter[2] - minZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[4][primitiveSetsCount[4]] = primitiveIndex;
		primitiveSetsCount[4]++;
	}

	boxCenter[0] = (maxX + ((maxX+minX)/2.0))/2.0;
	boxCenter[1] = ((maxY+minY)/2.0 + minY)/2.0;
	boxCenter[2] = (maxZ + ((maxZ+minZ)/2.0))/2.0;
	boxHalfSize[0] = boxCenter[0] - ((maxX+minX)/2.0);
	boxHalfSize[1] = boxCenter[1] - minY;
	boxHalfSize[2] = boxCenter[2] - ((maxZ+minZ)/2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[5][primitiveSetsCount[5]] = primitiveIndex;
		primitiveSetsCount[5]++;
	}

	boxCenter[0] = (maxX + ((maxX+minX)/2.0))/2.0;
	boxCenter[1] = (maxY + ((maxY+minY)/2.0))/2.0;
	boxCenter[2] = ((minZ+maxZ)/2.0 + minZ)/2.0;
	boxHalfSize[0] = boxCenter[0] - ((maxX+minX)/2.0);
	boxHalfSize[1] = boxCenter[1] - ((maxY+minY)/2.0);
	boxHalfSize[2] = boxCenter[2] - minZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[6][primitiveSetsCount[6]] = primitiveIndex;
		primitiveSetsCount[6]++;
	}

	boxCenter[0] = (maxX + ((maxX+minX)/2.0))/2.0;
	boxCenter[1] = (maxY + ((maxY+minY)/2.0))/2.0;
	boxCenter[2] = (maxZ + ((maxZ+minZ)/2.0))/2.0;
	boxHalfSize[0] = boxCenter[0] - ((maxX+minX)/2.0);
	boxHalfSize[1] = boxCenter[1] - ((maxY+minY)/2.0);
	boxHalfSize[2] = boxCenter[2] - ((maxZ+minZ)/2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if(obj->isPrimitiveIntersectingOctreeCell(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[7][primitiveSetsCount[7]] = primitiveIndex;
		primitiveSetsCount[7]++;
	}
}



void Octree::drawBoundingBox(Shader* myShader)
{
	if(box == NULL)
	{
		box = new Box();
		box->constructGeometry(myShader, static_cast<float>(minX), static_cast<float>(minY), static_cast<float>(minZ), static_cast<float>(maxX), static_cast<float>(maxY), static_cast<float>(maxZ));
	}
	else
	{
		box->render();
	}
}

void Octree::drawAllBoxes(Shader* myShader)
{
	if(Level >= maxLevel) //leaf
	{
		drawBoundingBox(myShader);
	}
	else
	{
		for(int i=0; i<8; i++)
		{
			if(children[i] != NULL)
				children[i]->drawAllBoxes(myShader);
		}
		drawBoundingBox(myShader);
	}
}

void Octree::processVerticesByLeaf(ThreeDModel* model)
{
	if(Level >= maxLevel) //leaf
	{
		if(VertexListSize > 0) //only draw boxes which contain vertices
		{
			model->calcVertNormals(VertexList, VertexListSize, PrimitiveList, PrimitiveListSize);
		}
	}
	else
	{
		for(int i=0; i<8; i++)
		{
			if(children[i] != NULL)
				children[i]->processVerticesByLeaf(model);
		}
	}
}

glm::vec3 Octree::getBoxCenter()
{
	glm::vec3 boxCenter;
	/*boxCenter.x = (maxX + ((maxX + minX) / 2.0)) / 2.0;
	boxCenter.y = (maxY + ((maxY + minY) / 2.0)) / 2.0;
	boxCenter.z = (maxZ + ((maxZ + minZ) / 2.0)) / 2.0;*/
	boxCenter.x = (maxX + minX) / 2.0;
	boxCenter.y = (maxY + minY) / 2.0;
	boxCenter.z = (maxZ + minZ) / 2.0;
	
	return boxCenter;
}

glm::vec3 Octree::getBoxSize()
{
	glm::vec3 boxSize;
	boxSize.x = (maxX - minX);
	boxSize.y = (maxY - minY);
	boxSize.z = (maxZ - minZ);

	return boxSize;
}

float Octree::getMaxBoxSize()
{
	glm::vec3 boxSize = getBoxSize();
	return max(max(boxSize.x, boxSize.y),boxSize.z);
}

float Octree::getBoxRadius()
{
	//glm::vec3 a = getBoxCenter();
	//glm::vec3 b = getBoxMins();
	//return sqrt((a.x-b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
	return radius;
}

glm::vec3 Octree::getBoxMins()
{
	return glm::vec3(minX,minY,minZ);
}

glm::vec3 Octree::getBoxMaxs()
{
	return glm::vec3(maxX, maxY, maxZ);
}

void Octree::drawBoxesAtLeaves(Shader* myShader)
{
	if(Level >= maxLevel) //leaf
	{
		if(PrimitiveListSize > 0) //only draw boxes which contain vertices
		{
			drawBoundingBox(myShader);
		}
	}
	else
	{
		for(int i=0; i<8; i++)
		{
			if(children[i] != NULL)
				children[i]->drawBoxesAtLeaves(myShader);
		}
	}
}

Octree* Octree::getChild(int i)
{
	return children[i];
}

int Octree::getLevel()
{
	return Level;
}

int Octree::getMaxLevel()
{
	return maxLevel;
}

int* Octree::getPrimitiveList()
{
	return PrimitiveList;
}

int Octree::getPrimitiveListSize()
{
	return PrimitiveListSize;
}

int * Octree::getVertexList()
{
	return VertexList;
}

int Octree::getVertexListSize()
{
	return VertexListSize;
}

