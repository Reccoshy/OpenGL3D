#include "Explode.h"
#include "../../../../GameEngine.h"
#include "../../../../Game/Scene/RaceScene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

bool Explosion::Init(glm::vec3 pos, RaceScene* p)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, glm::vec3(pos), "Explode", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Scale(glm::vec3(0.4));

	this->m_pEntity->Color(glm::vec4(1, 0.5, 0, 1));

	active = true;

	this->m_pRaceScene = p;
	p->PlayAudioCheck(this->Position(), 1000, EXPLODE, CRI_SOUND_BIGEXPLODE);

	return true;
}

void Explosion::Update(float delta)
{
	if (active) {
		
		this->explodeTime -= delta;

		if (explodeTime > peakTime) {
			scale = 0.4 * (1 - explodeTime) / (1 - peakTime);
		}
		m_pEntity->Scale(glm::vec3(m_pEntity->Scale() + delta * 5));

		this->m_pEntity->Rotation(glm::vec3(0, explodeTime, 0));

		this->m_pEntity->Scale(glm::vec3(scale));

		this->m_pEntity->Color(glm::vec4(1, 0.5, 0, explodeTime * 0.1 + 0.9));

		if (explodeTime < 0) {
			this->m_pEntity->ToggleVisibility(false);
			active = false;
		}

	}
}

bool Explosion::CollisionCheck(glm::vec3 pos, float radius)
{
	if (active) {
		float distance = glm::distance(this->m_pEntity->Position(), pos);

		if (distance < m_radius + radius) {
			return true;
		}
	}

	return false;
}
