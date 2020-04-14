#pragma once
#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL
//Shader.h has been created using common glsl structure
#include "Obj/OBJLoader.h"

#include "FreeType.h"
#include "Collectable.h"
#include "twoDImage.h"
using namespace freetype;

#include "MatrixRoutines.h"

class ScreenElements
{
public:
	ScreenElements();
	void load(Shader* shader);
	void display(int screenWidth, int screenHeight, Shader* shader);
	~ScreenElements();
	static void reset();

	static float healthBar;
	static int score;
	static bool paused;
	static bool started;
	static bool gameOver;
	static bool gameFinished;

private:
	static Font myFont;
	static Font pausedFont;
	static OBJLoader objLoader;
	Shader* healthBarShader;
	//twoDImage collectablePic = twoDImage(50.0f, 50.0f);
	//twoDImage subPic = twoDImage(195.3f, 70.0f);//533x191
	ThreeDModel collectablePic;
	ThreeDModel subPic;
	ThreeDModel scorePic;
	ThreeDModel healthbarPic;
	ThreeDModel startScreen;
	ThreeDModel gameOverScreen;
	ThreeDModel finishScreen;
};