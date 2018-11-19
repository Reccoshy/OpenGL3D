#pragma once
#include "../../Entity.h"
#include "../Utilities/CommandSelect/CommandSelect.h"
#include "../Entity/Player/Player.h"
#include "../Entity/Goal/Goal.h"
#include "../Entity/Item/Item.h"
#include "../Entity/Item/ElectroTrap/ElectroTrap.h"
#include "../Entity/Item/Missile/Missile.h"
#include "../Entity/Obstacle/Obstacle.h"
#include "../Entity/Item/Nuke/Nuke.h"
#include "../Entity/Item/Smoke/Smoke.h"
#include "../Entity/Item/Bomb/Bomb.h"
#include "../Entity/Item/FakeItem/FakeItem.h"
#include "../Entity/Item/Explode/Explode.h"
#include "../Entity/EffectDestroy/EffectDestroy.h"
#include "../Utilities/MiniMap/MiniMap.h"
#include "../Effects/AccelDust/AccelDust.h"
#include "../Effects/ItemGetEffect/ItemGetEffect.h"
#include "../Effects/SpeedUpEffect/SpeedUpEffect.h"
#include "../Effects/PlayerRespawn/RespawnEffect.h"
#include "../Utilities/UITexture/BlackFilter/BlackFilter.h"
#include <vector>


/// エンティティの衝突グループID.
enum EntityGroupId
{
	EntityGroupId_Background,
	EntityGroupId_Player,
	EntityGroupId_PlayerShot,
	EntityGroupId_Enemy,
	EntityGroupId_EnemyShot,
	EntityGroupId_Others
};

/// 音声プレイヤーのID.
enum AudioPlayerId
{
	AudioPlayerId_Shot, ///< 自機のショット音.
	AudioPlayerId_Bomb, ///< 爆発音.
	AudioPlayerId_BGM, ///<　BGM.
	AudioPlayerId_Max, ///< 同時発音可能な数.

	AudioPlayerId_UI = AudioPlayerId_Shot, ///<　ユーザーインターフェイス操作音.
};

enum class FromRaceSceneToNextScene {
	NONE = -1,
	ToTitleScene = 0,
	ToStageSelectScene = 1,
	Retry = 2,
	ShowResultTime = 3,
};

/*
メインゲーム画面.
*/
class RaceScene
{
public:
	explicit RaceScene(char score[], char stageFile[], char checkPointFile[], char itemFile[], char texfileName[], int playerNum, int lap);
	~RaceScene();
	void operator()(float delta);

	void SpawnElectroTrap(glm::vec3 pos);
	void SpawnMissile(glm::vec3 pos, float X_rot, bool aiming = false);
	void SpawnNuke(glm::vec3 pos, float y_rot);
	void SpawnBombs(glm::vec3 pos, float y_rot);
	void SpawnExplode(glm::vec3 pos);
	void SpawnSmoke(glm::vec3 pos, float y_rot);
	void SpawnFakeItem(glm::vec3 pos);
	void SpawnEffectExplode(glm::vec3 pos);

	void SpawnEffectAccelDust(glm::vec3 pos);
	void SpawnEffectItemGetSparkle(glm::vec3 pos, glm::vec4 color);
	void SpawnEffectSpeedUp(glm::vec3 pos, float yRot, float speed);
	void SpawnEffectRespawn(glm::vec3 pos, float lifeTime);

	bool BestTimeCheck(float Time);
	
	void OptionCommand(bool isAdd);

	void ActOptionCommand();

	void PlayAudioCheck(glm::vec3 pos, float distance, int soundType, int audioId);

	std::vector<Item*> GetFieldItems();

	std::vector<Obstacle*> getObstacles();

	void TogglePause(int playerindex);

	int GetLapNum() { return lapNum; }

private:
	bool isInitialized = false;

	std::vector<CPlayerCharacter*> m_pPlayerCharacters;

	std::vector<Goal*> m_pGoals;
	std::vector<Item*> m_pItems;

	std::vector<Obstacle*> m_pObstacles;

	std::vector<ElectroTrap*> m_pElectroTraps;
	std::vector<Missile*> m_pMissiles;
	std::vector<CNuke*> m_pNukes;
	std::vector<Smoke*> m_pSmokes;
	std::vector<Bomb*> m_pBombs;
	std::vector<FakeItem*> m_pFakeItems;
	std::vector<Explosion*> m_pExplodes;

	std::vector<EffectDestroy*> m_pEffectExplodes;

	std::vector<AccelDust*> m_pAccelDusts;
	std::vector<CitemGetEffect*> m_pItemGetEffects;
	std::vector<SpeedUpEffect*> m_pSpeedUpEffects;
	std::vector<RespawnEffect*> m_pRespawnEffects;

	CommandSelect resultCommand;

	char stageFileName[100];
	char checkPointFileName[100];
	char itemFileName[100];
	char textureFileName[100];
	char bestTimeFileName[100];

	bool testing = false;
	bool m_pausing = false;
	bool m_isInResult = false;

	int PauseIndex = 0;

	float CountDown = 6.0f;
	float Time = 0.0f;

	float m_resultMover = 0.0f;

	int rank = 1;
	int PlayerNum = 4;
	int AirCraftNum = 4;

	int lapNum = 3;

	float m_resultWaitTime = 5.0f;
	bool m_showRanking = false;

	bool m_activeInput = true;

	//bool m_sceneChanging = false;

	//float m_UIAlpha = 1.0f;

	//float m_waitTime = 1.0f;

	BlackFilter blackFilter;

	glm::vec4 rankColor[5] = {
		glm::vec4(0, 0, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(0.8, 0.8, 1, 1),
		glm::vec4(1, 0, 0.2, 1),
		glm::vec4(0, 0.5, 1, 1),
	};

	FromRaceSceneToNextScene m_nextScene = FromRaceSceneToNextScene::NONE;

	MiniMap m_minimap;

private:
	void Init();

	void EndFunc();

	void UpdateInGame(float delta);
	void UpdateGameStart(float delta);
	void UpdatePause(float delta);
	void UpdateResult(float delta);

	bool LoadStageFromFile(char* const filename);
	bool LoadCheckPointFromFile(char* const filename);
	bool LoadItemsFromFile(char* const filename);

	void WallSetPointToPoint(glm::vec3 from, glm::vec3 to, float space);
	
	bool LoadStageTextures(char* const filename);

	void PlayerUpdate(float delta);

	void ItemUpdate(float delta);

	void UpdateEffects(float delta);

	void DeleteCheck();

	void CheckPoint();
	void CollisionChecks();
	void RankingCheck();

	void CheckAllPlayerFinished();

	void SaveBestLap();

	void SceneChanger(float delta);

	void ShowUI(float delta);
	void ShowFilter();
	void ItemUI(glm::vec2 pos, glm::vec2 scale, ItemsCode id, int index);

	void DrawMapUi(int playerIndex);

	void RankingUI();
	void OptionUI();

	void ShowResult(float delta);
};