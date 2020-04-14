#pragma once
#include <windows.h>		// Header File For Windows
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL
#include "Images/nvImage.h"

#include <chrono>

class Explosion
{
public:
	Explosion(glm::vec3 position);
	void initialize(char* png_filename, Shader* shader);
	void createGeometry(Shader* shader);
	void display(glm::mat4 viewMatrix, Shader* shader);
	void update(float dt, glm::vec3 camPos);
	bool isFinished();
	~Explosion();
protected:
	const static int numOfVerts = 4;
	const static int numOfTris = 2;
	static float vert[12];
	static float col[16];
	static float tex[12];
	static unsigned int tris[6];
	static unsigned int m_vaoID;			// two vertex array objects, one for each drawn object
	static unsigned int m_vboID[4];		// four VBOs
	static GLuint ibo;
	static GLuint texName;				//identifier for the texture
	//std::chrono::time_point<std::chrono::high_resolution_clock> currentTime, previousTime;
	float timeSinceLastFrame;
	int currentFrame = 0;
	glm::vec3 pos;
	glm::mat4 objectRotation;
	glm::mat4 ModelViewMatrix;
	glm::mat4 ModelMatrix;
	bool finished = false;
};

