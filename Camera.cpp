#include "Camera.h"
#include <iostream>



Camera::Camera()
{
	activeCam = 1;
}

void Camera::setActiveCam(int camNumber, float subRot)
{
	activeCam = camNumber;
	camRot = subRot;
}

glm::vec3 Camera::getCamPos(glm::vec3 subPos)
{
	glm::vec3 camPos = glm::vec3(0, 0, 50);
	if (activeCam == 1) {
		camPos = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam1pos, 1.0));
		camPos = subPos + camPos;
	}
	else if (activeCam == 2) {
		camPos = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam2pos, 1.0));
		camPos = subPos + camPos;
	}
	else if (activeCam == 3) {
		camPos = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam3pos, 1.0));
		camPos = subPos + camPos;
	}
	else if (activeCam == 4) {
		camPos = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam4pos, 1.0));
		camPos = subPos + camPos;
	}
	return camPos;
}

glm::vec3 Camera::getCamLook(glm::vec3 subPos)
{
	glm::vec3 camLook = glm::vec3(0, 0, 0);
	if (activeCam == 1) {
		camLook = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam1look, 1.0));
		camLook = subPos + camLook;
	}
	else if (activeCam == 2) {
		camLook = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam2look, 1.0));
		camLook = subPos + camLook;
	}
	else if (activeCam == 3) {
		camLook = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam3look, 1.0));
		camLook = subPos + camLook;
	}
	else if (activeCam == 4) {
		camLook = glm::vec3(glm::rotate(glm::mat4(1.0), camRot, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(cam4look, 1.0));
		camLook = subPos + camLook;
	}
	return camLook;
}

int Camera::getActiveCam()
{
	return activeCam;
}

void Camera::straightenCam(float subRot, float rotSpeed, float dt)
{
	//if (activeCam == 1) {
		float rotDiff = subRot - camRot;
		if (rotDiff < 0) rotDiff += 360;
		/*std::cout << "subRotarion: " << subRot << std::endl;
		std::cout << "camRot: " << camRot << std::endl;
		std::cout << "rotDiff: " << rotDiff << std::endl;
		std::cout << "*****" << subRot << std::endl;*/
		if (rotDiff <= 0.3*rotSpeed*dt && rotDiff >= -0.3*rotSpeed*dt) camRot = subRot;
		else if (rotDiff != 0 && rotDiff>180) camRot -= 0.3*rotSpeed*dt;
		else if (rotDiff != 0 && rotDiff<180) camRot += 0.3*rotSpeed*dt;
		if (camRot < 0) camRot += 360;
		if (camRot > 360) camRot -= 360;
	//}
}

void Camera::rotateCamLeft(float subRot, float rotSpeed, float dt)
{
	if (activeCam == 1) {
		float rotDiff = subRot - camRot;
		if (rotDiff < 0) rotDiff += 360;
		if (rotDiff<180 && rotDiff>20) camRot += rotSpeed*dt;
		if (camRot>360) camRot -= 360;
	}
	else {
		camRot = subRot;
	}
	
}

void Camera::rotateCamRight(float subRot, float rotSpeed, float dt)
{
	if (activeCam == 1) {
		float rotDiff = camRot - subRot;
		if (rotDiff < 0) rotDiff += 360;
		if (rotDiff<180 && rotDiff>20) camRot -= rotSpeed * dt;
		if (camRot<0) camRot += 360;
	}
	else {
		camRot = subRot;
	}
}

void Camera::setCamRot(float rotation)
{
	camRot = rotation;
}


Camera::~Camera()
{
}
