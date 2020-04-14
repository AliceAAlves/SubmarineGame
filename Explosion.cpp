#include "Explosion.h"
#include <iostream>

GLuint Explosion::texName;
float Explosion::vert[12];
float Explosion::col[16];
float Explosion::tex[12];
unsigned int Explosion::tris[6];
unsigned int Explosion::m_vaoID;			// two vertex array objects, one for each drawn object
unsigned int Explosion::m_vboID[4];		// four VBOs
GLuint Explosion::ibo;

Explosion::Explosion(glm::vec3 position)
{
	pos = position;
}

void Explosion::initialize(char* png_filename, Shader* shader)
{
	nv::Image img;

	// Return true on success
	if (img.loadImageFromFile(png_filename))
	{
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	createGeometry(shader);
}

void Explosion::createGeometry(Shader* shader)
{
	vert[0] = -5.0f; vert[1] = 5.0f; vert[2] = 0.0f;
	vert[3] = -5.0f; vert[4] = -5.0f; vert[5] = 0.0f;
	vert[6] = 5.0f; vert[7] = -5.0f; vert[8] = 0.0f;
	vert[9] = 5.0f; vert[10] = 5.0f; vert[11] = 0.0f;

	tris[0] = 0; tris[1] = 1; tris[2] = 2;
	tris[3] = 0; tris[4] = 2; tris[5] = 3;

	col[0] = 1.0f; col[1] = 0.0f; col[2] = 0.0f;  col[3] = 1.0f; //r,g,b,a values for each vertex
	col[4] = 1.0f; col[5] = 0.0f; col[6] = 0.0f;  col[7] = 1.0f;
	col[8] = 1.0f; col[9] = 0.0f; col[10] = 0.0f; col[11] = 1.0f;
	col[12] = 1.0f; col[13] = 0.0f; col[14] = 0.0f; col[15] = 1.0f;

	//set texture coordinates (s,t) for each vertex
	tex[0] = 0.0f; tex[1] = 1.0;
	tex[2] = 0.0f; tex[3] = 0.8333;
	tex[4] = 0.125; tex[5] = 0.8333;
	tex[6] = 0.125; tex[7] = 1.0;

	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(3, m_vboID);

	//get the location of the attribute in the shader - we are interested in colour and position
	GLint vertexLocation = glGetAttribLocation(shader->handle(), "in_Position");
	GLint colorLocation = glGetAttribLocation(shader->handle(), "in_Color");

	//find the location of the tex coord in the vertex shader
	GLint texCoordLocation = glGetAttribLocation(shader->handle(), "in_TexCoord");

	//set the uniform variable for the texture map
	glUniform1i(glGetUniformLocation(shader->handle(), "DiffuseMap"), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 3 * sizeof(GLfloat), vert, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 4 * sizeof(GLfloat), col, GL_STATIC_DRAW);

	//set the colour - linked to the colour shader input.
	glEnableVertexAttribArray(colorLocation);
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[2]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 2 * sizeof(GLfloat), tex, GL_STATIC_DRAW);

	//set the texture coordinates
	glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texCoordLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfTris * 3 * sizeof(unsigned int), tris, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Explosion::display(glm::mat4 viewMatrix, Shader * shader)
{
	glUseProgram(shader->handle());  // use the shader

	ModelMatrix = glm::translate(glm::mat4(), pos);
	ModelMatrix = ModelMatrix * objectRotation;
	ModelViewMatrix = viewMatrix * ModelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniform1i(glGetUniformLocation(shader->handle(), "frameNumber"), currentFrame);
	glUniform1f(glGetUniformLocation(shader->handle(), "framesHoriz"), 8.0);
	glUniform1f(glGetUniformLocation(shader->handle(), "framesVert"), 6.0);
	
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	//std::cout << "texName: " << texName << std::endl;
	glBindTexture(GL_TEXTURE_2D, texName);
	//We need blending to allow us to use the alpha channel in the 
	//image

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(m_vaoID);		// select first VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, numOfTris * 3, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0); //unbind the vertex array object
}

void Explosion::update(float dt, glm::vec3 camPos)
{
	objectRotation = glm::transpose(glm::lookAt(glm::vec3(0, 0, 0), camPos - pos, glm::vec3(0.0f, 1.0f, 0.0f)));
	timeSinceLastFrame += dt;
	if (timeSinceLastFrame > 30){
		currentFrame++;
		//std::cout << currentFrame << std::endl;
		//float row = 6.0 - floor(currentFrame / 8.0) - 1.0;
		//float column = currentFrame - (6.0 - row-1.0)*8.0;
		//std::cout << row << std::endl;
		//std::cout << column << std::endl;
		////ex_TexCoord = in_TexCoord + vec2(column*(1/framesHoriz),-(framesVert-row-1)*(1/framesVert));
		//float test1 = tex[0] + column * (1.0 / 8.0);
		//float test2 = tex[1] -(6.0 - row - 1.0)*(1.0 / 6.0);
		//std::cout << test1 << std::endl;
		//std::cout << test2 << std::endl;
		timeSinceLastFrame = 0;
	}
	if (currentFrame == 47) {
		finished = true;
	}
}

bool Explosion::isFinished()
{
	return finished;
}


Explosion::~Explosion()
{
}
