#include "Smoke.h"
#include "../../../../GameEngine.h"
#include <iostream>
#include "../../../../Game/Scene/RaceScene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

/*
初期化処理.

@param	pos		配置位置.
@param	rot		モデルの回転度数.
@param	p		レースシーンのポインタ.
*/
bool Smoke::Init(glm::vec3 pos, float rot, RaceScene* p)
{
	GameEngine& game = GameEngine::Instance();
	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Cube", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Rotation(glm::quat(glm::vec3(0, rot, 0)));

	p->PlayAudioCheck(this->Position(), 1000, OTHERS, CRI_SOUND_ITEMPUT);

	return true;
}

/*
更新処理.

@param delta	1フレームの更新までの時間.
*/
void Smoke::Update(float delta)
{
	if (this->m_isActive) {

		m_time += delta;

		this->m_pEntity->Position(m_pEntity->Position() + glm::vec3(0, 1 * delta, 0));

		if (this->m_time < m_spawnTime) {
			this->m_scale += delta;
		}
		else if (this->m_time < m_smokeTime) {
			this->m_scale += delta * 0.1;
		}

		float alpha = (m_smokeTime - m_time) * 0.1 * 0.8;

		this->m_pEntity->Scale(glm::vec3(m_scale * 0.1));
		this->m_pEntity->Color(glm::vec4(1, 1, 1, alpha)); 

		if (this->m_time > m_smokeTime) {
			this->m_pEntity->ToggleVisibility(false);
			this->m_isActive = false;
		}
	}
}