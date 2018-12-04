#include "Player.h"
#include "../../../GameEngine.h"
#include "../../Scene/RaceScene/RaceScene.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include "../../../GameEngine.h"
#include "../../Entity/Goal/Goal.h"
#include "../../../res/Sound/Sound.h"

bool CPlayerCharacter::Init(glm::vec3 pos, int goalNum, float r, int playerIndex, RaceScene* pScene, std::vector<glm::vec3> g, bool isPlayer)
{
	GameEngine& game = GameEngine::Instance();

	m_entity = game.AddEntity(0, glm::vec3(0, 0, 0), "Aircraft", "res/Model/Player.bmp", nullptr);

	m_entity->Position(pos);
	m_entity->Scale(glm::vec3(2));

	glm::vec4 color[4] =
	{
		glm::vec4(1.0, 0.5, 0.5, 1),
		glm::vec4(0.5, 0.5, 1.0, 1),
		glm::vec4(1.0, 1.0, 0.5, 1),
		glm::vec4(0.5, 1.0, 0.5, 1),
	};

	m_entity->Color(glm::vec4(color[playerIndex]));

	this->loadRideInfo("res/Data/RideParameter/RideParamater.txt");

	this->m_radius = 2;
	this->m_playerIndex = playerIndex;
	this->MaxCheckPointIndex = goalNum - 1;

	m_pBarrier = game.AddEntity(0, this->Position(), "Cube", "res/Model/Toroid.bmp", nullptr);
	m_pBarrier->Scale(glm::vec3(0.02f));
	m_pBarrier->ToggleVisibility(false);

	this->m_Shadow.init(this->Position(), 0.01f);

	this->m_pRaceScene = pScene;

	if (isPlayer) {
		this->activeInput = true;
		this->m_autoDriveFlag = false;
		this->m_cameraActiveFlag = true;
	}
	else {
		this->activeInput = false;
		this->m_autoDriveFlag = true;
		this->m_cameraActiveFlag = false;
	}

	for (int i = 0; i < g.size(); i++) {
		this->m_goals.push_back(g[i]);
	}

	return true;
}

/*
更新処理.

@param	delta	
@param	Time	ゴールまでの時間.
*/
bool CPlayerCharacter::Update(float delta)
{
	this->InputFunc(delta);
	this->MoveFunc(delta);
	this->SpawnSmokes(delta);
	this->ItemStateUpdate(delta);
	this->CameraUpdate(delta);
	this->RespawnFunc(delta);

	m_Shadow.Update(Position());
	m_Shadow.ChangeVisible(m_entity->Visible());

	return true;
}

void CPlayerCharacter::PassCheckPoint(int passedIndex, float time)
{
	if (passedIndex == m_checkPointIndex) {
		return;
	}

	if (passedIndex == m_nextCheckPoint) {

		this->m_checkPointIndex = passedIndex;
		this->m_nextCheckPoint = m_checkPointIndex + 1;

		if (passedIndex == MaxCheckPointIndex) {
			this->m_nextCheckPoint = 0;
		}
		if (passedIndex == 0) {
			this->PassGoal(time);
		}
	}
}

void CPlayerCharacter::PassGoal(float Time)
{
	float LapTime = Time;

	for (int i = 0; i < m_lapTime.size(); i++) {
		LapTime -= m_lapTime[i];
	}

	m_lapTime.push_back(LapTime);

	m_lap += 1;

	if (m_lap > m_pRaceScene->GetLapNum()) {
		this->m_autoDriveFlag = true;
		this->FinishedRace = true;
	}

	this->PlayAudioCheck(SEUI, CRI_SOUND_PASSGAOL);
}

void CPlayerCharacter::SetPlayerRank(int num)
{
	m_rank = num;
}

void CPlayerCharacter::ObtainItem(int id)
{
	if (id == -1) {
		return;
	}

	if (m_type != ItemUsageType::PLAYERBUF && Item::GetItemType((ItemsCode)id) == ItemUsageType::PLAYERBUF) {
		this->m_BuffUseTime = rand() % (BuffMaxTime - BuffMinTime) + BuffMinTime;
	}

	if (itemId != -1) {
		m_stockItems = itemId;
	}

	if (itemId != -1 && m_stockItems != -1) {
		itemMixable = true;
	}

	this->PlayAudioCheck(PLAYER, CRI_SOUND_ITEMGET);

	itemId = id;

	m_type = Item::GetItemType((ItemsCode)itemId);
}

