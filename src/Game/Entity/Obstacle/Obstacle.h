#pragma once

#include <glm/glm.hpp>

#include "../../../Entity.h"

class Obstacle {
public:
	void Init(glm::vec3 pos, float raidus);

	float Radius() { return m_colRadius; }
	glm::vec3 Position() { return m_pEntity->Position(); }
	Entity::Entity* Entity() { return m_pEntity; }

private:
	Entity::Entity* m_pEntity = nullptr;

	float m_colRadius = 0.0f;
};