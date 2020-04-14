#pragma once
#include "Actor.h"
#include "Collectable.h"
#include "Torpedo.h"
#include <vector>

#define _USE_MATH_DEFINES

#include <math.h>
class Submarine :
	public Actor
{
public:
	Submarine();
	void display(glm::mat4 viewMatrix, Shader* shader);
	void update(float const dt);
	void moveForward(float const dt);
	void moveBackwards(float const dt);
	void turnLeft(float const dt);
	void turnRight(float const dt);
	void moveUp(float const dt);
	void moveDown(float const dt);
	void setMoving(bool moving);
	void setMovingForward(bool moving);
	void setMovingVertically(bool moving);
	float getRotSpeed();
	void setRotation(float newRotation);
	void reset();
	glm::vec3 getSpotLightPos();
	glm::vec3 getSpotLightDirection();
	virtual bool isColliding(Actor* otherActor, glm::vec3* colPoint, glm::vec3* colPoint2, glm::vec3* colFaceNormal, int* numTris);
	void handleCollision(glm::vec3* colPoint, glm::vec3* colFaceNormal, float dt, int numberTris);
	void checkAllCollisions(std::vector<Actor*> allActors, glm::vec3* colPoint, glm::vec3* colPoint2, glm::vec3* colFaceNormal, float dt, int* numTris);
	void loadPropellerModel(char * fileName, Shader* shader, int maxLevel);
	Torpedo* fireTorpedo();
	bool readyToFire();
	bool isHurt();
	float getHealth();
	void takeDamage(float damage);
	bool isDefeated();
	~Submarine();

protected:
	ThreeDModel propeller_model;
	float speed, colRotSpeed, propeller_spin, propeller_spin_speed, vertSpeed;
    float maxSpeed = 0.03f;
	float const maxbwSpeed = 0.025f;
	float const maxVertSpeed = 0.01f;
	float rotSpeed = 0.02f;
	float const accel = 0.00002f;
	float const accelVert = 0.000004f;
	float const rotLoss = 0.0001f;
	float const propeller_spin_maxspeed = 0.5f;
	float const propeller_spin_accel = 0.001f;
	float const rotation_x_cap = 4.0f;
	bool isMoving, isMovingForwards, isMovingVertically;
	bool canMoveForwards,canMoveBackwards, canTurnLeft, canTurnRight, canMoveUp, canMoveDown;
	float timeSinceLastTorpedoFired;
	float timeCapToNextTorpedo;
	float health = 1.0f;
	float timeSinceGotHurt;
	float timeCapForHurt = 2000.0f;
	bool defeated = false;
	bool hurt = false;
};