void CPlayerCharacter::InputFunc(float delta)
{
	GameEngine& game = GameEngine::Instance();
	GamePad gamePad = game.GetGamePad(m_playerIndex);

	if (gamePad.buttonDown & GamePad::START) {
		m_pRaceScene->TogglePause(m_playerIndex);
		this->m_optionPressedCheck = true;
	}

	if (!this->activeInput || this->m_autoDriveFlag) {
		return;
	}
	
	float camDif = GLFWEW::Window::instance().GetBackAxe(m_playerIndex);

	CameraYDif = 180 + camDif * 180;

	m_entity->Velocity(glm::vec3(0, 0, 0));

	if (gamePad.buttons & GamePad::A) {
		m_velocity += m_accel * delta;
		
		if (m_velocity > MaxSpeed) {
			m_velocity = MaxSpeed;
		}
	}

	if (gamePad.buttons & GamePad::X) {
		m_velocity -= m_break * delta;	
		if (m_velocity < MaxBackWardSpeed) {
			m_velocity = MaxBackWardSpeed;
		}
	}
	if (gamePad.buttonDown & GamePad::B) {
		ObtainItem((int)ItemsCode::FIRE);
	}

	if (gamePad.buttonDown & GamePad::Y) {
		this->SwapItemStock();
	}


	this->itemMixReady = false;
	if (gamePad.buttons & GamePad::L) {
		if ((ItemsCode)this->itemId != ItemsCode::NONE && (ItemsCode)this->m_stockItems != ItemsCode::NONE) {
			this->itemMixReady = true;
		}
	}

	if (gamePad.buttonDown & GamePad::R) {
		if (itemMixReady) {
			this->MixItem();
		}

		this->UseItem();
	}

	float yRot = m_Yrot;

	float LeftX = -GLFWEW::Window::instance().GetLeftAxe(this->m_playerIndex).x;

	yRot += LeftX * 30;

	if (LeftX != 0.0) {
		m_Yrot += LeftX * m_turnRate * delta;
	}
	else if (gamePad.buttons & GamePad::DPAD_LEFT) {
		m_Yrot += m_turnRate * delta;
	}
	else if (gamePad.buttons & GamePad::DPAD_RIGHT) {
		m_Yrot -= m_turnRate * delta;
	}

	m_entity->Rotation(glm::quat(glm::vec3(0, glm::radians(yRot), 0)));
}

bool CPlayerCharacter::loadRideInfo(const char * filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}
	
	int line = 0;

	while (fscanf(fp, "%lf,%lf,%lf,%lf", &m_accel, &MaxSpeed, &m_break, &m_turnRate) != EOF)
	{
		line++;
	}

	fclose(fp);

	return true;
}

void CPlayerCharacter::MoveFunc(float delta)
{
	if (m_velocity > 0) {
		m_velocity -= m_break * delta;
	}

	if (m_velocity > MaxSpeed + CurrentSpeedUpTime * 5) {
		m_velocity = MaxSpeed + CurrentSpeedUpTime * 5;
	}
	else if (m_velocity < MaxBackWardSpeed) {
		m_velocity = MaxBackWardSpeed;
	}

	if (currentBounceTime > 0.0f) {
		currentBounceTime -= delta;
	}
	if (currentBounceTime <= 0.0f) {
		currentBounceTime = 0.0f;
		this->m_bounceVel = glm::vec3(0);
	}

	if (m_velocity < 0 && StopTime < 1.0) {
		m_velocity = 0;
	}

	if (m_velocity <= 0) {
		StopTime += delta;
	}
	else {
		StopTime = 0.0f;
	}

	float m_speedUpper = 50.0f * this->CurrentSpeedUpTime;

	float x = sin(glm::radians(m_Yrot + 0)) * (m_velocity + m_speedUpper);
	float z = cos(glm::radians(m_Yrot + 0)) * (m_velocity + m_speedUpper);

	if (m_Yrot >= 360) {
		m_Yrot -= 360;
	}
	else if (m_Yrot < 0) {
		m_Yrot += 360;
	}

	float ratio = currentBounceTime / bounceTime;
	glm::vec3 bouncer = m_bounceVel * ratio;

	m_actualVel = glm::vec3(x, 0, z) + bouncer;
	m_acctualSpeed = m_velocity + m_speedUpper;

	m_entity->Position(m_entity->Position() + m_actualVel * delta);

	if (m_acctualSpeed > 0.0f) {
		m_pRaceScene->SpawnEffectAccelDust(Position());
	}
}

