#include "ScreenElements.h"

float ScreenElements::healthBar;
int ScreenElements::score;
bool ScreenElements::paused;
bool ScreenElements::started;
bool ScreenElements::gameOver;
bool ScreenElements::gameFinished;
Font ScreenElements::myFont;
Font ScreenElements::pausedFont;
OBJLoader ScreenElements::objLoader;

ScreenElements::ScreenElements()
{
}

void ScreenElements::load(Shader* shader)
{
	Font::loadShader("glslfiles/basicTexture"); //load a shader for use rendering text
	float colorWhite[] = { 1.0, 1.0, 1.0 };
	float colorRed[] = { 1.0, 0.0, 0.0 };
	myFont.init("fonts/digital-7.ttf", 36, colorWhite);	 //load a font 
	pausedFont.init("fonts/arialbd.ttf", 40, colorWhite);	 //load a font 
	
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);

	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/bag-picture.obj", collectablePic))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		collectablePic.initDrawElements();
		collectablePic.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/score-pic.obj", scorePic))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		scorePic.initDrawElements();
		scorePic.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/sub-pic.obj", subPic))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		subPic.initDrawElements();
		subPic.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	healthBarShader = new Shader();
	if (!healthBarShader->load("HealthBarShader", "glslfiles/healthBar.vert", "glslfiles/healthBar.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/health-bar.obj", healthbarPic))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		healthbarPic.initDrawElements();
		healthbarPic.initVBO(healthBarShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	// LOAD THE SCREEN MODELS

	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/screen-start.obj", startScreen))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		startScreen.initDrawElements();
		startScreen.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/screen-finish.obj", finishScreen))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		finishScreen.initDrawElements();
		finishScreen.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	cout << " loading model " << endl;
	if (objLoader.loadModel("TestModels/screen-gameover.obj", gameOverScreen))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		gameOverScreen.initDrawElements();
		gameOverScreen.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	/*shader2 = new Shader();
	if (!shader2->load("BasicTexture", "glslfiles/basicTexture.vert", "glslfiles/basicTexture.frag"))
	{
		cout << "failed to load shader" << endl;
	}*/
	//collectablePic.initialize("TestModels/bag-picture.png", shader);
	//subPic.initialize("TestModels/sub-picture.png", shader);
}

void ScreenElements::display(int screenWidth, int screenHeight,Shader* shader)
{
	glUseProgram(Font::myShader->handle());

	//change coordinate system to work in 2d
	float twoDprojection[16];
	MatrixRoutines<float>::Ortho2D(0.0, (float)screenWidth, 0.0, (float)screenHeight, twoDprojection);

	//set the projection matrix to the uniform variable used for the font shader
	glUniformMatrix4fv(glGetUniformLocation(Font::myShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, twoDprojection);
	//font, x position, y position, string of text and a float
	print(myFont, 170, screenHeight-50, "%d", score);
	print(myFont, 80, screenHeight-100, "%d",Collectable::totalCollected);
	//print(myFont, screenWidth-120, screenHeight - 50-65, "H: %3.0f", healthBar*100);
	if(paused) print(pausedFont, screenWidth / 2 - 105, screenHeight / 2 - 19, "PAUSED");

	glDepthFunc(GL_ALWAYS); //ensure picture is always displayed

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader->handle());
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ProjectionMatrix"), 1, GL_FALSE, twoDprojection);
	glm::mat4 model;
	model = glm::translate(glm::mat4(), glm::vec3(20, screenHeight - 110, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
	collectablePic.drawElementsUsingVBO(shader);

	model = glm::translate(glm::mat4(), glm::vec3(20, screenHeight - 47, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
	scorePic.drawElementsUsingVBO(shader);

	model = glm::translate(glm::mat4(), glm::vec3(screenWidth - 180, screenHeight - 65, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
	subPic.drawElementsUsingVBO(shader);

	glUseProgram(healthBarShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(healthBarShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, twoDprojection);
	model = glm::translate(glm::mat4(), glm::vec3(screenWidth - 142, screenHeight - 55, 0.0f));
	glUniform1f(glGetUniformLocation(healthBarShader->handle(), "health"), healthBar);
	glUniformMatrix4fv(glGetUniformLocation(healthBarShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
	healthbarPic.drawElementsUsingVBO(healthBarShader);

	glUseProgram(shader->handle());
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ProjectionMatrix"), 1, GL_FALSE, twoDprojection);
	if (!started) {
		model = glm::translate(glm::mat4(), glm::vec3(screenWidth/2 - 200, screenHeight/2 - 146.5, 0.0f)); 
		glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
		startScreen.drawElementsUsingVBO(shader);
		//cout << "game hasn't started" << endl;
	}
	if (gameOver) {
		model = glm::translate(glm::mat4(), glm::vec3(screenWidth / 2 - 200, screenHeight / 2 - 146.5, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
		gameOverScreen.drawElementsUsingVBO(shader);
		//cout << "game over" << endl;
	}
	if (gameFinished) {
		model = glm::translate(glm::mat4(), glm::vec3(screenWidth/2 - 200, screenHeight/2 - 146.5, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &model[0][0]);
		finishScreen.drawElementsUsingVBO(shader);
		//cout << "you win" << endl;
	}

	glDepthFunc(GL_LEQUAL);

	//glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
	////projection = glm::perspective(60.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 20.0f, 2000.0f);
	//glUseProgram(shader->handle());
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &projection[0][0]);
	////collectablePic.display(10, 60,shader, projection);
	//subPic.display(screenWidth- 195-20, 20, shader, projection);
}


ScreenElements::~ScreenElements()
{
}

void ScreenElements::reset()
{
	healthBar = 1.0;
	score = 0;
	started = false;
	gameOver = false;
	gameFinished = false;
	Collectable::reset();

}


//void glEnable2D()
//{
//	int vPort[4];
//
//	glGetIntegerv(GL_VIEWPORT, vPort);
//
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//
//	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//}
//
//void glDisable2D()
//{
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
//}
//
//
//void RenderScene()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//
//	glEnable2D();
//	glBegin(GL_TRIANGLES);
//	glColor3ub(255, 0, 0);
//	glVertex2d(0, 0);
//	glColor3ub(0, 255, 0);
//	glVertex2d(100, 0);
//	glColor3ub(0, 0, 255);
//	glVertex2d(50, 50);
//	glEnd();
//	glDisable2D();
//}
