#pragma once
#include "../../../Entity.h"
#include "../../Utilities/CommandSelect/CommandSelect.h"
#include "../../Entity/Player/Player.h"
#include "../../Entity/Goal/Goal.h"
#include "../../Entity/Item/Item/Item.h"
#include "../../Entity/Item/ElectroTrap/ElectroTrap.h"
#include "../../Entity/Item/Missile/Missile.h"
#include "../../Entity/Obstacle/Obstacle.h"
#include "../../Entity/Item/Nuke/Nuke.h"
#include "../../Entity/Item/Smoke/Smoke.h"
#include "../../Entity/Item/Bomb/Bomb.h"
#include "../../Entity/Item/FakeItem/FakeItem.h"
#include "../../Entity/Item/Explode/Explode.h"
#include "../../Entity/EffectDestroy/EffectDestroy.h"
#include "../../Utilities/MiniMap/MiniMap.h"
#include "../../Effects/AccelDust/AccelDust.h"
#include "../../Effects/ItemGetEffect/ItemGetEffect.h"
#include "../../Effects/SpeedUpEffect/SpeedUpEffect.h"
#include "../../Effects/PlayerRespawn/RespawnEffect.h"
#include "../../Utilities/UITexture/BlackFilter/BlackFilter.h"
#include <vector>

/*
メインゲーム画面.
*/
class RaceScene
{
public:
	explicit RaceScene(char score[], char stageFile[], char checkPointFile[], char itemFile[], char texfileName[], int playerNum, int lap);
	~RaceScene();
	void operator()(float delta);

	//アイテム
	void SpawnElectroTrap(glm::vec3 pos);
	void SpawnMissile(glm::vec3 pos, float rot, bool aiming = false);
	void SpawnNuke(glm::vec3 pos, float rot);
	void SpawnBombs(glm::vec3 pos, float rot);
	void SpawnExplode(glm::vec3 pos);
	void SpawnSmoke(glm::vec3 pos, float rot);
	void SpawnFakeItem(glm::vec3 pos);
	void SpawnEffectExplode(glm::vec3 pos);

	//エフェクト
	void SpawnEffectAccelDust(glm::vec3 pos);
	void SpawnEffectSparkle(glm::vec3 pos, glm::vec4 color, int emitNum);
	void SpawnEffectSpeedUp(glm::vec3 pos, float rot, float speed);
	void SpawnEffectRespawn(glm::vec3 pos, float lifeTime);


	bool BestTimeCheck(float Time);


	void TogglePause(int playerindex);

	void OptionCommand(bool isAdd);
	
	void ActOptionCommand();

	void PlayAudioCheck(glm::vec3 pos, float distance, int soundType, int audioId);

	std::vector<Item*> GetFieldItems();

	int GetLapNum() { return lapNum; }

	bool IsFinishedRace() { return m_isInResult; }

private:
	//初期化.
	bool m_isInitialized = false;

	//レースの経過時間.
	float Time = 0.0f;
	//レース開始前のカウンター.
	float CountDown = 6.0f;

	//プレイヤー.
	std::vector<CPlayerCharacter*> m_pPlayerCharacters;
	//チェックポイント.
	std::vector<Goal*> m_pGoals;
	
	//障害物.
	std::vector<Obstacle*> m_pObstacles;


	//==========================
	// アイテム類.
	//==========================

	//アイテム
	std::vector<Item*> m_pItems;

	//電気トラップ.
	std::vector<ElectroTrap*> m_pElectroTraps;
	//ミサイル.
	std::vector<Missile*> m_pMissiles;
	//核爆弾.
	std::vector<CNuke*> m_pNukes;
	//煙.
	std::vector<Smoke*> m_pSmokes;
	//後方爆弾.
	std::vector<Bomb*> m_pBombs;
	//偽アイテム.
	std::vector<FakeItem*> m_pFakeItems;
	
	//アイテムから出るあたり判定付き爆風.
	std::vector<Explosion*> m_pExplodes;

	//爆発エンティティ.
	std::vector<EffectDestroy*> m_pEffectExplodes;

	//==========================
	//エフェクト.
	//==========================

	//アクセル中のエフェクト.
	std::vector<AccelDust*> m_pAccelDusts;
	//キラキラエフェクト.
	std::vector<CsparkleEffect*> m_pSparkleEffects;
	//スピード上昇時エフェクト.
	std::vector<SpeedUpEffect*> m_pSpeedUpEffects;
	//死亡復活時のエフェクト.
	std::vector<RespawnEffect*> m_pRespawnEffects;

	//==============================
	//オプション画面の変数.
	//==============================

	//ポーズフラグ.
	bool m_pausing = false;
	//ポーズしたプレイヤーの番号.
	int PauseIndex = 0;
	//オプション画面の選択肢.
	CommandSelect m_optionCommand;

	//=====================================
	//ステージデータ読み込み用.
	//=====================================

	//ステージ全体の壁読み込み用テキスト.
	char stageFileName[100];
	//チェックポイントの読み込み用テキスト.
	char checkPointFileName[100];
	//アイテムファイル
	char itemFileName[100];
	//地面のテクスチャ読み込み用テキスト.
	char textureFileName[100];
	//周回最高記録保存テキスト.
	char bestTimeFileName[100];


	//プレイヤーの数.
	int PlayerNum = 4;

	//周回数.
	int lapNum = 3;

	//================================
	//リザルト用の変数.
	//================================

	//全プレイヤーゴールフラグ.
	bool m_isInResult = false;
	//ランキング画面表示フラグ.
	bool m_showRanking = false;
	//ランキング表示までのインターバル.
	float m_resultWaitTime = 5.0f;
	//結果発表の画面UI全体の移動位置.
	float m_resultMover = 0.0f;


	//入力有効フラグ.
	bool m_activeInput = true;

	//=========================
	// UI類変数.
	//=========================

	//暗転処理.
	BlackFilter blackFilter;
	//順位の色.
	glm::vec4 rankColor[5] = {
		glm::vec4(0, 0, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(0.8, 0.8, 1, 1),
		glm::vec4(1, 0, 0.2, 1),
		glm::vec4(0, 0.5, 1, 1),
	};
	//ミニマップ
	MiniMap m_minimap;
	
private:

	void Init();

	void EndFunc();

	//===========================
	//ゲーム状況にそれぞれの更新処理.
	//===========================
	void UpdateInGame(float delta);
	void UpdateGameStart(float delta);
	void UpdatePause(float delta);
	void UpdateResult(float delta);

	//=================================
	//ステージ生成用関数.
	//=================================
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

	void CalcFinishTime();
	void RankingSortFromTime();

	void SaveBestLap();

	void SceneChanger(float delta);

	void ShowUI(float delta);
	void ItemUI(glm::vec2 pos, glm::vec2 scale, ItemsCode id, int index);

	void DrawMapUi(int playerIndex);

	void GoalInformUi();
	void OptionUI();

	void RankingUI(float delta);
};