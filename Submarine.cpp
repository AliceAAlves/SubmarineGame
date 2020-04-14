#include "Submarine.h"



Submarine::Submarine()
{
	timeCapToNextTorpedo = 2000;
}

void Submarine::display(glm::mat4 viewMatrix, Shader * shader)
{
	glUseProgram(shader->handle());  // use the shader

	ModelMatrix = glm::translate(glm::mat4(), pos);
	ModelMatrix = glm::rotate(ModelMatrix, rotation, glm::vec3(0, 1, 0));
	ModelMatrix = ModelMatrix * objectRotation;
	ModelViewMatrix = viewMatrix * ModelMatrix;
	//ModelViewMatrix = glm::translate(viewMatrix, pos);
	//ModelViewMatrix = ModelViewMatrix * objectRotation;

	int hurtFlag = 0;
	if (hurt) hurtFlag = 1;
	//cout << hurtFlag << endl;
	glUniform1i(glGetUniformLocation(shader->handle(), "hurtFlag"), hurtFlag);

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	model.drawElementsUsingVBO(shader);
	//model.drawOctreeLeaves(shader);

	glm::mat4 propellerMVMatrix = glm::rotate(ModelViewMatrix, propeller_spin, glm::vec3(0, 0, 1));


	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &propellerMVMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(propellerMVMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	propeller_model.drawElementsUsingVBO(shader);
}

void Submarine::update(float const dt)
{
	if (dt > 2000) return;
	
	timeSinceLastTorpedoFired += dt;

	if(hurt) timeSinceGotHurt += dt;
	if (hurt && timeSinceGotHurt > timeCapForHurt) hurt = false;
	//cout << "speed: " << speed << " // pos: " << pos.x << "," << pos.z << endl;
	//if (hurt) cout << "HURT" << endl;

	if (defeated) {
		if (vertSpeed > -maxVertSpeed) {
			vertSpeed -= 0.5*accelVert * dt;
		}
		if (vertSpeed < -maxVertSpeed) {
			vertSpeed = -maxVertSpeed;
		}
		//cout << vertSpeed << endl;
		if (canMoveDown) {
			pos.y += vertSpeed * dt;
			isMovingVertically = true;
		}
	}

	if (!canMoveForwards && speed > 0) {
		speed = 0;
	}
	if (!canMoveBackwards && speed < 0) {
		speed = 0;
	}
	if (!canMoveUp && vertSpeed > 0) {
		vertSpeed = 0;
	}
	if (!canMoveDown && vertSpeed < 0) {
		vertSpeed = 0;
	}
	if (!isMoving) {
		if (speed > accel*dt) {
			speed -= accel * dt;
		}
		else if (speed < -accel * dt) {
			speed += accel * dt;
		}
		else speed = 0;
	}

	if (!isMovingVertically) {
		vertSpeed = 0;
		if (rotation_x > 0.1*rotSpeed*dt) 
			updateTransform(-0.1*rotSpeed*dt, 0, 0);
		else if (rotation_x < -0.1*rotSpeed*dt) 
			updateTransform(0.1*rotSpeed*dt, 0, 0);
	}
	pos.z -= cos(rotation*M_PI / 180)*speed*dt;
	pos.x -= sin(rotation*M_PI / 180)*speed*dt;
	pos.y += vertSpeed * dt;

	if (isMoving) {
		if (propeller_spin_speed < propeller_spin_maxspeed) {
			propeller_spin_speed += propeller_spin_accel * dt;
		}
		if (propeller_spin_speed > propeller_spin_maxspeed) {
			propeller_spin_speed = propeller_spin_maxspeed;
		}
	}
	else {
		if (propeller_spin_speed > propeller_spin_accel*dt) {
			propeller_spin_speed -= 0.2*propeller_spin_accel * dt;
		}
		else propeller_spin_speed = 0;
	}
	propeller_spin -= propeller_spin_speed * dt;
	if (propeller_spin < 0) propeller_spin += 360;

	isMoving = false;
}

void Submarine::moveForward(float const dt)
{
	if (dt > 2000) return;
	if (defeated) return;
	if (canMoveForwards) {
		if (speed < maxSpeed) {
			speed += accel * dt;
		}
		if (speed > maxSpeed) {
			speed = maxSpeed;
		}
	}
	isMoving = true;
	isMovingForwards = true;
}

void Submarine::moveBackwards(float const dt)
{
	if (dt > 2000) return;
	if (defeated) return;
	if (canMoveBackwards) {
		if (speed > -maxbwSpeed) {
			speed -= accel * dt;
		}
		if (speed < -maxbwSpeed) {
			speed = -maxbwSpeed;
		}
	}
	isMoving = true;
}

void Submarine::turnLeft(float const dt)
{
	if (dt > 2000) return;
	if (defeated) return;
	if (canTurnLeft) {
		rotation += rotSpeed * dt;
		if (rotation > 360) rotation -= 360;
		if (rotation < 0) rotation += 360;
	}
		
		//updateTransform(0, rotSpeed*dt, 0);
}

void Submarine::turnRight(float const dt)
{
	if (dt > 2000) return;
	if (defeated) return;
	if (canTurnRight) {
		rotation -= rotSpeed * dt;
		if (rotation > 360) rotation -= 360;
		if (rotation < 0) rotation += 360;
		//updateTransform(0,-rotSpeed*dt, 0);
	}
		
}

void Submarine::moveUp(float const dt)
{
	if (dt > 2000) return; 
	if (defeated) return;
	if (canMoveUp) {
		if (vertSpeed < maxVertSpeed) {
			vertSpeed += accelVert * dt;
		}
		if (vertSpeed > maxVertSpeed) {
			vertSpeed = maxVertSpeed;
		}
		if (rotation_x < rotation_x_cap) {
			updateTransform(0.2*rotSpeed*dt, 0, 0);
		}
	}
	isMovingVertically = true;
		//pos.y += vertSpeed*dt;
}

void Submarine::moveDown(float const dt)
{
	if (dt > 2000) return;
	if (defeated) return; 
	if (canMoveDown) {
		if (vertSpeed > -maxVertSpeed) {
			vertSpeed -= accelVert * dt;
		}
		if (vertSpeed < -maxVertSpeed) {
			vertSpeed = -maxVertSpeed;
		}
		if (rotation_x > -rotation_x_cap) {
			updateTransform(-0.2*rotSpeed*dt, 0, 0);
		}
	}
	isMovingVertically = true;
	//	pos.y -= vertSpeed * dt;
}


void Submarine::setMoving(bool moving)
{
	isMoving = moving;
}

void Submarine::setMovingForward(bool moving)
{
	isMovingForwards = moving;
}

void Submarine::setMovingVertically(bool moving)
{
	isMovingVertically = moving;
}

float Submarine::getRotSpeed()
{
	return rotSpeed;
}

void Submarine::setRotation(float newRotation)
{
	rotation = newRotation;
}

void Submarine::reset()
{
	objectRotation = glm::mat4();
	rotation = 0;
	rotation_x = 0;
	pos = glm::vec3(0, 0, 0);
	health = 1.0;
	speed = colRotSpeed = propeller_spin = propeller_spin_speed = vertSpeed = 0.0;
	defeated = hurt = false;
}

glm::vec3 Submarine::getSpotLightPos()
{
	return glm::vec3(ModelMatrix*glm::vec4(0,0,-7.7,1.0)); //(-0.92,-0.66,-7.7,1.0)
}

glm::vec3 Submarine::getSpotLightDirection()
{
	glm::vec4 dir = ModelMatrix * glm::vec4(0.0, 0.0, -1.0, 1.0) - ModelMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
	return glm::vec3(dir);
}

bool Submarine::isColliding(Actor * otherActor, glm::vec3 * colPoint, glm::vec3 * colPoint2, glm::vec3* colFaceNormal, int* numTris)
{
	if (otherActor == this) return false;
	if (Collectable* collectable = dynamic_cast<Collectable*>(otherActor)) {
		if (octreeSphereCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), 1.0)) {
			collectable->collect();
			//return true;
		}
		//else
		return false;
	}
	else return octreeCollisionWithGeometry(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2, colFaceNormal, numTris,
		getVerts(), otherActor->getVerts(),getFaceNormals(), otherActor->getFaceNormals(),getFaces(), otherActor->getFaces());
}

