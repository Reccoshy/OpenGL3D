#include "Nuke.h"
#include "../../../../GameEngine.h"
#include <iostream>
#include "../../../Scene/RaceScene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

/*
初期化処理.

@param	pos		アイテムを出現させる位置.
@param	rot		アイテムを飛ばす方向,
@param	p		レースシーンのポインタ.
*/
bool CNuke::Init(glm::vec3 pos, float rot, RaceScene* p)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Aircraft", "res/Model/Toroid.bmp", nullptr);
	this->m_pEntity->Rotation(glm::quat(glm::vec3(80, 0, 0)));
	this->m_pEntity->Scale(glm::vec3(3));

	this->m_Yrot = rot;

	m_pRaceScene = p;

	this->m_shadow.init(pos, 0.01f);

	p->PlayAudioCheck(this->Positioin(), 1000, SHOOT, CRI_SOUND_BARRIERACTIVE);

	return true;
}

/*
更新処理.

@param	delta	1フレームの経過時間.
*/
void CNuke::Update(float delta)
{
	if (this->m_isActive) {

		this->m_time += delta;

		float x = glm::sin(glm::radians(m_Yrot)) * m_velocity * delta;
		float z = glm::cos(glm::radians(m_Yrot)) * m_velocity * delta;

		if (m_time <= m_startFallTime) {
			this->m_pEntity->Position(glm::vec3(m_pEntity->Position().x + x, 0 + m_time * m_velocity, m_pEntity->Position().z + z));
		}

		else if (m_time > m_startFallTime)
		{
			this->m_pEntity->Position(glm::vec3(m_pEntity->Position().x + x, m_time * 0 + m_velocity * (m_startFallTime * 2 - m_time), m_pEntity->Position().z + z));

			if (m_time >= m_startFallTime * 2) {
				
				this->m_pEntity->ToggleVisibility(false);
				this->m_shadow.ChangeVisible(false);
				this->m_isActive = false;
				m_pRaceScene->SpawnExplode(this->m_pEntity->Position());
			}
		}
		this->m_shadow.Update(Positioin());
	}
}