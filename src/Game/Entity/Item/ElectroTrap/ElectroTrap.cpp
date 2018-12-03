#include "ElectroTrap.h"
#include "../../../../GameEngine.h"
#include "../../../../Game/Scene/RaceScene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

bool ElectroTrap::init(glm::vec3 pos, RaceScene* p)
{
	GameEngine& game = GameEngine::Instance();

	m_pEntity = game.AddEntity(0, pos, "Toroid", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Scale(glm::vec3(2));

	m_pEntity->Position(pos);

	m_shadow.init(pos, 0.01f);

	this->m_pRaceScene = p;
	this->m_pRaceScene->PlayAudioCheck(this->Position(), 1000, SHOOT, CRI_SOUND_ITEMPUT);

	return true;
}

bool ElectroTrap::CollisionCheck(glm::vec3 pos, float radius)
{
	if (m_isActive) {

		float distance = glm::distance(this->m_pEntity->Position(), pos);

		if (distance < radius) {
			this->m_isActive = false;
			this->m_pEntity->ToggleVisibility(false);
			this->m_shadow.ChangeVisible(false);

			this->m_pRaceScene->PlayAudioCheck(this->Position(), 1000, SHOOT, CRI_SOUND_SMALLEXPLODE);

			return true;
		}
	}
	return false;
}

void ElectroTrap::Destroy()
{
	this->m_pEntity->Destroy();
	this->m_shadow.Destroy();
}
