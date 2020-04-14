#pragma once
#include "Submarine.h"
class EnemySub :
	public Submarine
{
public:
	EnemySub();
	bool isColliding(Actor* otherActor, glm::vec3* colPoint, glm::vec3* colPoint2);
	Torpedo* update(float dt, Submarine* target);
	Torpedo* fireTorpedo(glm::vec3 targetPos);
	~EnemySub();
};

