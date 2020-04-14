#pragma once
#include <windows.h>		// Header File For Windows
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "..//Structures//Vector3d.h"
#include "Octree/Octree.h"

#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL
//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
class Actor
{
public:
	Actor();
	virtual void display(glm::mat4 viewMatrix, Shader* shader);
	glm::vec3 getPos();
	float getRot();
	void setPos(float x, float y, float z);
	void updateTransform(float xinc, float yinc, float zinc);
	void loadModel(char * fileName, Shader* shader,int maxLevel);
	virtual Octree* getOctree();
	virtual bool isColliding(Actor* otherActor, glm::vec3* colPoint, glm::vec3* colPoint2);
	virtual void update();
	virtual Vector3d* getVerts();
	virtual Vector3d* getFaceNormals();
	virtual aFace* getFaces();
	glm::mat4 getMVMatrix();
	~Actor();

	static OBJLoader objLoader;
protected:
	glm::vec3 pos;
	glm::mat4 objectRotation;
	glm::quat q;
	ThreeDModel model;
	glm::mat4 ModelViewMatrix;
	glm::mat4 ModelMatrix;
	float rotation;
	float rotation_x;
};

bool sphereColision(glm::vec3 c1, float r1, glm::vec3 c2, float r2);

bool octreeCollision(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2);

bool octreeCollisionWithGeometry(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2, glm::vec3* colFaceNormal, int* numTris,
	Vector3d* theVerts1, Vector3d* theVerts2, Vector3d* theFaceNormals1, Vector3d* theFaceNormals2, aFace* theFaces1, aFace* theFaces2);

bool octreeSphereCollision(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, float r2);

bool satCollision(Octree * ot1, Octree * ot2, glm::mat4 transform1, glm::mat4 transform2);

bool intersectsWhenProjected(vector<glm::vec3> aCorners, vector<glm::vec3> bCorners, glm::vec3 axis);

bool geometryCollision(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2, glm::vec3* colFaceNormal, int* numTris,
	Vector3d* theVerts1, Vector3d* theVerts2, Vector3d* theFaceNormals1, Vector3d* theFaceNormals2, aFace* theFaces1, aFace* theFaces2);

Vector3d faceMiddlePoint(aFace face, Vector3d* verts);

bool facesAdjacentAndConvex(aFace* faces, Vector3d* verts, Vector3d* faceNormals, unsigned int face1, unsigned int face2);