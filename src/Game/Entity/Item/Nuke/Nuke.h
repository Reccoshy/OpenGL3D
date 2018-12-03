#pragma once
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;


class CNuke {
public:
	bool Init(glm::vec3 pos, float rot, RaceScene* p);

	void Update(float delta);

	/*
	�G���e�B�e�B�̏ꏊ���擾.

	@return	�G���e�B�e�B�̏ꏊ.
	*/
	glm::vec3 Positioin()
	{
		return m_pEntity->Position();
	}

	/*
	�I�����̔j�󏈗�.
	*/
	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	/*
	�L���t���O�̊m�F.

	@return	�L���t���O.
	*/
	bool IsActive() {
		return m_isActive;
	}

private:
	//�e
	Shadow m_shadow;
	//���g�̃G���e�B�e�B.
	Entity::Entity* m_pEntity = nullptr;
	//�����Ă������.
	float m_Yrot = 0.0f;
	//��΂��X�s�[�h.
	float const m_velocity = 200.0f;
	//���݂̍���.
	float const m_height = 0.0f;

	//��������̌o�ߎ���.
	float m_time = 0.0f;
	//�������J�n���鎞��.
	float m_startFallTime = 0.5f;
	//���[�X�V�[���̃|�C���^.
	RaceScene* m_pRaceScene;
	//�L���t���O.
	bool m_isActive = true;
};