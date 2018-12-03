#pragma once

#include <glm/glm.hpp>

#include "../../../Entity.h"

//��Q���N���X
class Obstacle {
public:
	
	void Init(glm::vec3 pos, float radius);

	/*
	���a���擾����.

	@return ���a���擾.
	*/
	float Radius() { return m_colRadius; }
	
	/*
	�G���e�B�e�B�̏ꏊ���擾����.
	
	@return	�G���e�B�e�B�̏ꏊ.
	*/
	glm::vec3 Position() { return m_pEntity->Position(); }

	/*
	�G���e�B�e�B�̎擾.

	@return	�G���e�B�e�B�̃|�C���^.
	*/
	Entity::Entity* Entity() { return m_pEntity; }

private:
	//��Q���̃G���e�B�e�B.
	Entity::Entity* m_pEntity = nullptr;
	//�����蔻��p�̔��a.
	float m_colRadius = 0.0f;
};