void CPlayerCharacter::MixItem()
{
	if (m_stockItems >= 0 && itemId >= 0) {

		this->itemId = (itemId + 1) * 4 + m_stockItems;

		m_stockItems = -1;
	}
}

void CPlayerCharacter::SwapItemStock()
{
	std::swap(itemId, m_stockItems);
}

void CPlayerCharacter::SpawnSmokes(float delta)
{
	if (m_smokeEmitting) {

		m_smokeTimer -= delta;
		if (m_smokeTimer < 0) {
			m_pRaceScene->SpawnSmoke(Position(), Yrot());

			this->m_smokeEmitCount += 1;

			if (m_smokeEmitCount >= m_smokeEmitingEndCount) {
				this->m_smokeEmitting = false;
				this->m_smokeEmitCount = 0;
				this->m_smokeTimer = 0.0f;
			}
			else {
				this->m_smokeTimer -= this->m_smokeEmitInterval;
			}
		}
	}
}

void CPlayerCharacter::ItemStateUpdate(float delta)
{
	float alpha = CurrentShieldTime * 0.5f;
	if (alpha >= 0.5f) {
		alpha = 0.5f;
	}

	m_pBarrier->Color(glm::vec4(m_pBarrier->Color().r, m_pBarrier->Color().g, m_pBarrier->Color().b, alpha));

	m_pBarrier->Position(this->Position());
	m_pBarrier->Rotation(glm::quat(glm::vec3(0, CurrentShieldTime, 0)));

	if (CurrentShieldTime > 0) {
		CurrentShieldTime -= delta;

		float alpha = CurrentShieldTime;

		if (CurrentShieldTime <= 0) {
			CurrentShieldTime = 0;
			
			this->activeShield = false;

			m_pBarrier->ToggleVisibility(false);
		}
	}
	if (CurrentSpeedUpTime > 0) {
		CurrentSpeedUpTime -= delta;

		m_pRaceScene->SpawnEffectSpeedUp(Position(), Yrot(), m_acctualSpeed);

		if (CurrentSpeedUpTime <= 0) {
			CurrentSpeedUpTime = 0;
		}
	}

	return;
}

void CPlayerCharacter::RespawnFunc(float delta)
{
	if (CurrentRespawnTime > 0.0 && m_dead) {
		CurrentRespawnTime -= delta;

		if (CurrentRespawnTime <= 0.0) {
			CurrentRespawnTime = 0.0;

			m_dead = false;
			activeInput = true;
			
			this->CurrentShieldTime = 1.0f;
			this->m_entity->Position(glm::vec3(m_spawningPoint.x, m_spawningPoint.y, m_spawningPoint.z));
			this->m_entity->ToggleVisibility(true);
		}
	}
}

void CPlayerCharacter::PlayAudioCheck(int audioType, int audioId)
{
	if (!m_autoDriveFlag) {
		
		GameEngine& game = GameEngine::Instance();

		game.PlayAudio(audioType, audioId);
	}
}

