#pragma once
#include "Actor.h"
#include "Collectable.h"
#include "Explosion.h"


#define _USE_MATH_DEFINES

#include <math.h>
class Torpedo :
	public Actor
{
public:
	Torpedo(glm::vec3 position, float rotation_y, bool enemyTorpedo, glm::vec3 targetPos);
	void display(glm::mat4 viewMatrix, Shader* shader);
	void update(float const dt);
	bool isColliding(Actor* otherActor, glm::vec3* colPoint, glm::vec3* colPoint2);
	Explosion* handleCollision(glm::vec3* colPoint);
	Explosion* checkAllCollisions(std::vector<Actor*> allActors, glm::vec3* colPoint, glm::vec3* colPoint2);
	void loadStaticModel(char * fileName, Shader* shader, int maxLevel);
	void loadPropellerModel(char * fileName, Shader* shader, int maxLevel);
	Octree* getOctree();
	Vector3d* getVerts();
	Vector3d* getFaceNormals();
	aFace* getFaces();
	~Torpedo();
	bool hasExploded();

protected:
	static ThreeDModel staticModel;
	static ThreeDModel propeller_staticModel;
	float propeller_spin;
	float const speed = 0.04f;
	float const propeller_spin_speed = 0.5f;
	bool exploded = false;
	bool isEnemyTorpedo;
	glm::vec3 direction;
	float dir_distance;
};

