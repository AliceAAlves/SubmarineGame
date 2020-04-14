#pragma once
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL
#include "Images/nvImage.h"
#include "MatrixRoutines.h"

class twoDImage
{
public:
	twoDImage(float w, float h);
	~twoDImage();
	void initialize(char* png_filename, Shader* shader);
	void createGeometry(Shader* shader);
	void display(float x, float y, Shader * shader, glm::mat4 projection);
	float width, height;
protected:
	const int numOfVerts = 4;
	const int numOfTris = 2;
	float vert[12];
	float col[16];
	float tex[12];
	unsigned int tris[6];
	unsigned int m_vaoID;			// two vertex array objects, one for each drawn object
	unsigned int m_vboID[4];		// four VBOs
	GLuint ibo;
	GLuint texName;				//identifier for the texture
};

