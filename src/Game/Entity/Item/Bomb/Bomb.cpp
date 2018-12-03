#include "Bomb.h"
#include "../../../../GameEngine.h"
#include "../../../Scene/RaceScene.h"

/*
初期化処理.

@param	pos		アイテムを出す場所.
@param	yRot	アイテムを飛ばす方向.
@param	rs		レースシーンのポインタ.
*/
bool Bomb::Init(glm::vec3 pos, float yRot, RaceScene* rs)
{
	m_pEntity = GameEngine::Instance().AddEntity(0, pos, "pPlane2", "res/Model/Player.bmp", nullptr);

	this->yRot = yRot;

	this->m_pEntity->Scale(glm::vec3(0.1f));

	this->m_shadow.init(pos, 0.01f);

	this->m_pRaceScene = rs;

	return false;
}

/*
更新処理.

@param	delta	1フレームでの経過時間.
*/
void Bomb::Update(float delta)
{
	if (m_active) {
		this->explodeTime -= delta;

		float x = glm::sin(glm::radians(yRot)) * m_velocity * delta;
		float z = glm::cos(glm::radians(yRot)) * m_velocity * delta;

		this->m_pEntity->Position(glm::vec3(m_pEntity->Position().x + x, m_pEntity->Position().y + 5 * delta, m_pEntity->Position().z + z));

		this->m_shadow.Update(Position());

		if (this->explodeTime <= 0) {
			this->m_pEntity->ToggleVisibility(false);
			this->m_shadow.ChangeVisible(false);
			
			m_pRaceScene->SpawnExplode(this->m_pEntity->Position());

			m_active = false;
		}
	}
}