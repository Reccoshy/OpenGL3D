#pragma once

#include <glm/glm.hpp>

class AttackInformer 
{
public:
	
	bool Init(float distance);

	void Update(int index ,glm::vec3 origin, glm::vec3 items, float yRot);

	void Draw(int index);

private:

	float m_informDistance = 0.0f;

	glm::vec2 m_positon;
};