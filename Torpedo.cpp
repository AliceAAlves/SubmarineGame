#include "Torpedo.h"
#include "Submarine.h"
#include "EnemySub.h"
#include "ScreenElements.h"

ThreeDModel Torpedo::staticModel;
ThreeDModel Torpedo::propeller_staticModel;

Torpedo::Torpedo(glm::vec3 position, float rotation_y, bool enemyTorpedo, glm::vec3 target_pos)
{
	pos = position;
	rotation = rotation_y;
	isEnemyTorpedo = enemyTorpedo;
	direction = target_pos - position;
	dir_distance = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
	objectRotation = glm::transpose(glm::lookAt(glm::vec3(0, 0, 0), direction, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Torpedo::display(glm::mat4 viewMatrix, Shader * shader)
{
	glUseProgram(shader->handle());  // use the shader

	ModelMatrix = glm::translate(glm::mat4(), pos);
	if(!isEnemyTorpedo) ModelMatrix = glm::rotate(ModelMatrix, rotation, glm::vec3(0, 1, 0));
	else ModelMatrix = ModelMatrix * objectRotation;
	ModelViewMatrix = viewMatrix * ModelMatrix;
	//ModelViewMatrix = glm::translate(viewMatrix, pos);
	//ModelViewMatrix = ModelViewMatrix * objectRotation;

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	staticModel.drawElementsUsingVBO(shader);
	//model.drawOctreeLeaves(shader);

	glm::mat4 propellerMVMatrix = glm::rotate(ModelViewMatrix, propeller_spin, glm::vec3(0, 0, 1));

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &propellerMVMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(propellerMVMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform1i(glGetUniformLocation(shader->handle(), "hurtFlag"), 0);

	propeller_staticModel.drawElementsUsingVBO(shader);
}

void Torpedo::update(float const dt)
{
	if (!isEnemyTorpedo) {
		pos.z -= cos(rotation*M_PI / 180)*speed*dt;
		pos.x -= sin(rotation*M_PI / 180)*speed*dt;
	}
	else {
		float a = speed * dt / dir_distance;
		pos.x += a * direction.x;
		pos.y += a * direction.y;
		pos.z += a * direction.z;
	}

	propeller_spin -= propeller_spin_speed * dt;
	if (propeller_spin < 0) propeller_spin += 360;
}

bool Torpedo::isColliding(Actor * otherActor, glm::vec3 * colPoint, glm::vec3 * colPoint2)
{
	if (Collectable* collectable = dynamic_cast<Collectable*>(otherActor)) {
		return false;
	}
	if (isEnemyTorpedo) {
		if (EnemySub* enemy = dynamic_cast<EnemySub*>(otherActor)) {
			return false;
		}
		else if (Submarine* sub = dynamic_cast<Submarine*>(otherActor)) {
			bool collision = octreeCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2);
			if (collision) {
				// set damage to sub
				sub->takeDamage(0.1);
			}
			return collision;
		}
	}
	else { // is main submarine's torpedo
		if (EnemySub* enemy = dynamic_cast<EnemySub*>(otherActor)) {
			bool collision = octreeCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2);
			if (collision) {
				// set damage/defeat to sub
				enemy->takeDamage(0.5);
				ScreenElements::score += 200;
				if(enemy->isDefeated()) ScreenElements::score += 500;
			}
			return collision;
		}
	}
	return octreeCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2);
}

Explosion* Torpedo::handleCollision(glm::vec3 * colPoint)
{
	exploded = true;

	glm::vec3 explosion_pos = glm::vec3(ModelMatrix*glm::vec4(colPoint->x, colPoint->y, colPoint->z,1.0));

	return new Explosion(explosion_pos);
}


Explosion* Torpedo::checkAllCollisions(std::vector<Actor*> allActors, glm::vec3 * colPoint, glm::vec3 * colPoint2)
{
	for (int i = 0; i < allActors.size(); i++) {
		if (isColliding(allActors[i], colPoint, colPoint2)) {
			cout << "BOOOM!" << endl;
			return handleCollision(colPoint);
		}
	}
	return nullptr;
}

void Torpedo::loadStaticModel(char * fileName, Shader * shader, int maxLevel)
{
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);


	cout << " loading model " << endl;
	if (objLoader.loadModel(fileName, staticModel))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		//model.calcCentrePoint();
		//model.centreOnZero();


		staticModel.calcVertNormalsUsingOctree(maxLevel);  //the method will construct the octree if it hasn't already been created.


													//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		staticModel.initDrawElements();
		staticModel.initVBO(shader);
		//model.deleteVertexFaceData();

	}
	else
	{
		cout << " model failed to load " << endl;
	}
}

void Torpedo::loadPropellerModel(char * fileName, Shader * shader, int maxLevel)
{
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);


	cout << " loading model " << endl;
	if (objLoader.loadModel(fileName, propeller_staticModel))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		//model.calcCentrePoint();
		//model.centreOnZero();


		propeller_staticModel.calcVertNormalsUsingOctree(maxLevel);  //the method will construct the octree if it hasn't already been created.


															   //turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		propeller_staticModel.initDrawElements();
		propeller_staticModel.initVBO(shader);
		//model.deleteVertexFaceData();

	}
	else
	{
		cout << " model failed to load " << endl;
	}
}

Octree * Torpedo::getOctree()
{
	return staticModel.octree;
}

Vector3d * Torpedo::getVerts()
{
	return staticModel.theVerts;
}

Vector3d * Torpedo::getFaceNormals()
{
	return staticModel.theFaceNormals;
}

aFace * Torpedo::getFaces()
{
	return staticModel.theFaces;
}

Torpedo::~Torpedo()
{
}

bool Torpedo::hasExploded()
{
	return exploded;
}
