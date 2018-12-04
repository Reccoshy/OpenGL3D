#pragma once

#include <glm/glm.hpp>
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

enum class ItemsCode {
	NONE = -1,
	//SingleItem
	WIND = 0,
	FIRE = 1,
	WATER = 2,
	ELCTRO = 3,

	//WindMix
	SUPERSPEED = 4, //WIND x WIND
	AIMMISSILE = 5, //WIND x FIRE
	WINDWATEER = 6,
	WINDELECTRO = 7,

	//Firemix
	AIMMISSILE2 = 8,
	SPREADMISSILE = 9,
	SMOKE = 10,
	NUKE = 11,

	//WaterMix
	WATERWIND = 12,
	SMOKE2 = 13,
	SUPERBARRIER = 14,
	FAKEITEM = 15,

	//ElectroMix
	ELECTROWIND = 16,
	NUKE2 = 17,
	FAKEITEM2 = 18,
	BOMB = 19,

};

enum class ItemUsageType 
{
	NONE = 0,
	FRONT_ATTACK = 1,
	BACK_SET = 2,
	PLAYERBUF = 3,
};

class Item {
public:
	bool Init(glm::vec3 pos, int itemId);

	void Update(float delta);

	void ColorUpdate();

	int CollisionCheck(glm::vec3 pos, float radius);

	bool GetActive() { return m_active; }

	glm::vec3 Position() { return m_pEntity->Position(); }

	void Destroy() { 
		this->m_pEntity->Destroy(); 
		this->m_shadow.Destroy();
	}

	static ItemUsageType GetItemType(ItemsCode item);

private:
	//�A�C�e���̃G���e�B�e�B.
	Entity::Entity* m_pEntity;
	//�A�C�e����ID.
	int m_itemId = (int)ItemsCode::NONE;
	//�A�C�e���������ł���悤�ɂȂ�܂ł̃C���^�[�o��.
	float const respawnTime = 2.0f;
	//�A�C�e���L���t���O�܂ł̌o�ߎ���.
	float m_time = 0.0f;

	float m_radius = 1.5f;

	//�L���t���O.
	bool m_active = true;
	//�e.
	Shadow m_shadow;
	//��]�x��.
	float y_Rot = 0.0f;
};