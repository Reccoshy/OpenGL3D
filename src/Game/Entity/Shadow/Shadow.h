#pragma once

#include "../../../GameEngine.h"
#include "../../../Entity.h"
#include <glm/glm.hpp>

class Shadow {
public:

	bool init(glm::vec3 pos, float scale);

	void Update(glm::vec3 pos);

	void ChangeVisible(bool isVisible);

	void Destroy();
	
private:

	float m_scale = 0.0f;

	Entity::Entity* m_pEntity = nullptr;
};