void CPlayerCharacter::UseItem()
{
	ItemsCode item = (ItemsCode)itemId;

	if (item != ItemsCode::NONE) {
		switch (item)
		{
		case ItemsCode::WIND:
			this->CurrentSpeedUpTime = this->SpeedUpTime;
			break;
		case ItemsCode::FIRE:
		{
			float x = sin(glm::radians(m_Yrot)) * m_radius * 1.1;
			float z = cos(glm::radians(m_Yrot)) * m_radius * 1.1;

			glm::vec3 pos = glm::vec3(x, 0, z) + Position();

			m_pRaceScene->SpawnMissile(pos, m_Yrot);

			this->PlayAudioCheck(SHOOT, CRI_SOUND_ITEMSHOOT);
		}
			break;
		case ItemsCode::WATER:
			this->CurrentShieldTime = this->ShieldTime;

			this->m_pBarrier->Color(glm::vec4(0, 0, 1, 0.5));
			this->m_pBarrier->ToggleVisibility(true);
			this->activeShield = true;

			this->PlayAudioCheck(PLAYER, CRI_SOUND_BARRIERACTIVE);

			break;
		case ItemsCode::ELCTRO:
		{
			float x = sin(glm::radians(m_Yrot)) * -8;
			float z = cos(glm::radians(m_Yrot)) * -8;

			glm::vec3 pos = glm::vec3(x, 0, z) + Position();

			m_pRaceScene->SpawnElectroTrap(pos);

			this->PlayAudioCheck(SHOOT, CRI_SOUND_ITEMPUT);
		}
			break;
		case ItemsCode::SUPERSPEED:
			this->CurrentSpeedUpTime = this->SpeedUpTime * 1.5f;
			break;

		case ItemsCode::AIMMISSILE:
		case ItemsCode::AIMMISSILE2:
		{
			float x = sin(glm::radians(m_Yrot)) * m_radius * 1.1f;
			float z = cos(glm::radians(m_Yrot)) * m_radius * 1.1f;

			glm::vec3 pos = glm::vec3(x, 0, z) + Position();

			m_pRaceScene->SpawnMissile(pos, m_Yrot, true);


			this->PlayAudioCheck(PLAYER, CRI_SOUND_BARRIERACTIVE);
		}
		break;
		
		case ItemsCode::WINDWATEER:
		case ItemsCode::WATERWIND:
		{
			for (int i = 0; i < 8; i++) {

				float x = sin(glm::radians(m_Yrot - 6.0f + i * 45.0f)) * m_radius * 1.1f;
				float z = cos(glm::radians(m_Yrot - 6.0f + i * 45.0f)) * m_radius * 1.1f;

				glm::vec3 pos = glm::vec3(x, 0, z) + Position();

				m_pRaceScene->SpawnMissile(pos, m_Yrot - 6 + i * 45);
			}
		}
		break;

		case ItemsCode::WINDELECTRO:
		case ItemsCode::ELECTROWIND:
		{
			for (int i = 0; i < 3; i++) {
				float x = sin(glm::radians(m_Yrot -180 - 3 + i * 3)) * m_radius * 1.1;
				float z = cos(glm::radians(m_Yrot -180 - 3 + i * 3)) * m_radius * 1.1;

				glm::vec3 pos = glm::vec3(x, 0, z) + Position();

				m_pRaceScene->SpawnMissile(pos, m_Yrot -180 - 3 + i * 3);
			}
		}
		break;

		case ItemsCode::SPREADMISSILE:

			for (int i = 0; i < 5; i++) {
			
				float x = sin(glm::radians(m_Yrot - 6 + i * 3)) * m_radius * 1.1;
				float z = cos(glm::radians(m_Yrot - 6 + i * 3)) * m_radius * 1.1;

				glm::vec3 pos = glm::vec3(x, 0, z) + Position();

				m_pRaceScene->SpawnMissile(pos, m_Yrot - 6 + i * 3);
			}

			this->PlayAudioCheck(SHOOT, CRI_SOUND_ITEMSHOOT);

			break;

		case ItemsCode::SMOKE:
		case ItemsCode::SMOKE2:
		{
			this->m_smokeEmitting = true;

			this->m_smokeTimer = this->m_smokeEmitInterval;

			m_pRaceScene->SpawnSmoke(Position(), Yrot());
		}
			break;

		case ItemsCode::NUKE:
		case ItemsCode::NUKE2:
		{
			m_pRaceScene->SpawnNuke(Position(), Yrot());

			this->PlayAudioCheck(SHOOT, CRI_SOUND_ITEMSHOOT);
		}
		break;

		case ItemsCode::SUPERBARRIER:

			this->CurrentShieldTime = this->ShieldTime * 2;

			this->m_pBarrier->Color(glm::vec4(1, 0, 1, 0.5));
			this->m_pBarrier->ToggleVisibility(true);
			this->activeShield = true;

			this->PlayAudioCheck(PLAYER, CRI_SOUND_BARRIERACTIVE);
			break;

		case ItemsCode::FAKEITEM:
		case ItemsCode::FAKEITEM2:
		{
			float x = sin(glm::radians(m_Yrot)) * m_radius * -1.1;
			float z = cos(glm::radians(m_Yrot)) * m_radius * -1.1;

			glm::vec3 pos = glm::vec3(x, 0, z) + this->Position();

			m_pRaceScene->SpawnFakeItem(pos);
		}
			break;

		case ItemsCode::BOMB:
		{
			float x = sin(glm::radians(m_Yrot)) * m_radius * -1.1;
			float z = cos(glm::radians(m_Yrot)) * m_radius * -1.1;

			glm::vec3 pos = glm::vec3(x, 0, z) + this->Position();

			float angle = m_Yrot + 180.0f;
			if (angle > 360) {
				angle -= 360;
			}

			this->PlayAudioCheck(SHOOT, CRI_SOUND_ITEMPUT);

			m_pRaceScene->SpawnBombs(pos, angle);
		}
			break;

		default:
			break;
		}

		itemId = (int)ItemsCode::NONE;
		m_type = ItemUsageType::NONE;

		itemMixable == false;
	}
}

