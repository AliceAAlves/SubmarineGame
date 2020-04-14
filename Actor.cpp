#include "Actor.h"
#include <iostream>

OBJLoader Actor::objLoader;

Actor::Actor()
{
	pos = glm::vec3(0, 0, 0);
}

void Actor::display(glm::mat4 viewMatrix, Shader* shader)
{
	
	glUseProgram(shader->handle());  // use the shader

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ModelMatrix = glm::translate(glm::mat4(), pos);
	ModelMatrix = ModelMatrix * objectRotation;
	ModelViewMatrix = viewMatrix * ModelMatrix;
	//ModelViewMatrix = glm::translate(viewMatrix, pos);
	//ModelViewMatrix = ModelViewMatrix * objectRotation;

	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform1i(glGetUniformLocation(shader->handle(), "hurtFlag"), 0);

	model.drawElementsUsingVBO(shader);
	//model.drawOctreeLeaves(shader);
}

glm::vec3 Actor::getPos()
{
	return pos;
}

float Actor::getRot()
{
	return rotation;
}

void Actor::setPos(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void Actor::updateTransform(float xinc, float yinc, float zinc)
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

	rotation += yinc;
	if (rotation > 360) rotation -= 360;
	if (rotation < 0) rotation += 360;

	rotation_x += xinc;
	if (rotation_x > 180) rotation_x -= 360;
	if (rotation_x < -180) rotation_x += 360;
}

void Actor::loadModel(char * fileName, Shader* shader, int maxLevel)
{
	glUseProgram(shader->handle());  // use the shader
	glEnable(GL_TEXTURE_2D);


	cout << " loading model " << endl;
	if (objLoader.loadModel(fileName, model))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		//model.calcCentrePoint();
		//model.centreOnZero();


		model.calcVertNormalsUsingOctree(maxLevel);  //the method will construct the octree if it hasn't already been created.


		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		model.initDrawElements();
		model.initVBO(shader);
		//model.deleteVertexFaceData();

	}
	else
	{
		cout << " model failed to load " << endl;
	}
}

Octree * Actor::getOctree()
{
	return model.octree;
}

bool Actor::isColliding(Actor * otherActor, glm::vec3* colPoint, glm::vec3* colPoint2)
{
	return octreeCollision(getOctree(), otherActor->getOctree(), ModelViewMatrix, otherActor->getMVMatrix(), colPoint, colPoint2);
}

void Actor::update()
{
}

Vector3d * Actor::getVerts()
{
	return model.theVerts;
}

Vector3d * Actor::getFaceNormals()
{
	return model.theFaceNormals;
}

aFace * Actor::getFaces()
{
	return model.theFaces;
}

glm::mat4 Actor::getMVMatrix()
{
	return ModelViewMatrix;
}


Actor::~Actor()
{
}

bool sphereColision(glm::vec3 c1, float r1, glm::vec3 c2, float r2) {
	if ((c1.x - c2.x)*(c1.x - c2.x) + (c1.y - c2.y)*(c1.y - c2.y)
		+ (c1.z - c2.z)*(c1.z - c2.z) < (r1 + r2)*(r1 + r2))
		return true;
	else return false;
}

