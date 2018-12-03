#include "Smoke.h"
#include "../../../../GameEngine.h"
#include <iostream>
#include "../../../../Game/Scene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

bool Smoke::Init(glm::vec3 pos, float rot, RaceScene* p)
{
	GameEngine& game = GameEngine::Instance();

	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Cube", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Rotation(glm::quat(glm::vec3(0, rot, 0)));

	this->m_Yrot = rot;

	this->m_pRaceScene = p;

	p->PlayAudioCheck(this->Position(), 1000, OTHERS, CRI_SOUND_ITEMPUT);

	return true;
}

void Smoke::Update(float delta)
{
	if (this->isActive) {

		time += delta;

		this->m_pEntity->Position(m_pEntity->Position() + glm::vec3(0, 1 * delta, 0));

		if (this->time < SpawnTime) {
			this->scale += delta;
		}
		else if (this->time < SmokeTime) {
			this->scale += delta * 0.1;
		}

		float alpha = (SmokeTime - time) * 0.1 * 0.8;

		this->m_pEntity->Scale(glm::vec3(scale * 0.1));
		this->m_pEntity->Color(glm::vec4(1, 1, 1, alpha)); 

		if (this->time > SmokeTime) {
			this->m_pEntity->ToggleVisibility(false);
			this->isActive = false;
		}
	}
}