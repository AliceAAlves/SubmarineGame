#pragma once
#include "Actor.h"
#include "ScreenElements.h"

class Collectable :
	public Actor
{
public:
	Collectable();
	void display(glm::mat4 viewMatrix, Shader* shader);
	void loadStaticModel(char * fileName, Shader* shader);
	Octree* getOctree();
	Vector3d* getVerts();
	Vector3d* getFaceNormals();
	aFace* getFaces();
	static void reset();
	void resetThisCollectable();
	~Collectable();
	static int totalCollectables;
	static int totalCollected;
	bool isCollected();
	void collect();

protected:
	static ThreeDModel staticModel;
	bool collected;
	
};



