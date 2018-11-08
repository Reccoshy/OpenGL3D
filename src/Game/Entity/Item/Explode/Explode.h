#pragma once
#include <glm/glm.hpp>
#include "../../../../Entity.h"

class RaceScene;

//�_���[�W���܂ޔ���.
class Explosion {
public:
	bool Init(glm::vec3 pos, RaceScene* p);

	void Update(float delta);

	bool CollisionCheck(glm::vec3 pos, float radius);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
	}

	bool IsActive() {
		return this->active;
	}

private:
	
	RaceScene* m_pRaceScene = nullptr;
	//�G���e�B�e�B�̃|�C���^.
	Entity::Entity* m_pEntity;
	//�L�����ǂ���.
	bool active = false;
	//�j�󔻒�̂���傫��.
	float m_radius = 35.0f;
	//�������Ă��鎞��.
	float explodeTime = 1.0f;
	//�����̃T�C�Y���ō��ɂȂ鎞��.
	float peakTime = 0.9f;
	//�����̍ő�T�C�Y.
	float scale = 0.01f;
};