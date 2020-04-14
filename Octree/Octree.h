
#ifndef _OCTREE_H
#define _OCTREE_H

#include <vector>
#include <algorithm>
using namespace std;

#include "../3DStruct/threeDModel.h"
#include "../Box.h"
#include "..\glm\glm.hpp"

class Shader;

#define MAX_DEPTH  4

class Octree
{
private:
	double minX, minY, minZ, maxX, maxY, maxZ;
	float radius;
	Octree* children[8];
	int Level;
	int maxLevel;
	int* PrimitiveList;
	int PrimitiveListSize;

	int* VertexList;
	int VertexListSize;

	Box* box;

public:
	Octree();
	~Octree();
	int* getPrimitiveList();
	int getPrimitiveListSize();
	int* getVertexList();
	int getVertexListSize();
	void Delete();
	void set(int L, double x, double y, double z, double X, double Y, double Z, int* PrimList, int PrimListSize, int* vertList, int VertListSize);
	void setMaxLevel(int max);
	void start(int L, double x, double y, double z, double X, double Y, double Z, ThreeDModel* model);
	void CreateChildren(vector<Octree*> &stackOctree, ThreeDModel* model);
	void CalculatePrimitiveWithinSubdividedOctreeCells(ThreeDModel* model, int primitiveIndex, int** primitiveSets, int primitiveSetsCount[8]);
	void CalculateVerticesWithinSubdividedOctreeCells(ThreeDModel* model, int vertexIndex, int** vertexSets, int vertexSetsCount[8]);
	void drawAllBoxes(Shader* myShader);
	void drawBoxesAtLeaves(Shader* myShader);
	void drawBoundingBox(Shader* myShader);
	Octree* getChild(int i);
	int getLevel();
	int getMaxLevel();
	void processVerticesByLeaf(ThreeDModel* model);
	glm::vec3 getBoxCenter();
	glm::vec3 getBoxSize();
	float getMaxBoxSize();
	float Octree::getBoxRadius();
	glm::vec3 getBoxMins();
	glm::vec3 getBoxMaxs();
};

#endif _OCTREE_H