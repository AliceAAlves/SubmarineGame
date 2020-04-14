//OPENGL 3.2 DEMO FOR RENDERING OBJECTS LOADED FROM OBJ FILES

//includes areas for keyboard control, mouse control, resizing the window
//and draws a spinning rectangle

#include <windows.h>		// Header File For Windows
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")

#include <chrono>

#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL


#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "Actor.h"
#include "Submarine.h"
#include "Collectable.h"
#include "Torpedo.h"
#include "Explosion.h"
#include "EnemySub.h"
#include "Camera.h"
//#include "ScreenElements.h"

glm::mat4 objectRotation;
glm::quat q;

const int COLLECTABLES_ON_SCREEN = 3;
int totalCollected = 0;

Shader* myShader;  ///shader object 
Shader* myBasicShader;
Shader* waterShader;
Shader* frameAnimationShader;
Shader* myBasicTextureShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

std::chrono::time_point<std::chrono::high_resolution_clock> currentTime, previousTime;
float deltaTime;

float amount = 0;
float temp = 0.0015f;


Submarine* submarine = new Submarine();
EnemySub* enemy = new EnemySub();
std::vector<Actor*> allActors;
std::vector<Collectable*> collectables;
std::vector<Torpedo*> torpedos;
std::vector<Torpedo*> enemyTorpedos;
std::vector<Explosion*> explosions;
Actor* water = new Actor();
Actor* sky = new Actor();

Camera cam;
//ScreenElements screenElements;

ThreeDModel model, modelbox;
OBJLoader objLoader;
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing
glm::vec3* colPoint = new glm::vec3(0, 0, 0);
glm::vec3* colPoint2 = new glm::vec3(0, 0, 0);
int to_delete_index = -1;

//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
//float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
//float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
float Material_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Material_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { -30.0f, 300.0f, -220.0f, 0.0f };

