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

	glm::vec3 Position() {
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	bool IsActive() {
		return active;
	}

private:

	Shadow m_shadow;
	//���[�X�V�[���̃|�C���^.
	RaceScene* m_pRaceScene;
	//�G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity;
	//�L�����ǂ���.
	bool active = true;
	//�ړ����x(����ɂ��̑��x�ňړ�)
	float m_velocity = 80.0f;
	//���ʂ̊p�x.
	float yRot = 0.0f;
	//��������܂ł̎���.
	float explodeTime = 0.5f;
};