#include "Nuke.h"
#include "../../../../GameEngine.h"
#include <iostream>
#include "../../../Scene/RaceScene.h"
#include "../../../../../res/Sound/Sound.h"

bool CNuke::init(glm::vec3 pos, float rot, RaceScene* p)
{
	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Aircraft", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Rotation(glm::quat(glm::vec3(80, 0, 0)));

	this->m_pEntity->Scale(glm::vec3(3));

	this->m_Yrot = rot;

	pRaceScene = p;

	this->m_shadow.init(pos, 0.01f);

	p->PlayAudioCheck(this->Positioin(), 1000, OTHERS, CRI_SOUND_BARRIERACTIVE);

	return true;
}

void CNuke::Update(float delta)
{
	if (this->isActive) {

		this->m_time += delta;

		float x = glm::sin(glm::radians(m_Yrot)) * m_velocity * delta;
		float z = glm::cos(glm::radians(m_Yrot)) * m_velocity * delta;

		std::cout << m_pEntity->Position().x << " " << m_pEntity->Position().y << " " << m_pEntity->Position().z << std::endl;

		if (m_time <= m_startFallTime) {
			this->m_pEntity->Position(glm::vec3(m_pEntity->Position().x + x, 0 + m_time * m_velocity, m_pEntity->Position().z + z));
		}

		else if (m_time > m_startFallTime)
		{
			this->m_pEntity->Position(glm::vec3(m_pEntity->Position().x + x, m_time * 0 + m_velocity * (m_startFallTime * 2 - m_time), m_pEntity->Position().z + z));

			if (m_time >= m_startFallTime * 2) {
				
				this->m_pEntity->ToggleVisibility(false);
				this->m_shadow.ChangeVisible(false);
				this->isActive = false;
				pRaceScene->SpawnExplode(this->m_pEntity->Position());
			
			}
		}
		this->m_shadow.Update(Positioin());
	}
}