void CPlayerCharacter::Dead()
{
	if (CurrentShieldTime > 0.0) {
		return;
	}

	if (!m_dead) {
		m_dead = true;
		activeInput = false;
		m_velocity = 0.0;

		m_spawningPoint = this->Position();

		CurrentRespawnTime = RespawnTime;

		m_pRaceScene->SpawnEffectExplode(this->Position());
		this->m_entity->ToggleVisibility(false);

		m_pRaceScene->SpawnEffectRespawn(Position(), RespawnTime);
	}
}

void CPlayerCharacter::InputOptionFunc(float delta)
{
	GameEngine& game = GameEngine::Instance();

	GamePad gamePad = game.GetGamePad(m_playerIndex);

	if (!m_optionPressedCheck) {
		if (gamePad.buttonDown & GamePad::START) {
			m_pRaceScene->TogglePause(m_playerIndex);
		}
		if (gamePad.buttonDown & GamePad::A) {
			m_pRaceScene->ActOptionCommand();
			game.PlayAudio(SEUI, CRI_SOUND_DECISION);
		}
		if (gamePad.buttonDown & GamePad::DPAD_UP) {
			m_pRaceScene->OptionCommand(false);
		}
		else if (gamePad.buttonDown & GamePad::DPAD_DOWN) {
			m_pRaceScene->OptionCommand(true);
		}
	}
	else {
		if (!gamePad.buttons) {
			m_optionPressedCheck = false;
		}
	}
}

glm::vec3 CPlayerCharacter::CollisionCheckAndBounce(glm::vec3 pos, float radius, glm::vec3 movement, glm::vec3 force)
{
	float distance = glm::distance(this->m_entity->Position(), pos);

	if (distance < radius + m_radius) {
		this->currentBounceTime = bounceTime;
		
		this->m_bounceVel = this->Position() - pos;
		
		this->CurrentSpeedUpTime = 0.0;

		m_bounceVel = glm::normalize(m_bounceVel);

		this->m_bounceVel *= 10.0f;
		this->m_velocity = 0;
	}

	return glm::vec3();
}

