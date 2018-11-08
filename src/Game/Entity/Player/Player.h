#pragma once
#pragma warning(disable:4996)

#include "../../../Entity.h"
#include "../Item/Item.h"
#include "../Shadow/Shadow.h"

class RaceScene;
class Goal;

class CPlayerCharacter
{
public:

	bool Init(glm::vec3 pos, int goalNum, float radius, int playerIndex, RaceScene* pScene, std::vector<glm::vec3> g, bool isPlayer);
	bool Update(float delta, float Time);

	float Yrot() { return m_Yrot; }

	float Velocity() { return m_acctualSpeed; }

	glm::vec3 Position() { return m_entity->Position(); }

	void PassCheckPoint(int passedIndex, float time);
	void PassGoal(float Time);

	void SetPlayerRank(int num);

	void ObtainItem(int id);
	int GetItemId() { return itemId; }
	int GetStockItemId() { return m_stockItems; }
	float Radius() { return radius; }

	void UseItem();

	void Dead();

	void DestroyEntity() {
		this->m_entity->Destroy();
		this->m_Shadow.Destroy();
	}

	void inputOptionFunc(float delta);

	bool GetFinishedRace() { return FinishedRace; }

	glm::vec3 CollisionCheckAndBounce(glm::vec3 pos, float radius, glm::vec3 movement = glm::vec3(0), glm::vec3 force = glm::vec3(0));

	void AutoDrive(float delta, std::vector<CPlayerCharacter*> players);

	void CameraUpdate(float delta);

	int const GetCheckPointIndex() { return m_checkPointIndex; }
	int const GetNextCheckPointIndex() { return m_nextCheckPoint; }
	int const GetLap() { return m_lap; }
	int const GetRank() { return m_rank;}

	void SetScore(float score) { m_score = score; }
	float const GetScore() { return m_score; }
	float const GetFinishTime() { return m_FinishTime; }

	std::vector<float> const GetPlayerLapTime() { return m_lapTime; }

	bool const IsAutoDrive() { return m_autoDriveFlag; }
	bool const IsDead() { return m_dead; }

private:

	int m_checkPointIndex = 0;
	int m_nextCheckPoint = 1;
	int MaxCheckPointIndex = -1;

	int m_lap = 1;

	float m_score = 0.0f;
	float m_FinishTime = 0.0f;

	bool activeInput = true;
	bool m_autoDriveFlag = false;
	bool m_cameraActiveFlag = false;

	bool m_optionPressedCheck = false;

	glm::vec3 m_spawningPoint = glm::vec3(0);

	std::vector<float> m_lapTime;

	bool m_dead = false;

	int m_rank = 0;

private:
	void inputFunc(float delta);

	bool loadRideInfo(const char* filename);

	void MoveFunc(float delta);

	void MixItem();
	void SwapItemStock();

	void SpawnSmokes(float delta);
	void ItemStateUpdate(float delta);

	void RespawnFunc(float delta);

	void PlayAudioCheck(int audioType, int audioId);

private:
	
	Entity::Entity* m_entity;

	Entity::Entity* m_pBarrier;

	std::vector<glm::vec3> m_goals;

	RaceScene* m_pRaceScene = nullptr;

	Shadow m_Shadow;

	int m_playerIndex = -1;

	bool FinishedRace = false;

	float cameraGoalTurn = 0.0f;

	float radius = 0.0f;
	float CameraYDif = 180.0f;

	float m_Yrot = 0.0f;
	float m_acctualSpeed = 0.0f;
	float m_velocity = 0.0f;
	glm::vec3 m_actualVel = glm::vec3(0);
	glm::vec3 m_bounceVel = glm::vec3(0);

	float currentBounceTime = 0.0f;
	float bounceTime = 3.0f;

	double m_accel = 20.0;
	double MaxSpeed = 60.0;
	double m_break = 60.0;
	double m_turnRate = 180.0;

	float StopTime = 0.0f;
	bool backward = false;
	double MaxBackWardSpeed = -5.0;

	double SpeedUpTime = 2.0;
	double CurrentSpeedUpTime = 0.0;

	bool activeShield = false;
	double ShieldTime = 5.0;
	double CurrentShieldTime = 0.0f;

	double RespawnTime = 2.0;
	double CurrentRespawnTime = 2.0f;

	bool itemMixReady = false;
	bool itemMixable = false;

	bool m_smokeEmitting = false;
	int m_smokeEmitCount = 0;
	int m_smokeEmitingEndCount = 3;
	float m_smokeTimer = 0.0f;
	float m_smokeEmitInterval = 0.2f;

	float m_frontAttackTime;
	float m_frontAttackInterval = 2.0f;

	float m_backAttackTime;
	float m_backAttackInterval = 2.0f;

	float m_BuffUseTime;
	int BuffMaxTime = 20;
	int BuffMinTime = 5;

	int itemId = -1;
	ItemUsageType m_type = ItemUsageType::NONE;

	int m_stockItems = -1;
};