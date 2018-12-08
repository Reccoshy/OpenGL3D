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
���C���Q�[�����.
*/
class RaceScene
{
public:
	explicit RaceScene(char score[], char stageFile[], char checkPointFile[], char itemFile[], char texfileName[], int playerNum, int lap);
	~RaceScene();
	void operator()(float delta);

	//�A�C�e��
	void SpawnElectroTrap(glm::vec3 pos);
	void SpawnMissile(glm::vec3 pos, float rot, bool aiming = false);
	void SpawnNuke(glm::vec3 pos, float rot);
	void SpawnBombs(glm::vec3 pos, float rot);
	void SpawnExplode(glm::vec3 pos);
	void SpawnSmoke(glm::vec3 pos, float rot);
	void SpawnFakeItem(glm::vec3 pos);
	void SpawnEffectExplode(glm::vec3 pos);

	//�G�t�F�N�g
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
	//������.
	bool m_isInitialized = false;

	//���[�X�̌o�ߎ���.
	float Time = 0.0f;
	//���[�X�J�n�O�̃J�E���^�[.
	float CountDown = 6.0f;

	//�v���C���[.
	std::vector<CPlayerCharacter*> m_pPlayerCharacters;
	//�`�F�b�N�|�C���g.
	std::vector<Goal*> m_pGoals;
	
	//��Q��.
	std::vector<Obstacle*> m_pObstacles;


	//==========================
	// �A�C�e����.
	//==========================

	//�A�C�e��
	std::vector<Item*> m_pItems;

	//�d�C�g���b�v.
	std::vector<ElectroTrap*> m_pElectroTraps;
	//�~�T�C��.
	std::vector<Missile*> m_pMissiles;
	//�j���e.
	std::vector<CNuke*> m_pNukes;
	//��.
	std::vector<Smoke*> m_pSmokes;
	//������e.
	std::vector<Bomb*> m_pBombs;
	//�U�A�C�e��.
	std::vector<FakeItem*> m_pFakeItems;
	
	//�A�C�e������o�邠���蔻��t������.
	std::vector<Explosion*> m_pExplodes;

	//�����G���e�B�e�B.
	std::vector<EffectDestroy*> m_pEffectExplodes;

	//==========================
	//�G�t�F�N�g.
	//==========================

	//�A�N�Z�����̃G�t�F�N�g.
	std::vector<AccelDust*> m_pAccelDusts;
	//�L���L���G�t�F�N�g.
	std::vector<CsparkleEffect*> m_pSparkleEffects;
	//�X�s�[�h�㏸���G�t�F�N�g.
	std::vector<SpeedUpEffect*> m_pSpeedUpEffects;
	//���S�������̃G�t�F�N�g.
	std::vector<RespawnEffect*> m_pRespawnEffects;

	//==============================
	//�I�v�V������ʂ̕ϐ�.
	//==============================

	//�|�[�Y�t���O.
	bool m_pausing = false;
	//�|�[�Y�����v���C���[�̔ԍ�.
	int PauseIndex = 0;
	//�I�v�V������ʂ̑I����.
	CommandSelect m_optionCommand;

	//=====================================
	//�X�e�[�W�f�[�^�ǂݍ��ݗp.
	//=====================================

	//�X�e�[�W�S�̂̕Ǔǂݍ��ݗp�e�L�X�g.
	char stageFileName[100];
	//�`�F�b�N�|�C���g�̓ǂݍ��ݗp�e�L�X�g.
	char checkPointFileName[100];
	//�A�C�e���t�@�C��
	char itemFileName[100];
	//�n�ʂ̃e�N�X�`���ǂݍ��ݗp�e�L�X�g.
	char textureFileName[100];
	//����ō��L�^�ۑ��e�L�X�g.
	char bestTimeFileName[100];


	//�v���C���[�̐�.
	int PlayerNum = 4;

	//����.
	int lapNum = 3;

	//================================
	//���U���g�p�̕ϐ�.
	//================================

	//�S�v���C���[�S�[���t���O.
	bool m_isInResult = false;
	//�����L���O��ʕ\���t���O.
	bool m_showRanking = false;
	//�����L���O�\���܂ł̃C���^�[�o��.
	float m_resultWaitTime = 5.0f;
	//���ʔ��\�̉��UI�S�̂̈ړ��ʒu.
	float m_resultMover = 0.0f;


	//���͗L���t���O.
	bool m_activeInput = true;

	//=========================
	// UI�ޕϐ�.
	//=========================

	//�Ó]����.
	BlackFilter blackFilter;
	//���ʂ̐F.
	glm::vec4 rankColor[5] = {
		glm::vec4(0, 0, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(0.8, 0.8, 1, 1),
		glm::vec4(1, 0, 0.2, 1),
		glm::vec4(0, 0.5, 1, 1),
	};
	//�~�j�}�b�v
	MiniMap m_minimap;
	
private:

	void Init();

	void EndFunc();

	//===========================
	//�Q�[���󋵂ɂ��ꂼ��̍X�V����.
	//===========================
	void UpdateInGame(float delta);
	void UpdateGameStart(float delta);
	void UpdatePause(float delta);
	void UpdateResult(float delta);

	//=================================
	//�X�e�[�W�����p�֐�.
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