bool octreeCollision(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2)
{
	int maxLevel1 = ot1->getMaxLevel();
	int maxLevel2 = ot2->getMaxLevel();
	bool collision;
	// if both octrees are in the last 2 levels, do SAT collision
	if (ot1->getLevel() > maxLevel1 - 2 && ot2->getLevel() > maxLevel2 - 2)
		collision = satCollision(ot1, ot2, transform1, transform2);
	// else do sphere collision
	else {
		glm::vec3 c1 = ot1->getBoxCenter();
		c1 = glm::vec3(transform1 * glm::vec4(c1, 1.0));
		glm::vec3 c2 = ot2->getBoxCenter();
		c2 = glm::vec3(transform2 * glm::vec4(c2, 1.0));
		float r1, r2;
		r1 = ot1->getBoxRadius();
		r2 = ot2->getBoxRadius();

		collision = sphereColision(c1, r1, c2, r2);
	}
	if (collision) {
		if (ot1->getLevel() >= maxLevel1) {
			if (ot2->getLevel() >= maxLevel2) {
				*colPoint1 = ot1->getBoxCenter();
				*colPoint2 = ot2->getBoxCenter();
				return true;
			}
			else {
				for (int i = 0; i < 8; i++) {
					Octree* child2 = ot2->getChild(i);
					if (child2 != nullptr) {
						if (octreeCollision(ot1, child2, transform1, transform2, colPoint1, colPoint2)) return true;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 8; i++) {
				Octree* child1 = ot1->getChild(i);
				if (child1 != nullptr) {
					if( octreeCollision(child1, ot2, transform1, transform2, colPoint1, colPoint2)) return true;
				}
			}
		}
	}
	else return false;
}

bool octreeCollisionWithGeometry(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2, glm::vec3* colFaceNormal, int* numTris,
	Vector3d* theVerts1, Vector3d* theVerts2, Vector3d* theFaceNormals1, Vector3d* theFaceNormals2, aFace* theFaces1, aFace* theFaces2)
{
	int maxLevel1 = ot1->getMaxLevel();
	int maxLevel2 = ot2->getMaxLevel();

	bool collision;
	// if both octrees are in the last 2 levels, do SAT collision
	if (ot1->getLevel() > maxLevel1 - 2 && ot2->getLevel() > maxLevel2 - 2)
		collision = satCollision(ot1, ot2, transform1, transform2);
	// else do sphere collision
	else {
		glm::vec3 c1 = ot1->getBoxCenter();
		c1 = glm::vec3(transform1 * glm::vec4(c1, 1.0));
		glm::vec3 c2 = ot2->getBoxCenter();
		c2 = glm::vec3(transform2 * glm::vec4(c2, 1.0));
		float r1, r2;
		r1 = ot1->getBoxRadius();
		r2 = ot2->getBoxRadius();

		collision = sphereColision(c1, r1, c2, r2);
	} 
	if (collision) {
		if (ot1->getLevel() >= maxLevel1) {
			if (ot2->getLevel() >= maxLevel2) {
				if(geometryCollision(ot1, ot2, transform1, transform2, colPoint1, colPoint2, colFaceNormal, numTris, theVerts1, theVerts2,
					theFaceNormals1, theFaceNormals2, theFaces1, theFaces2))
					return true;
			}
			else {
				for (int i = 0; i < 8; i++) {
					Octree* child2 = ot2->getChild(i);
					if (child2 != nullptr) {
						if (octreeCollisionWithGeometry(ot1, child2, transform1, transform2, colPoint1, colPoint2, colFaceNormal, numTris, theVerts1, theVerts2,
							theFaceNormals1, theFaceNormals2, theFaces1, theFaces2)) return true;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 8; i++) {
				Octree* child1 = ot1->getChild(i);
				if (child1 != nullptr) {
					if (octreeCollisionWithGeometry(child1, ot2, transform1, transform2, colPoint1, colPoint2, colFaceNormal, numTris, theVerts1, theVerts2,
						theFaceNormals1, theFaceNormals2, theFaces1, theFaces2)) return true;
				}
			}
		}
	}
	return false;
}

bool octreeSphereCollision(Octree * ot1, Octree * ot2, glm::mat4 transform1, glm::mat4 transform2, float r2)
{
	int maxLevel1 = ot1->getMaxLevel();
	glm::vec3 c1 = ot1->getBoxCenter();
	c1 = glm::vec3(transform1 * glm::vec4(c1, 1.0));
	glm::vec3 c2 = ot2->getBoxCenter();
	c2 = glm::vec3(transform2 * glm::vec4(c2, 1.0));
	float r1;

	if (ot1->getLevel() < maxLevel1) r1 = ot1->getBoxRadius();
	else r1 = 0;
	if (sphereColision(c1, r1, c2, r2)) {
		if (ot1->getLevel() >= maxLevel1) {
			return true;
		}
		else {
			for (int i = 0; i < 8; i++) {
				Octree* child1 = ot1->getChild(i);
				if (child1 != nullptr) {
					if (octreeSphereCollision(child1, ot2, transform1, transform2, r2)) return true;
				}
			}
		}
	}
	else return false;
}

bool satCollision(Octree * ot1, Octree * ot2, glm::mat4 transform1, glm::mat4 transform2) {
	vector<glm::vec3> ot1_corners;
	vector<glm::vec3> ot2_corners;
	vector<glm::vec3> axis;

	glm::vec3 mins1 = ot1->getBoxMins();
	glm::vec3 maxs1 = ot1->getBoxMaxs();
	glm::vec3 mins2 = ot2->getBoxMins();
	glm::vec3 maxs2 = ot2->getBoxMaxs();

	// Corners for ot1
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(mins1.x, mins1.y, mins1.z, 1.0)));	// 0: m, m, m
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(maxs1.x, mins1.y, mins1.z, 1.0))); // 1: M, m, m
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(mins1.x, maxs1.y, mins1.z, 1.0)));	// 2: m, M, m
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(maxs1.x, maxs1.y, mins1.z, 1.0))); // 3: M, M, m
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(mins1.x, mins1.y, maxs1.z, 1.0)));	// 4: m, m, M
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(maxs1.x, mins1.y, maxs1.z, 1.0))); // 5: M, m, M
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(mins1.x, maxs1.y, maxs1.z, 1.0)));	// 6: m, M, M
	ot1_corners.push_back(glm::vec3(transform1*glm::vec4(maxs1.x, maxs1.y, maxs1.z, 1.0))); // 7: M, M, M

	// Corners for ot2
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(mins2.x, mins2.y, mins2.z, 1.0)));	// 0: m, m, m
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(maxs2.x, mins2.y, mins2.z, 1.0))); // 1: M, m, m
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(mins2.x, maxs2.y, mins2.z, 1.0)));	// 2: m, M, m
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(maxs2.x, maxs2.y, mins2.z, 1.0))); // 3: M, M, m
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(mins2.x, mins2.y, maxs2.z, 1.0)));	// 4: m, m, M
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(maxs2.x, mins2.y, maxs2.z, 1.0))); // 5: M, m, M
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(mins2.x, maxs2.y, maxs2.z, 1.0)));	// 6: m, M, M
	ot2_corners.push_back(glm::vec3(transform2*glm::vec4(maxs2.x, maxs2.y, maxs2.z, 1.0))); // 7: M, M, M

	// Axis
	glm::vec3 a_x = ot1_corners[1] - ot1_corners[0]; // ot1 x axis
	glm::vec3 a_y = ot1_corners[2] - ot1_corners[0]; // ot1 y axis
	glm::vec3 a_z = ot1_corners[4] - ot1_corners[0]; // ot1 z axis
	glm::vec3 b_x = ot2_corners[1] - ot2_corners[0]; // ot2 x axis
	glm::vec3 b_y = ot2_corners[2] - ot2_corners[0]; // ot2 y axis
	glm::vec3 b_z = ot2_corners[4] - ot2_corners[0]; // ot2 z axis
	axis.push_back(a_x);
	axis.push_back(a_y);
	axis.push_back(a_z);
	axis.push_back(b_x);
	axis.push_back(b_y);
	axis.push_back(b_z);
	axis.push_back(glm::cross(a_x, b_x)); // cross(a_x, b_x)
	axis.push_back(glm::cross(a_x, b_y)); // cross(a_x, b_y)
	axis.push_back(glm::cross(a_x, b_z)); // cross(a_x, b_z)
	axis.push_back(glm::cross(a_y, b_x)); // cross(a_y, b_x)
	axis.push_back(glm::cross(a_y, b_y)); // cross(a_y, b_y)
	axis.push_back(glm::cross(a_y, b_z)); // cross(a_y, b_z)
	axis.push_back(glm::cross(a_z, b_x)); // cross(a_z, b_x)
	axis.push_back(glm::cross(a_z, b_y)); // cross(a_z, b_y)
	axis.push_back(glm::cross(a_z, b_z)); // cross(a_z, b_z)

	for (int i = 0; i < axis.size(); i++) {
		if (!intersectsWhenProjected(ot1_corners, ot2_corners, axis[i])) //if doesn't intersect in one of the axis then is not colliding
			return false;
	}
	return true;
}