void CPlayerCharacter::AutoDrive(float delta, std::vector<CPlayerCharacter*> character)
{
	if (m_autoDriveFlag && !m_dead) {

		if (m_goals.size() == 0) {
			return;
		}

		glm::vec3 destination = m_goals[m_nextCheckPoint];

		float radian = atan2(destination.x - this->Position().x, destination.z - this->Position().z);
		float degree = glm::degrees(radian);

		if (degree < 0) {
			degree += 360;
		}

		float bet = 5.0f;

		///アイテム使用のAI.
		for (int i = 0; i < character.size(); i++) {
			if (i == m_playerIndex || character[i]->m_dead)
				continue;

			float posDif = glm::degrees(atan2(character[i]->Position().x - this->Position().x, character[i]->Position().z - this->Position().z));

			if (posDif < 0) {
				posDif += 360;
			}

			float eneChecker = m_Yrot;

			if (posDif < 90 || posDif > 360 - 90) {
				eneChecker += 180;
				if (eneChecker > 360) {
					eneChecker -= 360;
				}
				posDif += 180;
				if (posDif > 360) {
					posDif += 360;
				}
			}

			if (glm::distance(character[i]->Position(), this->Position()) < 300) {

				float frontAttackAngle = 10.0f;

				if (m_type == ItemUsageType::FRONT_ATTACK) {
					if (posDif > eneChecker - frontAttackAngle && posDif < eneChecker + frontAttackAngle) {
						
						m_frontAttackTime -= delta;

						if (m_frontAttackTime <= 0) {
							if (itemMixable) {
								this->MixItem();
							}

							this->UseItem();
						}
					}
				}
				else {
					m_frontAttackTime = m_frontAttackInterval;
				}

				if (m_type == ItemUsageType::BACK_SET) {
					if (glm::distance(character[i]->Position(), this->Position()) < 100) {

						m_backAttackTime -= delta;

						if (m_backAttackTime <= 0) {
							if (itemMixable) {
								this->MixItem();
							}
							this->UseItem();
						}
					}
				}
				else {
					m_backAttackTime = m_backAttackInterval;
				}

				if (m_type == ItemUsageType::PLAYERBUF) {
					m_BuffUseTime -= delta;
					if (m_BuffUseTime <= 0) {
						if (itemMixable) {
							this->MixItem();
						}
						this->UseItem();
					}
				}
			}
		}
		///ここまでアイテム使用のAI.

		std::vector<Item*> items = m_pRaceScene->GetFieldItems();

		bool detectItem = false;
		int index = -1;
		float distance = 0;
		for (int i = 0; i < items.size(); i++) {

			if (items[i]->GetActive()) {

				if (glm::distance(this->Position(), items[i]->Position()) < 100) {

					if (index == -1) {
						distance = glm::distance(this->Position(), items[i]->Position());
						index = i;
					}
					else if (distance > glm::distance(this->Position(), items[i]->Position())) {
						distance = glm::distance(this->Position(), items[i]->Position());
						index = i;
					}
				}
			}
		}

		if (index != -1) {

			float itemPosDif = glm::degrees(atan2(items[index]->Position().x - this->Position().x, items[index]->Position().z - this->Position().z));
			float itemChecker = Yrot();

			if (itemPosDif < 0) {
				itemPosDif += 360;
			}
			if (itemPosDif < 90 || itemPosDif > 360 - 90) {
				itemChecker += 180;
				if (itemChecker > 360) {
					itemChecker -= 360;
				}
				itemPosDif += 180;
				if (itemPosDif > 360) {
					itemPosDif += 360;
				}
			}

			float itemGettingAngle = 10;

			if (itemPosDif < itemChecker + itemGettingAngle && itemPosDif > itemChecker - itemGettingAngle) {

				if (itemPosDif < itemChecker) {
					m_Yrot -= m_turnRate * delta;
				}
				else {
					m_Yrot += m_turnRate * delta;
				}

				detectItem = true;

				m_velocity += m_accel * delta;
			}
		}

		float checker = m_Yrot;

		if (!detectItem) {
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

			if (degree + bet < checker) {
				m_Yrot -= m_turnRate * delta;
			}
			else if (degree - bet > checker)
			{
				m_Yrot += m_turnRate * delta;
			}
			else {
				m_velocity += m_accel * delta;
			}
		}

		m_entity->Rotation(glm::quat(glm::vec3(0, glm::radians(m_Yrot), 0)));
	}
}

/*
カメラ位置の更新.
*/
void CPlayerCharacter::CameraUpdate(float delta)
{
	GameEngine& game = GameEngine::Instance();

	if (m_cameraActiveFlag) {

		if (FinishedRace) {
			cameraGoalTurn += delta * 30;
		}

		glm::vec3 pos = game.CalcPosition(Position(), Yrot() + CameraYDif + cameraGoalTurn,/* 89*/15, /*200*/20 + CurrentSpeedUpTime);
		game.Camera({ { pos },{ this->Position() },{ 0, 1, 0 } }, m_playerIndex);
	}
}