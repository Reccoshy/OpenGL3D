#pragma once
#pragma warning(disable:4996)

#include "../../../Entity.h"
#include "../Item/Item/Item.h"
#include "../Shadow/Shadow.h"

class RaceScene;
class Goal;

//�v���C���[�N���X.
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
	//�v���C���[�̎ԑ̂̃��f���̃|�C���^.
	Entity::Entity* m_entity;
	//�v���C���[�̃o���A�̃��f���̃|�C���^.
	Entity::Entity* m_pBarrier;

	//���[�X�V�[�����̃��f���̃|�C���^.
	RaceScene* m_pRaceScene = nullptr;
	//�e�̃��f��.
	Shadow m_Shadow;
	//�v���C���[�̃C���f�b�N�X.
	int m_playerIndex = -1;
	//���[�X���I�����Ă����̃t���O.
	bool FinishedRace = false;
	//�S�[���̃J�����̉�]�x��.
	float cameraGoalTurn = 0.0f;
	
	//�����蔻��p�̔��a.
	float radius = 0.0f;
	
	//�J������Y���ł̉�]�x��.
	float CameraYDif = 180.0f;
	
	//�v���C���[�������Ă�������x��.
	float m_Yrot = 0.0f;
	//�v���C���[�̎��ۂ̑���.
	float m_acctualSpeed = 0.0f;
	//�v���C���[�̃A�N�Z���̑���.
	float m_velocity = 0.0f;

	glm::vec3 m_actualVel;
	
	//==================
	//���ˏ���
	//==================

	//�Ԃ��������̔����̃x�N�g��.
	glm::vec3 m_bounceVel = glm::vec3(0);
	//�������������Ă���o�ߎ���.
	float currentBounceTime = 0.0f;
	//�����̏�������������.
	float bounceTime = 3.0f;

	//==================
	//�@�̂̃X�e�[�^�X
	//==================

	//�A�N�Z���ł�1�b�܂łł����鎞��.
	double m_accel = 20.0;
	//�A�N�Z���ŏo����ō����x.
	double MaxSpeed = 60.0;
	//�u���[�L�ł̑��x�ቺ�x.
	double m_break = 60.0;
	//���񑬓x.
	double m_turnRate = 180.0;

	//�A�N�Z���������Ă��Ȃ�����
	float StopTime = 0.0f;
	//����ֈړ����Ă���.
	bool backward = false;
	//���ɉ������ō����x.
	double MaxBackWardSpeed = -5.0;

	//=========================
	//�S�[���ւ̃��[�g�E�^�C��
	//=========================

	//�v���C���[�̃S�[���̈ʒu�̃x�N�^�[.
	std::vector<glm::vec3> m_goals;

	//���݂̃`�F�b�N�|�C���g.
	int m_checkPointIndex = 0;
	//���̃`�F�b�N�|�C���g.
	int m_nextCheckPoint = 1;
	//�`�F�b�N�|�C���g�̍ő吔.
	int MaxCheckPointIndex = -1;
	
	//���݂̎���
	int m_lap = 1;

	//����ɂ����������Ԃ̃x�N�^�[.
	std::vector<float> m_lapTime;

	//�I�����Ԃ̃^�C��.
	float m_FinishTime = 0.0f;

	//����
	int m_rank = 0;
	//���ʂ����肳����_��.
	float m_score = 0.0f;

	//==========================
	//�I�v�V�������
	//==========================

	//���͗L���t���O.
	bool activeInput = true;

	//�J�����L���t���O.
	bool m_cameraActiveFlag = false;
	//�I�v�V������ʂ��J�������2�d�����`�F�b�N.
	bool m_optionPressedCheck = false;

	//===========================
	//�A�C�e��
	//===========================

	//�A�C�e���ɂ��X�s�[�h�A�b�v�ł��鎞��
	double SpeedUpTime = 2.0;
	//�X�s�[�h�A�b�v�g�p����̌o�ߎ���.
	double CurrentSpeedUpTime = 0.0;

	//�o���A�̗L���t���O.
	bool activeShield = false;
	//�o���A�̎�������.
	double ShieldTime = 5.0;
	//�o���A�̎g�p���̌o�ߎ���.
	double CurrentShieldTime = 0.0f;

	//�A�C�e���������ł���.
	bool itemMixReady = false;
	//�A�C�e���������\���ǂ����̃t���O.
	bool itemMixable = false;

	//���𕬏o���Ă��邩�ǂ����̃t���O.
	bool m_smokeEmitting = false;
	//���𕬏o������.
	int m_smokeEmitCount = 0;
	//���𕬏o����ő吔.
	int m_smokeEmitingEndCount = 3;
	//���ɂ����鎞��,
	float m_smokeTimer = 0.0f;
	//���𕬏o����C���^�[�o��.
	float m_smokeEmitInterval = 0.2f;

	//================
	//AI
	//================

	//�������c�t���O.
	bool m_autoDriveFlag = false;

	//���ʂɌ������čU������A�C�e���̔��˂܂ł̌o�ߎ���.
	float m_frontAttackTime = 2.0f;
	//���ʂɌ������čU������A�C�e���Ɏ���.
	float m_frontAttackInterval = 2.0f;

	//����Ɍ������čU������A�C�e���̔��˂܂ł̌o�ߎ���.
	float m_backAttackTime = 2.0f;
	//����Ɍ������čU������A�C�e���Ɏ���.
	float m_backAttackInterval = 2.0f;

	//�o�t�n�A�C�e�����g�p����܂ł̎���.
	float m_BuffUseTime;
	//�����_���Ńo�t���g�p����܂ł̍ő厞��.
	int BuffMaxTime = 20;
	//�����_���Ńo�t���g�p����܂ł̍Œ᎞��.
	int BuffMinTime = 5;

	//�����A�C�e��ID.
	int itemId = -1;
	//�݌ɓ��̃A�C�e��ID.
	int m_stockItems = -1;
	//�A�C�e�����g�p���@�̃^�C�v.
	ItemUsageType m_type = ItemUsageType::NONE;

	//==============
	//���S����
	//==============
	
	//���S�t���O.
	bool m_dead = false;
	
	//���S�������̕����n�_.
	glm::vec3 m_spawningPoint = glm::vec3(0);

	//�����ɂ����鎞��.
	double RespawnTime = 2.0;
	//���S���Ă���̌o�ߎ���.
	double CurrentRespawnTime = 2.0f;
};