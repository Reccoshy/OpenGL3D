#pragma once

#include <glm/glm.hpp>
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;

//����̓����锚�e.
class Bomb {
public:
	bool Init(glm::vec3 pos, float yRot, RaceScene* rs);

	void Update(float delta);

	/*
	�G���e�B�e�B�̈ʒu�̎擾.

	@return	�G���e�B�e�B�̈ʒu.
	*/
	glm::vec3 Position() {
		return m_pEntity->Position();
	}

	/*
	�L���t���O������̔j�󏈗�.
	*/
	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	/*
	�L���t���O�̃`�F�b�N.

	@return �L���t���O.
	*/
	bool IsActive() {
		return m_active;
	}

private:

	Shadow m_shadow;
	//���[�X�V�[���̃|�C���^.
	RaceScene* m_pRaceScene;
	//�G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity;
	//�L�����ǂ���.
	bool m_active = true;
	//�ړ����x(����ɂ��̑��x�ňړ�)
	float m_velocity = 80.0f;
	//���ʂ̊p�x.
	float yRot = 0.0f;
	//��������܂ł̎���.
	float explodeTime = 0.5f;
};