// https://gamedev.stackexchange.com/questions/44500/how-many-and-which-axes-to-use-for-3d-obb-collision-with-sat
bool intersectsWhenProjected(vector<glm::vec3> aCorners, vector<glm::vec3> bCorners, glm::vec3 axis) {
	// Handles the cross product = {0,0,0} case
	if (axis == glm::vec3(0,0,0))
		return true;
	float aMin = 10000;
	float aMax = -10000;
	float bMin = 10000;
	float bMax = -10000;

	// Define two intervals, a and b.Calculate their min and max values
	for (int i = 0; i < 8; i++) {
		float aDist = glm::dot(aCorners[i], axis);
		aMin = (aDist < aMin) ? aDist : aMin;
		aMax = (aDist > aMax) ? aDist : aMax;
		float bDist = glm::dot(bCorners[i], axis);
		bMin = (bDist < bMin) ? bDist : bMin;
		bMax = (bDist > bMax) ? bDist : bMax;
	}

	// One-dimensional intersection test between a and b
	float longSpan = max(aMax, bMax) - min(aMin, bMin);
	float sumSpan = aMax - aMin + bMax - bMin;
	return longSpan <= sumSpan; // Change this to <= if you want the case were they are touching but not overlapping, to count as an intersection
}


bool geometryCollision(Octree* ot1, Octree* ot2, glm::mat4 transform1, glm::mat4 transform2, glm::vec3* colPoint1, glm::vec3* colPoint2, glm::vec3* colFaceNormal, int* numTris,
	Vector3d* theVerts1, Vector3d* theVerts2, Vector3d* theFaceNormals1, Vector3d* theFaceNormals2, aFace* theFaces1, aFace* theFaces2)
{
	if (ot1->getVertexListSize() == 0) return false;
	*numTris = ot2->getPrimitiveListSize();
	for (int j = 0; j < ot2->getPrimitiveListSize(); j++)		 //GO through all the faces in Octree2
	{
		unsigned int faceVertIndex = theFaces2[ot2->getPrimitiveList()[j]].thePoints[0];
		Vector3d faceVert = theVerts2[faceVertIndex];
		glm::vec3 faceVert2 = glm::vec3(transform2 * glm::vec4(faceVert.x, faceVert.y, faceVert.z,1.0));
		unsigned int faceNormalIndex = theFaces2[ot2->getPrimitiveList()[j]].theFaceNormal;
		Vector3d faceNormal = theFaceNormals2[faceNormalIndex];
		glm::vec3 faceNormal2 = glm::vec3(transform2 * glm::vec4(faceNormal.x, faceNormal.y, faceNormal.z, 0.0));
		
		for (int k = 0; k < ot1->getVertexListSize(); k++)								//GO through all the vertex in Octree1
		{
			Vector3d vertex = theVerts1[ot1->getVertexList()[k]];
			glm::vec3 vertex1 = glm::vec3(transform1 * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0));
			
			float dotP = glm::dot(vertex1-faceVert2, faceNormal2);
			
			if (dotP < 0) {			// vertex of obj1 is behind the face, check if it's behind adjacent convex faces
				int facesAdjacentAndConvexCount = 0;
				bool collision_flag = true;

				//Project colPoint 1 in current face
				//the point (x,y,z) and the plane with normal (a,b,c) with the point (d,e,f).
				//t=(ad-ax+be-by+cf-cz)/(a^2+b^2+c^2)   ---> (x+ta,y+tb,z+tc)
				float x = vertex1.x, y = vertex1.y, z = vertex1.z;
				float a = faceNormal2.x, b = faceNormal2.y, c = faceNormal2.z;
				float d = faceVert2.x, e = faceVert2.y, f_ = faceVert2.z;
				float t = (a*d - a * x + b * e - b * y + c * f_ - c * z) / (a*a + b * b + c * c);
				glm::vec3 point2 = vertex1 + t * faceNormal2;
				glm::vec3 point2_closest = point2;
				glm::vec3 faceNormal2_closest = faceNormal2;

				for (int f = 0; f < ot2->getPrimitiveListSize(); f++) {
					if (!collision_flag) break; // if collision_flag has already been set to false, there's no need to check the rest of the faces
					if (f == j) continue;
					if (facesAdjacentAndConvex(theFaces2, theVerts2, theFaceNormals2, ot2->getPrimitiveList()[j], ot2->getPrimitiveList()[f])) {
						//if faces and adjacent and convex, check if it's also behind that face
						facesAdjacentAndConvexCount++;
						unsigned int faceVertIndex_f = theFaces2[ot2->getPrimitiveList()[f]].thePoints[0];
						Vector3d faceVert_f = theVerts2[faceVertIndex_f];
						glm::vec3 faceVert2_f = glm::vec3(transform2 * glm::vec4(faceVert_f.x, faceVert_f.y, faceVert_f.z, 1.0));
						unsigned int faceNormalIndex_f = theFaces2[ot2->getPrimitiveList()[f]].theFaceNormal;
						Vector3d faceNormal_f = theFaceNormals2[faceNormalIndex_f];
						glm::vec3 faceNormal2_f = glm::vec3(transform2 * glm::vec4(faceNormal_f.x, faceNormal_f.y, faceNormal_f.z, 0.0));

						float dotP2 = glm::dot(vertex1 - faceVert2_f, faceNormal2_f);

						if (dotP2 > 0) { collision_flag = false; } // if it's in front of one of the adjacent convex faces then it's not colliding
						else {
							//Project colPoint 1 in current face
							//the point (x,y,z) and the plane with normal (a,b,c) with the point (d,e,f).
							//t=(ad-ax+be-by+cf-cz)/(a^2+b^2+c^2)   ---> (x+ta,y+tb,z+tc)
							x = vertex1.x, y = vertex1.y, z = vertex1.z;
							a = faceNormal2_f.x, b = faceNormal2_f.y, c = faceNormal2_f.z;
							d = faceVert2_f.x, e = faceVert2_f.y, f_ = faceVert2_f.z;
							t = (a*d - a * x + b * e - b * y + c * f_ - c * z) / (a*a + b * b + c * c);
							glm::vec3 point2_f = vertex1 + t * faceNormal2_f;
							if (glm::length(point2_f - vertex1) < glm::length(point2 - vertex1)) {
								glm::vec3 point2_closest = point2_f;
								glm::vec3 faceNormal2_closest = faceNormal2_f;
							}
						}
					}
				}
				if (collision_flag) {
					*colPoint1 = glm::vec3(vertex.x, vertex.y, vertex.z);
					*colFaceNormal = glm::vec3(glm::inverse(transform1) * glm::vec4(faceNormal2_closest,0.0)); // Face normal of ot2 in ot1 object coordenates
					*colPoint2 = glm::vec3(glm::inverse(transform2)*glm::vec4(point2_closest, 1.0));
					//cout << "colliding" << endl;
					return true;
				}
				if (facesAdjacentAndConvexCount == 0) return true;
			}
		}
	}
	return false;
}

