#pragma once
#include <vector>
#include "gl/glew.h"
#include "gl/wglew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL
#include "Images/nvImage.h"
#include <iostream>

class Skybox
{
public:
	Skybox();
	~Skybox();
	static unsigned int cubemapTexture;
	static Shader* cubemapShader;
	static float skyboxVertices[];
	static unsigned int skyboxVAO;
};

unsigned int loadCubemap(std::vector<std::string> faces);

void load();
void display(glm::mat4 viewMatrix);
