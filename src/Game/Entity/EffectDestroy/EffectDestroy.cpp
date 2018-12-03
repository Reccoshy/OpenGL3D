#include "EffectDestroy.h"
#include "../../../GameEngine.h"
#include "../../../Game/Scene/RaceScene.h"

bool EffectDestroy::Init(glm::vec3 pos)
{
	GameEngine& game = GameEngine::Instance();

	m_pEntity = game.AddEntity(0, pos, "Blast", "res/Model/Toroid.bmp", nullptr);

	return true;
}

void EffectDestroy::Update(float delta)
{
	if (m_active) {
		if (m_lifeTime > 0) {
			m_lifeTime -= delta;

			m_pEntity->Scale(glm::vec3(4 - m_lifeTime * 4));
			m_pEntity->Rotation(glm::vec3(0, m_lifeTime * 2, 0));

			ChangeColor();
		}
		else {
			m_pEntity->ToggleVisibility(false);
			this->m_active = false;
		}
	}
}

void EffectDestroy::Destroy()
{
	this->m_pEntity->Destroy();
}

void EffectDestroy::ChangeColor()
{
	if (m_lifeTime < 0.1) {
		m_pEntity->Color(glm::vec4(0.25f, 0.1f, 0.1f, 1));
	}
	else if (m_lifeTime < 0.3) {
		m_pEntity->Color(glm::vec4(1.0f, 0.5f, 0.1f, 1));
	}
	else {
		m_pEntity->Color(glm::vec4(1.0f, 1.0f, 0.75f, 1));
	}
}
