#include "Goal.h"
#include <iostream>
#include <glm/gtx/common.hpp>
#include <math.h>
#include "../../../GameEngine.h"

bool Goal::Init(glm::vec3 pos, glm::vec3 min, glm::vec3 max)
{
	GameEngine& game = GameEngine::Instance();

	this->m_pEntity = game.AddEntity(0, glm::vec3(0, 1, 50), "Cube", "res/Model/Toroid.bmp", nullptr, true);
	this->m_pEntity->Scale(glm::vec3(0.01f));

	this->m_pEntity->Position(pos);

	this->m_position = pos;

	this->m_collision.min = min;

	this->m_collision.max = max;

	return true;
}

bool Goal::CollisionCheck(glm::vec3 pos,float radius)
{
	glm::vec3 CPcolMin = m_position + m_collision.min;
	glm::vec3 CPcolMax = m_position + m_collision.max;

	//CalcClosestPoint(pos, radius);

	if (CPcolMax.x < pos.x - radius || CPcolMin.x > pos.x + radius) return false;
	if (CPcolMax.y < pos.y - radius || CPcolMin.y > pos.y + radius) return false;
	if (CPcolMax.z < pos.z - radius || CPcolMin.z > pos.z + radius) return false;

	return true;
}

bool Goal::ToggleVisible(bool b)
{
	m_pEntity->ToggleVisibility(b);

	return b;
}
//
//float angleBetween(
//	glm::vec3 a,
//	glm::vec3 b,
//	glm::vec3 origin
//) {
//	glm::vec3 da = glm::normalize(a - origin);
//	glm::vec3 db = glm::normalize(b - origin);
//	return glm::acos(glm::dot(da, db));
//}
//
//glm::vec3 Goal::CalcClosestPoint(glm::vec3 pos, float r)
//{
//	glm::vec3 difference = pos - m_position;
//
//	float angle = angleBetween(glm::vec3(10, 0, 0), m_position, pos);
//
//	float rotateX = 0;
//	float rotateY = 0;
//
//	float x = sin(rotateX) * r;
//	float z = cos(rotateX) * r;
//
//	float lengthRatio = cos(rotateY) * x;
//	float lengthZ = cos(rotateY) * z;
//	float y = sin(rotateY) * r;
//
//	glm::vec3 local = glm::vec3(x, y, z);
//
//	return local + pos;
//}