#include "Item.h"
#include "../../../../res/Sound/Sound.h"
#include "../../../GameEngine.h"
#include <stdio.h>
#include <Time.h>

bool Item::Init(glm::vec3 pos, int Id)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, glm::vec3(0, 1, 50), "Aircraft", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Position(pos);

	this->itemId = Id;

	this->m_pEntity->Scale(glm::vec3(2));

	this->m_shadow.init(this->Position(), 0.01f);

	return true;
}

void Item::Update(float delta)
{
	y_Rot += delta * 5;
	if (y_Rot >= 360) {
		y_Rot -= 360;
	}

	m_pEntity->Rotation(glm::quat(glm::vec3(0, y_Rot, 0)));

	if (time > 0) {
		this->time -= delta;
	}
	if (time <= 0) {
		active = true;
	}

	this->ColorUpdate();

	m_pEntity->ToggleVisibility(this->active);

	m_shadow.Update(this->Position());
	m_shadow.ChangeVisible(m_pEntity->Visible());
}

void Item::ColorUpdate()
{
	switch ((ItemsCode)itemId)
	{
	case ItemsCode::WIND:
		m_pEntity->Color(glm::vec4(0, 1, 0, 1));
		break;

	case ItemsCode::FIRE:
		m_pEntity->Color(glm::vec4(1, 0, 0, 1));
		break;

	case ItemsCode::WATER:
		m_pEntity->Color(glm::vec4(0, 0, 1, 1));
		break;

	case ItemsCode::ELCTRO:
		m_pEntity->Color(glm::vec4(1, 1, 0, 1));
		break;

	default:
		break;
	}
}

int Item::CollisionCheck(glm::vec3 pos, float radius)
{
	if (!this->active) {
		return -1;
	}

	float distance = glm::distance(this->m_pEntity->Position(), pos);

	if (distance < radius) {

		this->time = this->respawnTime;

		this->active = false;

		return itemId;
	}

	return -1;
}

ItemUsageType Item::GetItemType(ItemsCode item)
{
	switch (item)
	{
	case ItemsCode::NONE:
		break;
	case ItemsCode::WIND:
		return ItemUsageType::PLAYERBUF;
		break;
	case ItemsCode::FIRE:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::WATER:
		return ItemUsageType::PLAYERBUF;
		break;
	case ItemsCode::ELCTRO:
		return ItemUsageType::BACK_SET;
		break;
	case ItemsCode::SUPERSPEED:
		return ItemUsageType::PLAYERBUF;
		break;
	case ItemsCode::AIMMISSILE:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::WINDWATEER:
		return ItemUsageType::NONE;
		break;
	case ItemsCode::WINDELECTRO:
		return ItemUsageType::NONE;
		break;
	case ItemsCode::AIMMISSILE2:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::SPREADMISSILE:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::SMOKE:
		return ItemUsageType::BACK_SET;
		break;
	case ItemsCode::NUKE:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::WATERWIND:
		return ItemUsageType::NONE;
		break;
	case ItemsCode::SMOKE2:
		return ItemUsageType::BACK_SET;
		break;
	case ItemsCode::SUPERBARRIER:
		return ItemUsageType::PLAYERBUF;
		break;
	case ItemsCode::FAKEITEM:
		return ItemUsageType::BACK_SET;
		break;
	case ItemsCode::ELECTROWIND:
		return ItemUsageType::NONE;
		break;
	case ItemsCode::NUKE2:
		return ItemUsageType::FRONT_ATTACK;
		break;
	case ItemsCode::FAKEITEM2:
		return ItemUsageType::BACK_SET;
		break;
	case ItemsCode::BOMB:
		return ItemUsageType::BACK_SET;
		break;
	default:
		return ItemUsageType::NONE;
		break;
	}
}