void Submarine::handleCollision(glm::vec3 * colPoint, glm::vec3 * colFaceNormal, float dt, int numberTris)
{
	glm::vec3 front = glm::vec3(0, 0, -1);

	if (colPoint == nullptr) return;
	
	if (colPoint->y > 0.2) canMoveUp = false;
	else if (colPoint->y < -0.2) canMoveDown = false;

	float dotProduct = front.x*colPoint->x + front.z*colPoint->z;
	float magA = 1; //sqrt(front.x*front.x + front.z*froInt.z);
	float magB = sqrt(colPoint->x*colPoint->x + colPoint->z*colPoint->z);
	float angle = acos(dotProduct / (magA*magB))/M_PI * 180;
	if (colPoint->x > 0) angle = 360-angle;

	//cout << "col angle " << angle << endl;

	if (angle < 20 || angle > 340) canMoveForwards = false;
	else if (angle > 160 && angle < 200) canMoveBackwards = false;

	if ((angle > 5 && angle < 85) || (angle > 185 && angle < 265)) canTurnLeft = false;
	else if ((angle > 95 && angle < 175) || (angle > 275 && angle < 355)) canTurnRight = false;

	if (isMovingForwards && numberTris <3 && !defeated) {
		glm::vec3 colFaceTangent = glm::normalize(glm::cross(*colFaceNormal, glm::vec3(0, 1, 0)));
		float dotP = glm::dot(colFaceTangent, front);
		//cout << "colFaceNormal: " << colFaceNormal->x << ", " << colFaceNormal->y << ", " << colFaceNormal->z << endl;
		//cout << "colFaceTangent: " << colFaceTangent.x << ", " << colFaceTangent.y << ", " << colFaceTangent.z << endl;
		//cout << "dot: " << dotP << endl;
		pos += dotP * 0.5f*maxSpeed*dt*glm::vec3(ModelMatrix*glm::vec4(colFaceTangent,0.0)); //- 0.1f*maxSpeed*dt*(glm::vec3(colFaceNormal->x,0.0, colFaceNormal->z));
	}
}

