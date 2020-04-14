#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

class Camera
{
public:
	Camera();
	void setActiveCam(int camNumber, float subRot);
	glm::vec3 getCamPos(glm::vec3 subPos);
	glm::vec3 getCamLook(glm::vec3 subPos);
	int getActiveCam();
	void straightenCam(float subRot, float rotSpeed, float dt);
	void rotateCamLeft(float subRot, float rotSpeed, float dt);
	void rotateCamRight(float subRot, float rotSpeed, float dt);
	void setCamRot(float rotation);
	~Camera();
private:
	int activeCam;
	glm::vec3 const cam1pos = glm::vec3(0, 10, 30);//15,30
	glm::vec3 const cam1look = glm::vec3(0, 0, -500);
	glm::vec3 const cam2pos = glm::vec3(0, 10, -30);
	glm::vec3 const cam2look = glm::vec3(0, 0, 500);
	glm::vec3 const cam3pos = glm::vec3(0, 4.7, -1.5);
	glm::vec3 const cam3look = glm::vec3(0, 4.7, -500);
	glm::vec3 const cam4pos = glm::vec3(0, 40, 0);
	glm::vec3 const cam4look = glm::vec3(0, 0, -1);
	float camRot;
};

