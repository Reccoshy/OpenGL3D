#include "Missile.h"
#include "../../../../GameEngine.h"
#include "../../../Entity/Player/Player.h"
#include "../../../Scene/RaceScene.h"
#include "../../../../res/Sound/Sound.h"
#include <iostream>

bool Missile::init(glm::vec3 pos, float rot, RaceScene* pScene, bool target)
{
	GameEngine& game = GameEngine::Instance();

	m_pEntity = game.AddEntity(0, pos, "NormalShot", "res/Model/Player.bmp", nullptr);

	m_Yrot = rot;

	this->m_pEntity->Scale(glm::vec3(5));

	this->m_pEntity->Rotation(glm::quat(glm::vec3(0, glm::radians(rot), 0)));

	this->m_Aiming = target;

	this->m_pRaceScene = pScene;

	m_shadow.init(pos, 0.01f);

	return true;
}

void Missile::Update(float delta, std::vector<CPlayerCharacter*> players)
{
	if (m_isActive) {

		if (m_Aiming) {
			this->DetectPlayersAndChase(delta, players);
		}

		float x = sin(glm::radians(m_Yrot)) * m_velocity * delta;
		float z = cos(glm::radians(m_Yrot)) * m_velocity * delta;

		glm::vec3 pos = glm::vec3(x, 0, z) + m_pEntity->Position();
		m_pEntity->Rotation(glm::quat(glm::vec3(0, glm::radians(m_Yrot), 0)));

		this->m_pEntity->Position(pos);

		this->m_shadow.Update(pos);
	}
}

bool Missile::CollisionCheck(glm::vec3 pos, float radius)
{
	if (m_isActive) {

		float distance = glm::distance(this->m_pEntity->Position(), pos);

		if (distance < radius) {

			this->m_isActive = false;

			this->m_pEntity->ToggleVisibility(false);
			this->m_shadow.ChangeVisible(false);

			this->m_pRaceScene->SpawnEffectExplode(this->m_pEntity->Position());

			m_pRaceScene->PlayAudioCheck(this->Position(), 1000, OTHERS, CRI_SOUND_SMALLEXPLODE);

			return true;
		}
	}
	return false;
}

void Missile::DetectPlayersAndChase(float delta, std::vector<CPlayerCharacter*> players)
{
	glm::vec3 target;
	int detectNum = 0;

	for (int i = 0; i < players.size(); i++) {
		glm::vec3 test = players[i]->Position();

		double radian = atan2(test.x - m_pEntity->Position().x, test.z - m_pEntity->Position().z);
		double degree = glm::degrees(radian);

		int angle = degree - m_Yrot;
		if (angle < 30.0 && angle > -30.0) {

			if (detectNum == 0) {
				target = test;
			}
			else {
				float testD = glm::distance(test, m_pEntity->Position());
				float targetD = glm::distance(target, m_pEntity->Position());

				if (testD < targetD) {
					target = test;
				}
			}
			detectNum++;
		}
	}

	double radian = atan2(target.x - m_pEntity->Position().x, target.z - m_pEntity->Position().z);
	double degree = glm::degrees(radian);

	if (degree < 0) {
		degree += 360;
	}

	float checker = this->m_Yrot;

	if (degree < 90 || degree > 360 - 90) {
		checker += 180;
		if (checker > 360) {
			checker -= 360;
		}
		degree += 180;
		if (degree > 360) {
			degree -= 360;
		}
	}

	float bet = 1.0f;

	if (degree + bet < checker) {
		m_Yrot -= m_turnRate * delta;
	}
	else if (degree - bet > checker)
	{
		m_Yrot += m_turnRate * delta;
	}

	std::cout << checker << " " << degree << std::endl;
}

void Missile::Destroy()
{
	m_pEntity->Destroy();
	m_shadow.Destroy();
}