#pragma once

#include <glm/glm.hpp>
#include "../../../Entity.h"

//�l�p�̃R���W����
struct Collision {
	glm::vec3 min;
	glm::vec3 max;
};

//�ړI�n��
class Goal {
public:
	bool Init(glm::vec3 pos, glm::vec3 min = glm::vec3(-10, -10, -10), glm::vec3 max = glm::vec3(10, 100, 10));

	bool CollisionCheck(glm::vec3 pos, float radius);

	bool ToggleVisible(bool b);

	glm::vec3 Position() { return m_pEntity->Position(); }

	void Destroy() {
		m_pEntity->Destroy();
	}

private:

	glm::vec3 CalcClosestPoint(glm::vec3 pos, float r);

private:
	//�G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity;
	//���̖ړI�n�̃C���f�b�N�X(0�̓S�[���n�_).
	int m_checkPointIndex = -1;
	//���������ꂽ��.
	bool m_initialized = false;
	//�S�[���̈ʒu.
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	//�S�[���𔻒肷��ʒu.
	Collision m_collision = { glm::vec3(-10, -10, -10), glm::vec3(10, 100, 10) };
};