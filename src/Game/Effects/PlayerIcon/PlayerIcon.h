#pragma once

#include <glm/glm.hpp>

class PlayerIcon {
public:
	bool Init(int index, float height, glm::vec3 pos);

	void Update(glm::vec3 pos);

	void Draw();

private:

	glm::vec3 m_position;

	float m_height;

	int m_index;
};