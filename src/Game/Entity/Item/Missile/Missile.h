#pragma once

#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"
#include <vector>

class CPlayerCharacter;
class RaceScene;

//���[�X�A�C�e���~�T�C��.
class Missile {
public:
	
	bool Init(glm::vec3 pos, float rot, RaceScene* m_pScene, bool aiming = false);


	void Update(float delta, std::vector<CPlayerCharacter*> player);


	bool CollisionCheck(glm::vec3 pos, float radius);


	void DetectPlayersAndChase(float delta, std::vector<CPlayerCharacter*> players);

	void SpawnEffects();

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	bool IsActive() { return m_isActive; }

	void Destroy();

private:
	//�e
	Shadow m_shadow;

	//�g�p���Ă���G���e�B�e�B�̃|�C���^.
	Entity::Entity* m_pEntity = nullptr;
	
	//���[�X���̃V�[��.
	RaceScene* m_pRaceScene = nullptr;


	float m_radius = 1.0f;
	//�����Ă������.
	float m_Yrot = 0.0f;

	//1�b�œ��B���鑬��.
	float const m_velocity = 200.0f;
	//1�b�ŉ�]����.
	float const m_turnRate = 20.0f;

	//�A�C�e�����L����.
	bool m_isActive = true;

	//�ǔ����邩�ǂ���.
	bool m_Aiming = true;
};