void Submarine::checkAllCollisions(std::vector<Actor*> allActors, glm::vec3* colPoint, glm::vec3* colPoint2, glm::vec3* colFaceNormal, float dt, int* numTris)
{
	canMoveForwards = canMoveBackwards = canTurnLeft = canTurnRight = canMoveUp = canMoveDown = true;
	for (int i = 0; i < allActors.size(); i++) {
		if (isColliding(allActors[i], colPoint, colPoint2, colFaceNormal, numTris)) {
			//cout << "COLLINDING!" << endl;
			handleCollision(colPoint, colFaceNormal, dt, *numTris);
		}
		else{
			//cout << "not colliding" << endl;
		}
	}
	if (pos.y >= -5.5) canMoveUp = false; // don't move above the water
}

void Submarine::loadPropellerModel(char * fileName, Shader * shader, int maxLevel)
{
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);


	cout << " loading model " << endl;
	if (objLoader.loadModel(fileName, propeller_model))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		//model.calcCentrePoint();
		//model.centreOnZero();


		propeller_model.calcVertNormalsUsingOctree(maxLevel);  //the method will construct the octree if it hasn't already been created.


													 //turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		propeller_model.initDrawElements();
		propeller_model.initVBO(shader);
		//model.deleteVertexFaceData();

	}
	else
	{
		cout << " model failed to load " << endl;
	}
}

Torpedo * Submarine::fireTorpedo()
{
	timeSinceLastTorpedoFired = 0;
	return new Torpedo(pos-glm::vec3(0,2.0,0),rotation,false, glm::vec3(0, 0, 0));
}

bool Submarine::readyToFire()
{
	if (timeSinceLastTorpedoFired > timeCapToNextTorpedo) return true;
	else return false;
}

bool Submarine::isHurt()
{
	return false;
}

float Submarine::getHealth()
{
	return health;
}

void Submarine::takeDamage(float damage)
{
	if (!hurt) {
		hurt = true; 
		timeSinceGotHurt = 0;
		health -= damage;
	}
	if (health <= 0) {
		health = 0;
		defeated = true;
	}
}

bool Submarine::isDefeated()
{
	return defeated;
}

Submarine::~Submarine()
{
}
