#include "twoDImage.h"
#include <iostream>


twoDImage::twoDImage(float w, float h)
{
	width = w;
	height = h;
}

void twoDImage::initialize(char* png_filename, Shader* shader)
{
	nv::Image img;

	// Return true on success
	if (img.loadImageFromFile(png_filename))
	{
		std::cout << "internal format: " << img.getInternalFormat() << std::endl;
		std::cout << "width: " << img.getWidth() << std::endl;
		std::cout << "height: " << img.getHeight() << std::endl;
		std::cout << "format: " << img.getFormat() << std::endl;
		std::cout << "type: " << img.getType() << std::endl;
		std::cout << "level: " << img.getLevel(0) << std::endl;
		std::cout << "texName: " << texName << std::endl;
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);
		std::cout << "texName: " << texName << std::endl;
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

void twoDImage::createGeometry(Shader* shader)
{
	vert[0] = 0.0; vert[1] = height; vert[2] = 0.0f;
	vert[3] = 0.0; vert[4] = 0.0; vert[5] = 0.0f;
	vert[6] = width; vert[7] = 0.0; vert[8] = 0.0f;
	vert[9] = width; vert[10] = height; vert[11] = 0.0f;

	tris[0] = 0; tris[1] = 1; tris[2] = 2;
	tris[3] = 0; tris[4] = 2; tris[5] = 3;

	col[0] = 1.0f; col[1] = 1.0f; col[2] = 1.0f;  col[3] = 1.0f; //r,g,b,a values for each vertex
	col[4] = 1.0f; col[5] = 1.0f; col[6] = 1.0f;  col[7] = 1.0f;
	col[8] = 1.0f; col[9] = 1.0f; col[10] = 1.0f; col[11] = 1.0f;
	col[12] = 1.0f; col[13] = 1.0f; col[14] = 1.0f; col[15] = 1.0f;

	//set texture coordinates (s,t) for each vertex
	tex[0] = 1.0; tex[1] = 0.0;
	tex[2] = 1.0; tex[3] = 1.0;
	tex[4] = 0.0f; tex[5] = 1.0;
	tex[6] = 0.0f; tex[7] = 0.0;


	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(3, m_vboID);

	//get the location of the attribute in the shader - we are interested in colour and position
	GLint vertexLocation = glGetAttribLocation(shader->handle(), "in_Position");
	GLint colorLocation = glGetAttribLocation(shader->handle(), "in_Color");

	std::cout << vertexLocation << std::endl;

	//find the location of the tex coord in the vertex shader
	GLint texCoordLocation = glGetAttribLocation(shader->handle(), "in_TexCoord");

	//set the uniform variable for the texture map
	glUniform1i(glGetUniformLocation(shader->handle(), "DiffuseMap"), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 3 * sizeof(GLfloat), vert, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	std::cout << vertexLocation << std::endl;

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
	std::cout << "geometry created" << std::endl;
}

void twoDImage::display(float x, float y, Shader * shader, glm::mat4 projection)
{
	glUseProgram(shader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &projection[0][0]);

	glDepthFunc(GL_ALWAYS); //ensure text is always displayed

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(x,y,0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);

	//std::cout << "texName: " << texName << std::endl;
	glBindTexture(GL_TEXTURE_2D, texName);

	glBindVertexArray(m_vaoID);		// select first VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, numOfTris * 3, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0); //unbind the vertex array object
	/*glm::vec4 test = projection * model * glm::vec4(vert[0], vert[1], vert[2], 1);
	std::cout << "displayed at " << test.x << "," << test.y << "," << test.z << std::endl;
	test = projection * model * glm::vec4(vert[3], vert[4], vert[5], 1);
	std::cout << "displayed at " << test.x << "," << test.y << "," << test.z << std::endl;*/
	//std::cout << "displayed at " << x << std::endl;
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_BLEND);
}


twoDImage::~twoDImage()
{
}
