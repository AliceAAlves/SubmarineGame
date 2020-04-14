#include "EnemySub.h"



EnemySub::EnemySub()
{
	maxSpeed = 0.01f;
	rotSpeed = 0.00636f;
	timeCapToNextTorpedo = 8000;
}

bool EnemySub::isColliding(Actor * otherActor, glm::vec3 * colPoint, glm::vec3 * colPoint2)
{
	if (Collectable* collectable = dynamic_cast<Collectable*>(otherActor)) {
		 return false;
	}
	else if (otherActor == this) {
		return false;
	}
	else return octreeCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2);
}

Torpedo* EnemySub::update(float dt, Submarine * target)
{
	Torpedo* torpedo = nullptr;
	timeSinceLastTorpedoFired += dt;
	//canMoveForwards = canMoveBackwards = canTurnLeft = canTurnRight = canMoveUp = canMoveDown = true;
	glm::vec3 dirToTarget = target->getPos() - pos;
	float distanceToTarget = glm::length(dirToTarget);
	//cout << "distance to target = " << distanceToTarget << endl;
	if (distanceToTarget < 100) {
		glm::vec3 front = glm::vec3(objectRotation*glm::vec4(0, 0, -1,1));
		dirToTarget = glm::vec3(glm::inverse(ModelMatrix)*glm::vec4(dirToTarget, 0.0));
		float dotProduct = front.x*dirToTarget.x + front.y*dirToTarget.y + front.z*dirToTarget.z;
		float magA = 1;
		float magB = distanceToTarget;
		float angle = acos(dotProduct / (magA*magB)) / M_PI * 180;
		//cout << "angle = " << angle << endl;

		if (angle < 40) {
			if (readyToFire()) {
				torpedo = fireTorpedo(target->getPos());
			}
		}
		else {
			turnRight(dt);
			moveForward(dt);
		}
	}
	else {
		turnRight(dt);
		moveForward(dt);
	}

	Submarine::update(dt);

	return torpedo;
	//isMoving = false;
}

Torpedo * EnemySub::fireTorpedo(glm::vec3 targetPos)
{
	timeSinceLastTorpedoFired = 0;
	return new Torpedo(pos - glm::vec3(0, 2.0, 0), rotation, true, targetPos);
}


EnemySub::~EnemySub()
{
}
