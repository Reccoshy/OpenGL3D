#include "../Item/Item.h"
#include "../../../../../res/Sound/Sound.h"
#include "../../../../GameEngine.h"
#include <stdio.h>
#include <Time.h>
#include "FakeItem.h"
#include "../../../../Game/Scene/RaceScene/RaceScene.h"

bool FakeItem::Init(glm::vec3 pos, RaceScene* pSce)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, glm::vec3(0, 1, 50), "Aircraft", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Position(pos);
	m_shadow.init(pos, 0.01f);

	this->m_itemId = rand() % 4;

	this->m_pEntity->Scale(glm::vec3(2));

	this->m_pRaceScene = pSce;

	this->m_pRaceScene->PlayAudioCheck(this->Position(), 1000, SHOOT, CRI_SOUND_ITEMPUT);

	return true;
}

void FakeItem::Update(float delta)
{
	if (m_active) {

		y_Rot -= delta * 5;
		if (y_Rot <= 0) {
			y_Rot += 360;
		}

		m_pEntity->Rotation(glm::quat(glm::vec3(0, y_Rot, 0)));

		this->ColorUpdate();

		m_pEntity->ToggleVisibility(this->m_active);
		this->m_shadow.Update(Position());
	}
}

void FakeItem::ColorUpdate()
{
	switch ((ItemsCode)m_itemId)
	{
	case ItemsCode::WIND:
		m_pEntity->Color(glm::vec4(0, 0.5, 0, 1));
		break;

	case ItemsCode::FIRE:
		m_pEntity->Color(glm::vec4(0.5, 0, 0, 1));
		break;

	case ItemsCode::WATER:
		m_pEntity->Color(glm::vec4(0, 0, 0.5, 1));
		break;

	case ItemsCode::ELCTRO:
		m_pEntity->Color(glm::vec4(0.5, 0.5, 0, 1));
		break;

	default:
		break;
	}
}

bool FakeItem::CollisionCheck(glm::vec3 pos, float radius)
{
	if (!this->m_active) {
		return false;
	}

	float distance = glm::distance(this->m_pEntity->Position(), pos);

	if (distance < radius) {

		this->m_active = false;

		this->m_pEntity->ToggleVisibility(false);

		this->m_shadow.ChangeVisible(false);

		m_pRaceScene->PlayAudioCheck(this->Position(), 50, OTHERS, CRI_SOUND_SMALLEXPLODE);

		return true;
	}

	return false;
}