bool facesAdjacentAndConvex(aFace* faces, Vector3d* verts, Vector3d* faceNormals, unsigned int face1, unsigned int face2) {
	int points_in_common = 0;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int face1VertIndex = faces[face1].thePoints[i];
			unsigned int face2VertIndex = faces[face2].thePoints[j];

			if (face1VertIndex == face2VertIndex) points_in_common++;
			if (points_in_common > 1) { // faces are adjacent
				unsigned int face1NormalIndex = faces[face1].theFaceNormal;
				Vector3d face1Normal = faceNormals[face1NormalIndex];
				Vector3d face1middleP = faceMiddlePoint(faces[face1], verts);
				Vector3d face2middleP = faceMiddlePoint(faces[face2], verts);
				Vector3d vector12 = face2middleP - face1middleP;
				
				float dot = Vector3d::dotProduct(face1Normal, vector12);
				if (dot < 0) return true; // faces are convex
				else return false; // faces are concave or in same plane
			}
		}
	}
	return false;
}

Vector3d faceMiddlePoint(aFace face, Vector3d* verts) {
	Vector3d p1 = verts[face.thePoints[0]];
	Vector3d p2 = verts[face.thePoints[1]];
	Vector3d p3 = verts[face.thePoints[2]];

	Vector3d middlePoint = (p1 + p2 + p3) / 3;;
	return  middlePoint;
}