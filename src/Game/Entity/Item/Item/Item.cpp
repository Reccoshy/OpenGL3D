#include "Item.h"
#include "../../../../res/Sound/Sound.h"
#include "../../../../GameEngine.h"

/*
初期化処理.

@param	pos	アイテムの配置位置.
@param	Id	アイテムのID.
*/
bool Item::Init(glm::vec3 pos, int Id)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, glm::vec3(0, 1, 50), "Aircraft", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Position(pos);

	this->m_itemId = Id;

	this->m_pEntity->Scale(glm::vec3(2));

	this->m_shadow.init(this->Position(), 0.01f);

	return true;
}

/*
更新処理.

@param	delta	次のフレームまでの経過時間.
*/
void Item::Update(float delta)
{
	y_Rot += delta * 5;
	if (y_Rot >= 360) {
		y_Rot -= 360;
	}

	m_pEntity->Rotation(glm::quat(glm::vec3(0, y_Rot, 0)));

	if (m_time > 0) {
		this->m_time -= delta;
	}
	if (m_time <= 0) {
		m_active = true;
	}

	this->ColorUpdate();

	m_pEntity->ToggleVisibility(this->m_active);

	m_shadow.Update(this->Position());
	m_shadow.ChangeVisible(m_pEntity->Visible());
}

void Item::ColorUpdate()
{
	switch ((ItemsCode)m_itemId)
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

/*
アイテムが接触しているかどうかのチェック.

@param	アイテムと触れている機体の場所,
@param	機体のあたり判定の半径.

@return	アイテムのID.
*/
int Item::CollisionCheck(glm::vec3 pos, float radius)
{
	if (!this->m_active) {
		return -1;
	}

	float distance = glm::distance(this->m_pEntity->Position(), pos);

	
	//アイテム入手処理.
	if (distance < radius + m_radius) {

		this->m_time = this->respawnTime;
		this->m_active = false;

		return m_itemId;
	}

	return -1;
}

/*
アイテムの使用用途チェック.

@param	ItemsCode	プレイヤーが持っているアイテムのID.

@return	アイテムの使用用途を返す.
*/
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
		return ItemUsageType::BACK_SET;
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
