#pragma once
#pragma warning(disable:4996)

#include "../../../Entity.h"
#include "../Item/Item/Item.h"
#include "../Shadow/Shadow.h"

class RaceScene;
class Goal;

//プレイヤークラス.
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
	//プレイヤーの車体のモデルのポインタ.
	Entity::Entity* m_entity;
	//プレイヤーのバリアのモデルのポインタ.
	Entity::Entity* m_pBarrier;

	//レースシーン中のモデルのポインタ.
	RaceScene* m_pRaceScene = nullptr;
	//影のモデル.
	Shadow m_Shadow;
	//プレイヤーのインデックス.
	int m_playerIndex = -1;
	//レースが終了していかのフラグ.
	bool FinishedRace = false;
	//ゴールのカメラの回転度数.
	float cameraGoalTurn = 0.0f;
	
	//あたり判定用の半径.
	float radius = 0.0f;
	
	//カメラのY軸での回転度数.
	float CameraYDif = 180.0f;
	
	//プレイヤーが向いている方向度数.
	float m_Yrot = 0.0f;
	//プレイヤーの実際の速さ.
	float m_acctualSpeed = 0.0f;
	//プレイヤーのアクセルの速さ.
	float m_velocity = 0.0f;

	glm::vec3 m_actualVel;
	
	//==================
	//反射処理
	//==================

	//ぶつかった時の反動のベクトル.
	glm::vec3 m_bounceVel = glm::vec3(0);
	//反動が発生している経過時間.
	float currentBounceTime = 0.0f;
	//反動の処理が続く時間.
	float bounceTime = 3.0f;

	//==================
	//機体のステータス
	//==================

	//アクセルでの1秒まででかかる時間.
	double m_accel = 20.0;
	//アクセルで出せる最高速度.
	double MaxSpeed = 60.0;
	//ブレーキでの速度低下度.
	double m_break = 60.0;
	//旋回速度.
	double m_turnRate = 180.0;

	//アクセルを押していない時間
	float StopTime = 0.0f;
	//後方へ移動している.
	bool backward = false;
	//後ろに下がれる最高速度.
	double MaxBackWardSpeed = -5.0;

	//=========================
	//ゴールへのルート・タイム
	//=========================

	//プレイヤーのゴールの位置のベクター.
	std::vector<glm::vec3> m_goals;

	//現在のチェックポイント.
	int m_checkPointIndex = 0;
	//次のチェックポイント.
	int m_nextCheckPoint = 1;
	//チェックポイントの最大数.
	int MaxCheckPointIndex = -1;
	
	//現在の周回数
	int m_lap = 1;

	//一周にかかった時間のベクター.
	std::vector<float> m_lapTime;

	//終了時間のタイム.
	float m_FinishTime = 0.0f;

	//順位
	int m_rank = 0;
	//順位を決定させる点数.
	float m_score = 0.0f;

	//==========================
	//オプション画面
	//==========================

	//入力有効フラグ.
	bool activeInput = true;

	//カメラ有効フラグ.
	bool m_cameraActiveFlag = false;
	//オプション画面を開いた後の2重押しチェック.
	bool m_optionPressedCheck = false;

	//===========================
	//アイテム
	//===========================

	//アイテムによるスピードアップできる時間
	double SpeedUpTime = 2.0;
	//スピードアップ使用からの経過時間.
	double CurrentSpeedUpTime = 0.0;

	//バリアの有効フラグ.
	bool activeShield = false;
	//バリアの持続時間.
	double ShieldTime = 5.0;
	//バリアの使用中の経過時間.
	double CurrentShieldTime = 0.0f;

	//アイテムを合成できる.
	bool itemMixReady = false;
	//アイテムが合成可能かどうかのフラグ.
	bool itemMixable = false;

	//煙を噴出しているかどうかのフラグ.
	bool m_smokeEmitting = false;
	//煙を噴出した数.
	int m_smokeEmitCount = 0;
	//煙を噴出する最大数.
	int m_smokeEmitingEndCount = 3;
	//煙にかかる時間,
	float m_smokeTimer = 0.0f;
	//煙を噴出するインターバル.
	float m_smokeEmitInterval = 0.2f;

	//================
	//AI
	//================

	//自動操縦フラグ.
	bool m_autoDriveFlag = false;

	//正面に向かって攻撃するアイテムの発射までの経過時間.
	float m_frontAttackTime = 2.0f;
	//正面に向かって攻撃するアイテムに時間.
	float m_frontAttackInterval = 2.0f;

	//後方に向かって攻撃するアイテムの発射までの経過時間.
	float m_backAttackTime = 2.0f;
	//後方に向かって攻撃するアイテムに時間.
	float m_backAttackInterval = 2.0f;

	//バフ系アイテムを使用するまでの時間.
	float m_BuffUseTime;
	//ランダムでバフを使用するまでの最大時間.
	int BuffMaxTime = 20;
	//ランダムでバフを使用するまでの最低時間.
	int BuffMinTime = 5;

	//所持アイテムID.
	int itemId = -1;
	//在庫内のアイテムID.
	int m_stockItems = -1;
	//アイテムを使用方法のタイプ.
	ItemUsageType m_type = ItemUsageType::NONE;

	//==============
	//死亡処理
	//==============
	
	//死亡フラグ.
	bool m_dead = false;
	
	//死亡した時の復活地点.
	glm::vec3 m_spawningPoint = glm::vec3(0);

	//復活にかかる時間.
	double RespawnTime = 2.0;
	//死亡してからの経過時間.
	double CurrentRespawnTime = 2.0f;
};