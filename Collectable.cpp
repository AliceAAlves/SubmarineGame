#include "Collectable.h"

ThreeDModel Collectable::staticModel;
int Collectable::totalCollectables = 0;
int Collectable::totalCollected = 0;

Collectable::Collectable()
{
	totalCollectables++;
	collected = false;
}

void Collectable::display(glm::mat4 viewMatrix, Shader * shader)
{
	if (collected) return;
	glUseProgram(shader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewMatrix, pos);
	ModelViewMatrix = ModelViewMatrix * objectRotation;

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform1i(glGetUniformLocation(shader->handle(), "hurtFlag"), 0);

	staticModel.drawElementsUsingVBO(shader);
	//model.drawOctreeLeaves(shader);
}

Octree * Collectable::getOctree()
{
	return staticModel.octree;
}
Vector3d * Collectable::getVerts()
{
	return staticModel.theVerts;
}

Vector3d * Collectable::getFaceNormals()
{
	return staticModel.theFaceNormals;
}

aFace * Collectable::getFaces()
{
	return staticModel.theFaces;
}

void Collectable::reset()
{
	totalCollected = 0;
}

void Collectable::resetThisCollectable()
{
	collected = false;
}

void Collectable::loadStaticModel(char * fileName, Shader* shader)
{
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);


	cout << " loading model " << endl;
	if (objLoader.loadModel(fileName, staticModel))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		//model.calcCentrePoint();
		//model.centreOnZero();


		staticModel.calcVertNormalsUsingOctree(0);  //the method will construct the octree if it hasn't already been created.


													 //turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		staticModel.initDrawElements();
		staticModel.initVBO(shader);
		//model.deleteVertexFaceData();

	}
	else
	{
		cout << " model failed to load " << endl;
	}
}


Collectable::~Collectable()
{
}

bool Collectable::isCollected()
{
	return collected;
}

void Collectable::collect()
{
	if (!collected) {
		collected = true;
		totalCollected++;
		ScreenElements::score += 100;
	}
}