//SpotLight Properties
float SpotLight_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //{ 1.0f, 0.98f, 0.09f }
float SpotLight_Specular[4] = { 1.0f, 1.0f, 1.0f }; //{ 1.0f, 0.98f, 0.09f }
float SpotLightPos[4] = { 0.0f, -34.0f, -160.0f, 1.0f };
float SpotLightDir[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

//
int	mouse_x = 0, mouse_y = 0;
bool LeftPressed = false;
int screenWidth = 600, screenHeight = 600;
bool keys[256];
float spin = 180;
float speed = 0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void update();				//called in winmain to update variables
void updateTransform(float xinc, float yinc, float zinc);

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = (float)((currentTime - previousTime).count()) * 1000 * std::chrono::high_resolution_clock::period::num /
		std::chrono::high_resolution_clock::period::den; // milliseconds
	previousTime = currentTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	submarine->update(deltaTime);
	glm::mat4 viewingMatrix = glm::lookAt(cam.getCamPos(submarine->getPos()), cam.getCamLook(submarine->getPos()), glm::vec3(0, 1, 0));


	// BASIC SHADER (for sky box)
	glUseProgram(myBasicShader->handle());  // use the shader
	glDepthMask(GL_FALSE);
	sky->display(viewingMatrix, myBasicShader);
	glDepthMask(GL_TRUE);


	// NORMAL SHADER
	glUseProgram(myShader->handle());  // use the shader

	GLuint matLocation = glGetUniformLocation(myShader->handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);

	SpotLightPos[0] = submarine->getSpotLightPos().x;
	SpotLightPos[1] = submarine->getSpotLightPos().y;
	SpotLightPos[2] = submarine->getSpotLightPos().z;
	SpotLightDir[0] = submarine->getSpotLightDirection().x;
	SpotLightDir[1] = submarine->getSpotLightDirection().y;
	SpotLightDir[2] = submarine->getSpotLightDirection().z;
	glUniform4fv(glGetUniformLocation(myShader->handle(), "SpotLightPos"), 1, SpotLightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "SpotLightDir"), 1, SpotLightDir);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "spotlight_ambient"), 1, SpotLight_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "spotlight_specular"), 1, SpotLight_Specular);
	glUniform1f(glGetUniformLocation(myShader->handle(), "spotlight_outercutoff"), cos(glm::radians(17.5f)));
	glUniform1f(glGetUniformLocation(myShader->handle(), "spotlight_cutoff"), cos(glm::radians(15.0f)));

	submarine->display(viewingMatrix, myShader);

	Torpedo* enemyTorpedo = enemy->update(deltaTime, submarine);
	if (enemyTorpedo != nullptr) {
		enemyTorpedos.push_back(enemyTorpedo);
	}

	to_delete_index = -1;
	for (int i = 0; i < allActors.size(); i++) {
		allActors[i]->display(viewingMatrix, myShader);
		if (Collectable* collectable = dynamic_cast<Collectable*>(allActors[i])) {
			if (collectable->isCollected()) to_delete_index = i;
		}
	}
	if (to_delete_index >= 0) allActors.erase(allActors.begin() + to_delete_index);

	to_delete_index = -1;
	for (int i = 0; i < enemyTorpedos.size(); i++) {
		enemyTorpedos[i]->update(deltaTime);
		enemyTorpedos[i]->display(viewingMatrix, myShader);
		if (enemyTorpedos[i]->hasExploded()) to_delete_index = i;
	}
	if (to_delete_index >= 0) enemyTorpedos.erase(enemyTorpedos.begin() + to_delete_index);


	to_delete_index = -1;
	for (int i = 0; i < torpedos.size(); i++) {
		torpedos[i]->update(deltaTime);
		torpedos[i]->display(viewingMatrix, myShader);
		if (torpedos[i]->hasExploded()) to_delete_index = i;
	}
	if (to_delete_index >= 0) torpedos.erase(torpedos.begin() + to_delete_index);

	// FRAME ANIMATION SHADER
	glUseProgram(frameAnimationShader->handle());  // use the shader

	matLocation = glGetUniformLocation(frameAnimationShader->handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(frameAnimationShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	to_delete_index = -1;
	for (int i = 0; i < explosions.size(); i++) {
		explosions[i]->update(deltaTime, cam.getCamPos(submarine->getPos()));
		explosions[i]->display(viewingMatrix, frameAnimationShader);
		if (explosions[i]->isFinished()) to_delete_index = i;
	}
	if (to_delete_index >= 0) explosions.erase(explosions.begin() + to_delete_index);
	/*explosion->update(deltaTime, cam.getCamPos(submarine->getPos()));
	explosion->display(viewingMatrix, frameAnimationShader);*/

	// WATER SHADER
	glUseProgram(waterShader->handle());  // use the shader

	amount += temp * deltaTime;
	if (amount > 6.23)
		amount = 0;
	glUniform1f(glGetUniformLocation(waterShader->handle(), "displacement"), amount);

	matLocation = glGetUniformLocation(waterShader->handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(waterShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(waterShader->handle(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(waterShader->handle(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(waterShader->handle(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(waterShader->handle(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(waterShader->handle(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(waterShader->handle(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(waterShader->handle(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(waterShader->handle(), "material_shininess"), Material_Shininess);

	water->display(viewingMatrix, waterShader);
	//allActors[1]->display(viewingMatrix, myShader);
	//test

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//model.drawBoundingBox(myBasicShader);
	//model.drawOctreeLeaves(myBasicShader);

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(20, 0, 0));

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	//Pass the uniform for the modelview matrix - in this case just "r"
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	modelbox.drawElementsUsingVBO(myShader);

	//screenElements.display(screenWidth, screenHeight, myBasicTextureShader);

	glFlush();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(60.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 20.0f, 2000.0f);
}
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);

	myShader = new Shader;
	//if(!myShader->load("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if (!myShader->load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	myBasicShader = new Shader;
	if (!myBasicShader->load("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	waterShader = new Shader;
	if (!waterShader->load("Water", "glslfiles/waterShader.vert", "glslfiles/waterShader.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	frameAnimationShader = new Shader;
	if (!frameAnimationShader->load("FrameAnimation", "glslfiles/frameAnimationShader.vert", "glslfiles/frameAnimationShader.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	myBasicTextureShader = new Shader;
	if (!myBasicTextureShader->load("BasicTexture", "glslfiles/basicTexture.vert", "glslfiles/basicTexture.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(myShader->handle());  // use the shader

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	submarine->loadModel("TestModels/submarine2.obj", myShader, 4);
	submarine->loadPropellerModel("TestModels/submarine2-propeller.obj", myShader, 1);
	submarine->setPos(0, -5.5, -342);
	submarine->updateTransform(0, 180, 0);

	enemy->loadModel("TestModels/submarine2.obj", myShader, 4);
	enemy->loadPropellerModel("TestModels/submarine2-propeller.obj", myShader, 1);
	enemy->setPos(-60, -71, 207);

	std::vector<char*> filenames;
	filenames.push_back("TestModels/box001.obj");
	filenames.push_back("TestModels/box002.obj");
	filenames.push_back("TestModels/box003.obj");
	filenames.push_back("TestModels/box004.obj");
	filenames.push_back("TestModels/box005.obj");
	filenames.push_back("TestModels/box006.obj");
	filenames.push_back("TestModels/BigRockPart1.obj");
	filenames.push_back("TestModels/BigRockPart2.obj");
	filenames.push_back("TestModels/BigRockPart3.obj");
	filenames.push_back("TestModels/TallRock2.obj");
	filenames.push_back("TestModels/SmallRock.obj");

	for (int i = 0; i < filenames.size(); i++) {
		Actor* scenary = new Actor();
		scenary->loadModel(filenames[i], myShader, 7);
		allActors.push_back(scenary);
	}
	allActors.push_back(enemy);

	int const COLL_OBJ_COUNT = 17;
	float collectableObjs[COLL_OBJ_COUNT][3] = {
		{ 0.0,-7.8,-315.7 },{ -40.7,-30.9,-236.3 },{ 32.4,-61.4,-153.0 },{ 113.7,-92.4,-130.0 },{ 140.7,-104.3,-267.9 },
		{ 59.1,-104.3,-315.0 },{ -41.3,-104.3,-257.0 },{ -4.3,-104.3,-165.5 },{ 21.4,-110.9,-36.2 },{ 64.6,-110.9,64.5 },
		{ 174.4,-110.9,72.5 },{ 121.4,-69.1,168.2 },{ 90.8,-49.0,290.6 },{ -30.2,-78.9,304.6 },{ -69.8,-126.5,216.5 },
		{ -35.9,-126.5,111.0 },{ 18.6,-126.5,28.0 } };

	for (int i = 0; i < COLL_OBJ_COUNT; i++) {
		Collectable* bag = new Collectable();
		if (i == 0) bag->loadStaticModel("TestModels/bag.obj", myShader);
		bag->setPos(collectableObjs[i][0], collectableObjs[i][1], collectableObjs[i][2]);
		collectables.push_back(bag);
		if (i < COLLECTABLES_ON_SCREEN) allActors.push_back(bag);
	}

	Torpedo torpedo(glm::vec3(0, 0, 0), 0, false, glm::vec3(1, 1, 2));
	torpedo.loadStaticModel("TestModels/torpedo2.obj", myShader, 2);
	torpedo.loadPropellerModel("TestModels/torpedo2-heltor.obj", myShader, 1);

	Explosion explosion2(glm::vec3(0, 0, 0));
	explosion2.initialize("TestModels/explosion-sprite-sheet.png", frameAnimationShader);
	//explosion2.initialize("TestModels/eye_gree.png", frameAnimationShader); 

	water->loadModel("TestModels/water.obj", waterShader, 1);
	sky->loadModel("TestModels/SkyBox.obj", myBasicShader, 1);

	cam.setCamRot(180);

	//screenElements.load(myBasicTextureShader);


}
void processKeys()
{
	if (keys['W'])
	{
		submarine->moveForward(deltaTime);
		cam.straightenCam(submarine->getRot(), submarine->getRotSpeed(), deltaTime);
		keys['S'] = false;
	}
	if (keys['S'])
	{
		submarine->moveBackwards(deltaTime);
		cam.straightenCam(submarine->getRot(), submarine->getRotSpeed(), deltaTime);
		keys['W'] = false;
	}
	if (!keys['W'] && !keys['S']) {
		submarine->setMoving(false);
	}
	if (keys['A'])
	{
		submarine->turnLeft(deltaTime);
		cam.rotateCamLeft(submarine->getRot(), submarine->getRotSpeed(), deltaTime);
		keys['D'] = false;
	}
	if (keys['D'])
	{
		submarine->turnRight(deltaTime);
		cam.rotateCamRight(submarine->getRot(), submarine->getRotSpeed(), deltaTime);
		keys['A'] = false;
	}
	if (keys[VK_UP])
	{
		submarine->moveUp(deltaTime);
		keys[VK_DOWN] = false;
	}
	if (keys[VK_DOWN])
	{
		submarine->moveDown(deltaTime);
		keys[VK_UP] = false;
	}
	if (keys[VK_SPACE]) {
		if (submarine->readyToFire())
			torpedos.push_back(submarine->fireTorpedo());
		keys[VK_SPACE] = false;
	}
	if (keys['1'])
	{
		cam.setActiveCam(1, submarine->getRot());
	}
	if (keys['2'])
	{
		cam.setActiveCam(2, submarine->getRot());
	}
	if (keys['3'])
	{
		cam.setActiveCam(3, submarine->getRot());
	}
}
void updateTransform(float xinc, float yinc, float zinc)
{
	glm::mat4 matrixX, matrixXY;

	//rotation about the local x axis
	q = glm::angleAxis(xinc, glm::vec3(objectRotation[0][0], objectRotation[0][1], objectRotation[0][2]));
	matrixX = glm::mat4_cast(q) * objectRotation;

	//EXAMPLE FOR ACCESSING USING A 1D array
	const float *pSource = (const float*)glm::value_ptr(matrixX);
	//rotation about the local y axis
	q = glm::angleAxis(yinc, glm::vec3(pSource[4], pSource[5], pSource[6]));
	matrixXY = glm::mat4_cast(q) * matrixX;

	//EXAMPLE ACCESSING WITH 2D GLM structure.
	//rotation about the local z axis
	q = glm::angleAxis(zinc, glm::vec3(matrixXY[2][0], matrixXY[2][1], matrixXY[2][2]));
	objectRotation = glm::mat4_cast(q) * matrixXY;
}
void update()
{
	submarine->checkAllCollisions(allActors, colPoint, colPoint2);
	enemy->checkAllCollisions(allActors, colPoint, colPoint2);
	for (int i = 0; i < torpedos.size(); i++) {
		Explosion* explosion = torpedos[i]->checkAllCollisions(allActors, colPoint, colPoint2);
		if (explosion != nullptr) explosions.push_back(explosion);
	}
	for (int i = 0; i < enemyTorpedos.size(); i++) {
		std::vector<Actor*> allActorsPlusSub = allActors;
		allActorsPlusSub.push_back(submarine);
		Explosion* explosion = enemyTorpedos[i]->checkAllCollisions(allActorsPlusSub, colPoint, colPoint2);
		if (explosion != nullptr) explosions.push_back(explosion);
	}

	if (totalCollected < Collectable::totalCollected) {
		int index_to_add = totalCollected + COLLECTABLES_ON_SCREEN;
		if (index_to_add < collectables.size()) allActors.push_back(collectables[index_to_add]);
		totalCollected++;
	}
	//ScreenElements::healthBar = submarine->getHealth();
}
/**************** END OPENGL FUNCTIONS *************************/

//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

//win32 global variabless
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application


/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done = false;								// Bool Variable To Exit Loop

	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	// Create Our OpenGL Window
	if (!CreateGLWindow("OpenGL Win32 Example", screenWidth, screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if (keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard

			display();					// Draw The Scene
			update();					// update variables
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
	}


	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}
	break;

	case WM_SIZE:								// Resize The OpenGL Window
	{
		reshape(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	break;

	case WM_LBUTTONDOWN:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
		LeftPressed = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		LeftPressed = false;
	}
	break;

	case WM_MOUSEMOVE:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
	}
	break;
	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = true;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}
	break;
	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = false;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*/

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	glewInit();

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
		cout << " not possible to make context " << endl;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	cout << GLVersionString << endl;

	//OpenGL 3.2 way of checking the version
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height);					// Set Up Our Perspective GL Screen

	init();

	return true;									// Success
}



