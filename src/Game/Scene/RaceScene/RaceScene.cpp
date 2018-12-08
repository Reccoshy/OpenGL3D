/*
@file RaceScene.cpp
*/

#include "RaceScene.h"
#include "../../../GameEngine.h"
#include "../../../res/Sound/Sound.h"
#include "../TitleScene/TitleScene.h"
#include "../StageSelectScene/StageSelectScene.h"
#include "../../Entity/Player/Player.h"
#include "../../Entity/Item/Item/Item.h"
#include <algorithm>
#include <iostream>
#include <fstream>

/*
���[�X�V�[���R���X�g���N�^.(�X�e�[�W���ǂݍ���).

@param	scoreFile		����ō��^�C�����e�L�X�g.
@param	stageFile		�X�e�[�W�̕Ǐ��e�L�X�g.
@param	checkPointFile	�`�F�b�N�|�C���g���e�L�X�g.
@param	itemFile		�A�C�e���z�u���e�L�X�g.
@param	texFileName		�e�N�X�`�����e�L�X�g.
@param	playerNum		�Q���v���C���[��.
@param	lap				����.
*/
RaceScene::RaceScene(char scoreFile[], char stageFile[], char checkPointFile[], char itemFile[], char texfileName[], int playerNum, int lap)
{
	snprintf(bestTimeFileName, 100, "%s", scoreFile);
	snprintf(stageFileName,100, "%s", stageFile);
	snprintf(checkPointFileName, 100, "%s", checkPointFile);
	snprintf(itemFileName, 100, "%s", itemFile);
	snprintf(textureFileName, 100, "%s", texfileName);

	this->PlayerNum = playerNum + 1;
	this->lapNum = lap + 1;
}

RaceScene::~RaceScene()
{
	this->EndFunc();
}

void RaceScene::operator()(float delta)
{
	this->DeleteCheck();

	if (!m_isInitialized) {
		this->Init();
	}

	this->CountDown -= delta;

	if (m_isInResult) {
		this->UpdateResult(delta);
	}
	else {
		if (m_pausing)
			this->UpdatePause(delta);
		else if (CountDown > 0.0)
			this->UpdateGameStart(delta);
		else
			this->UpdateInGame(delta);
	}
	this->SceneChanger(delta);
}

/*
�g���b�v��z�u.

@param	pos	�ʒu.
*/
void RaceScene::SpawnElectroTrap(glm::vec3 pos)
{
	this->m_pElectroTraps.push_back(new ElectroTrap);
	this->m_pElectroTraps.back()->init(pos, this);
}

/*
�~�T�C���𔭎˂���.

@param	pos		�z�u�ʒu.
@param	rot		���˕���.
@param	aiming	�ǔ����邩�ǂ���.
*/
void RaceScene::SpawnMissile(glm::vec3 pos, float rot, bool aiming)
{
	m_pMissiles.push_back(new Missile);
	m_pMissiles.back()->Init(pos, rot, this, aiming);
}

/*
�j���e����.

@param	pos	���e���ˈʒu.
@param	rot	���e���˕���.
*/
void RaceScene::SpawnNuke(glm::vec3 pos, float rot)
{
	m_pNukes.push_back(new CNuke);
	m_pNukes.back()->Init(pos, rot, this);
}

/*
����ɔ��e����.

@param	pos	���e���ˈʒu.
@param	rot	���e���˕���.
*/
void RaceScene::SpawnBombs(glm::vec3 pos, float rot)
{
	m_pBombs.push_back(new Bomb);
	m_pBombs.back()->Init(pos, rot, this);
}

/*
�������o��������.

@param	pos	�o���ʒu.
@param	rot	���f���̊p�x.
*/
void RaceScene::SpawnSmoke(glm::vec3 pos, float rot)
{
	m_pSmokes.push_back(new Smoke);
	m_pSmokes.back()->Init(pos, rot, this);
}

/*
�U�A�C�e�����o������.

@param	pos	�o���ʒu.
*/
void RaceScene::SpawnFakeItem(glm::vec3 pos)
{
	m_pFakeItems.push_back(new FakeItem);
	m_pFakeItems.back()->Init(pos, this);
}

/*
�������f���̏o��.

@param	pos	�o���ʒu.
*/
void RaceScene::SpawnEffectExplode(glm::vec3 pos)
{
	m_pEffectExplodes.push_back(new EffectDestroy);
	m_pEffectExplodes.back()->Init(pos);
}

/*
�A�N�Z���̉��̏o���ʒu.

@param	pos	�o���ʒu.
*/
void RaceScene::SpawnEffectAccelDust(glm::vec3 pos)
{
	m_pAccelDusts.push_back(new AccelDust);
	m_pAccelDusts.back()->Init(pos);
}

/*
�L���L���G�t�F�N�g���o������.

@param	pos		�o���ʒu.
@param	color	�G�t�F�N�g�̐F.
@param	emitNum	�o����.
*/
void RaceScene::SpawnEffectSparkle(glm::vec3 pos, glm::vec4 color, int emitNum)
{
	m_pSparkleEffects.push_back(new CsparkleEffect);
	m_pSparkleEffects.back()->Init(pos, color, emitNum);
}

/*
�X�s�[�h�A�b�v���̃G�t�F�N�g.

@param	pos		�o���ʒu.
@param	rot		������.
@param	speed	����.
*/
void RaceScene::SpawnEffectSpeedUp(glm::vec3 pos, float rot, float speed)
{
	GameEngine& game = GameEngine::Instance();

	m_pSpeedUpEffects.push_back(new SpeedUpEffect);
	glm::vec3 vel = pos - game.CalcPosition(pos, rot + game.GetRandomFloat(50, -50), game.GetRandomFloat(50, 0), 10);
	m_pSpeedUpEffects.back()->Init(pos, glm::vec4(1, 0, 0, 0.8), vel);
}

/*
�����G�t�F�N�g�o��.

@param	pos			�o���ʒu.
@param	lifeTime	�o������.
*/
void RaceScene::SpawnEffectRespawn(glm::vec3 pos, float lifeTime)
{
	m_pRespawnEffects.push_back(new RespawnEffect);
	m_pRespawnEffects.back()->Init(pos, lifeTime);
}

/*
�ō��L�^���ǂ����̃`�F�b�N.
*/
bool RaceScene::BestTimeCheck(float Time)
{
	FILE *fp = fopen(bestTimeFileName, "wb");//�o�C�i���t�@�C�����J��
	if (fp == NULL) {//�G���[���N������NULL��Ԃ�
		return false;
	}
	fwrite(&Time, sizeof(Time), 1, fp); // SaveData_t�\���̂̒��g���o��
	fclose(fp);

	return true;
}

/*
�I�v�V������ʂ̑I��ύX.
*/
void RaceScene::OptionCommand(bool isAdd)
{
	if (isAdd) {
		this->m_optionCommand.IncreaseIndex();
	}
	else {
		this->m_optionCommand.DecreaseIndex();
	}
}

/*
�I�v�V������ʂ̍��ڎ��s.
*/
void RaceScene::ActOptionCommand()
{
	if (m_optionCommand.getIndex() == 0) {
		blackFilter.SetNextScene(NEXT_SCENE::InRace);

	}
	else if (m_optionCommand.getIndex() == 1) {
		blackFilter.SetNextScene(NEXT_SCENE::StageSelect);

	}
	else if (m_optionCommand.getIndex() == 2) {
		blackFilter.SetNextScene(NEXT_SCENE::Title);
	}
}

/*
�w��͈͓���SE�𗬂����ǂ����̃`�F�b�N.

@param	pos			�����̏ꏊ.
@param	distance	�������Đ��\�̏ꏊ.
@param	audioType	�����̃^�C�v.
@param	audioId		����������ID.
*/
void RaceScene::PlayAudioCheck(glm::vec3 pos, float distance, int audioType, int audioId)
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		if (!m_pPlayerCharacters[i]->IsAutoDrive()) {
			if (glm::distance(m_pPlayerCharacters[i]->Position(), pos) < distance) {

				GameEngine::Instance().PlayAudio(audioType, audioId);
			}
		}
	}
}

/*
�t�B�[���h��̃A�C�e�����擾.

@return	�A�C�e���̃|�C���^.
*/
std::vector<Item*> RaceScene::GetFieldItems()
{
	return m_pItems;
}

/*
�����蔻��t������.

@param	pos	�o���ʒu.
*/
void RaceScene::SpawnExplode(glm::vec3 pos)
{
	m_pExplodes.push_back(new Explosion);
	m_pExplodes.back()->Init(pos, this);
}

/*
�|�[�Y��Ԑ؂�ւ�.

@param	playerIndex	�|�[�Y�����v���C���[�̃C���f�b�N�X.
*/
void RaceScene::TogglePause(int playerIndex)
{
	GameEngine& game = GameEngine::Instance();

	if (this->m_pausing) {
		m_pausing = false;
		this->PauseIndex = playerIndex;

		game.PlayAudio(SEUI, CRI_SOUND_OPENMENU);
	}
	else {
		m_pausing = true;
		game.PlayAudio(SEUI, CRI_SOUND_CANCEL);
	}
}

/*
����������.
*/
void RaceScene::Init()
{
	GameEngine& game = GameEngine::Instance();

	game.SetCameraNum(PlayerNum);

	m_isInitialized = true;

	this->m_activeInput = true;
	this->blackFilter.Init();

	this->LoadStageFromFile(stageFileName);
	this->LoadCheckPointFromFile(checkPointFileName);
	this->LoadItemsFromFile(itemFileName);
	this->LoadStageTextures(textureFileName);

	game.Camera({ { 0,20, -8 },{ 0, 0, 12 },{ 0, 0, 1 } }, 0);
	game.Camera({ { 400, 950, 300},{400,0,300},{1,0,0} }, 3);
	game.AmbientLight(glm::vec4(0.005f, 0.1f, 0.2f, 1));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			game.Light((i * 3 + j), { { -200 + i * 400, 200, -200 + j * 400, 1 }, { 24000,24000,24000,1 } });
		}
	}

	for (int i = 0; i < 4; i++) {
		m_pPlayerCharacters.push_back(new CPlayerCharacter);
		
		bool playerCheck = false;
		if (i < PlayerNum) {
			playerCheck = true;
		}
		m_pPlayerCharacters.back()->Init(glm::vec3(i * 5, 0, 0), 2.0f, i, this, m_pGoals, playerCheck);
	}

	m_optionCommand.InitCommand(3);

	m_pausing = false;
	m_isInResult = false;
	CountDown = 6.0f;
	Time = 0.0f;

	m_resultWaitTime = 5.0f;
	m_showRanking = false;
	m_resultMover = game.GetWindowSize().x;

	game.PlayAudio(BGM, CRI_SOUND_STAGEBGM2);
}

/*
�Q�[���I�����������s����.
*/
void RaceScene::EndFunc()
{
	GameEngine& game = GameEngine::Instance();

	this->SaveBestLap();

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		m_pPlayerCharacters[i]->DestroyEntity();
	}
	game.DeleteAll(m_pPlayerCharacters);

	for (int i = 0; i < m_pItems.size(); i++) {
		m_pItems[i]->Destroy();
	}
	game.DeleteAll(m_pItems);

	for (int i = 0; i < m_pObstacles.size(); i++) {
		m_pObstacles[i]->Entity()->Destroy();
	}
	game.DeleteAll(m_pObstacles);

	for (int i = 0; i < m_pGoals.size(); i++) {
		m_pGoals[i]->Destroy();
	}
	game.DeleteAll(m_pGoals);

	for (int i = 0; i < m_pMissiles.size(); i++) {
		m_pMissiles[i]->Destroy();
	}
	game.DeleteAll(m_pMissiles);

	for (int i = 0; i < m_pElectroTraps.size(); i++) {
		m_pElectroTraps[i]->Destroy();
	}
	game.DeleteAll(m_pElectroTraps);

	for (int i = 0; i < m_pBombs.size(); i++) {
		m_pBombs[i]->Destroy();
	}
	game.DeleteAll(m_pBombs);

	for (int i = 0; i < m_pFakeItems.size(); i++) {
		m_pFakeItems[i]->Destroy();
	}
	game.DeleteAll(m_pFakeItems);

	for (int i = 0; i < m_pNukes.size(); i++) {
		m_pNukes[i]->Destroy();
	}
	game.DeleteAll(m_pNukes);

	for (int i = 0; i < m_pSmokes.size(); i++) {
		m_pSmokes[i]->Destroy();
	}
	game.DeleteAll(m_pSmokes);

	for (int i = 0; i < m_pExplodes.size(); i++) {
		m_pExplodes[i]->Destroy();
	}
	game.DeleteAll(m_pExplodes);

	for (int i = 0; i < m_pEffectExplodes.size(); i++) {
		m_pEffectExplodes[i]->Destroy();
	};
	game.DeleteAll(m_pEffectExplodes);

	game.DeleteAll(m_pAccelDusts);
	game.DeleteAll(m_pSparkleEffects);
	game.DeleteAll(m_pSpeedUpEffects);
	game.DeleteAll(m_pRespawnEffects);

	game.RemoveGroundTexture();

	game.StopAllSound();
}

/*
�Q�[�����̍X�V����.

@param	delta	1�t���[���̍X�V����,
*/
void RaceScene::UpdateInGame(float delta)
{
	GameEngine& game = GameEngine::Instance();

	this->PlayerUpdate(delta);
	this->CheckPoint();
	this->ItemUpdate(delta);
	this->UpdateEffects(delta);
	this->CollisionChecks();
	this->RankingCheck();
	this->ShowUI(delta);

	this->CheckAllPlayerFinished();

	Time += delta;
}

/*
�Q�[���J�n�܂ł̍X�V����.
*/
void RaceScene::UpdateGameStart(float delta)
{
	GameEngine& game = GameEngine::Instance();
	GLFWEW::Window& window = GLFWEW::Window::instance();

	glm::vec3 cameraPos[4];

	float adder = 0;

	if (CountDown > 3.0f) {
		adder = (CountDown - 3.0f) * 10.0f;
	}

	for (int i = 0; i < PlayerNum; i++) {
		cameraPos[i] = game.CalcPosition(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Yrot() + 180, 15, 20 + adder);

		game.Camera({ { cameraPos[i] },{ m_pPlayerCharacters[i]->Position() },{ 0, 1, 0 } }, i);
	}

	this->ItemUpdate(delta);
	this->UpdateEffects(delta);

	this->RankingCheck();
	this->ShowUI(delta);
}

/*
�|�[�Y���̍X�V����.
*/
void RaceScene::UpdatePause(float delta)
{
	this->UpdateEffects(delta);
	this->m_pPlayerCharacters[PauseIndex]->InputOptionFunc();
	this->GoalInformUi();

	this->OptionUI();
	this->ShowUI(delta);
}

/*
�S���S�[�������ォ�猋�ʔ��\�܂ł̋L�^.
*/
void RaceScene::UpdateResult(float delta)
{
	this->PlayerUpdate(delta);
	this->ItemUpdate(delta);
	this->UpdateEffects(delta);

	this->CheckPoint();
	this->CollisionChecks();

	if (m_resultWaitTime > 0) {
		this->ShowUI(delta);
		m_resultWaitTime -= delta;
		this->GoalInformUi();
	}
	else
	{
		if (!m_showRanking) {
			blackFilter.SetNextScene(NEXT_SCENE::Result);
		}
	}
	
	//�Ō�̃����L���O�\��.
	if (m_showRanking) {
		GameEngine& game = GameEngine::Instance();

		game.Camera({ {-20, 20, -100},{0, 0, 0}, {0, 1, 0} }, 0);

		if (!blackFilter.IsInAction()) {
			this->RankingUI(delta);

			if (m_resultMover <= 0) {
				this->m_pPlayerCharacters[0]->InputOptionFunc();
				OptionUI();
			}
		}
	}

	Time += delta;
}

/*
�X�e�[�W�̕ǂ̃f�[�^�̓ǂݍ���.
*/
bool RaceScene::LoadStageFromFile(char * const filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	int line = 0;

	glm::vec3 from;
	glm::vec3 to;

	while (fscanf(fp, "%f,%f,%f,%f,%f,%f", &from.x, &from.y, &from.z, &to.x, &to.y, &to.z) != EOF)
	{
		WallSetPointToPoint(from, to, 10.0f);

		line++;
	}

	fclose(fp);

	return true;
}

/*
���̃`�F�b�N�|�C���g�܂ł̃t�@�C����ǂݍ���.
*/
bool RaceScene::LoadCheckPointFromFile(char* const filename)
{
	GameEngine& game = GameEngine::Instance();

	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	int line = 0;

	glm::vec3 pos;
	glm::vec3 min;
	glm::vec3 max;

	while (fscanf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f", &pos.x, &pos.y, &pos.z, &min.x, &min.y, &min.z, &max.x, &max.y, &max.z) != EOF)
	{
		m_pGoals.push_back(new Goal);
		m_pGoals.back()->Init(pos, min, max);
	}

	fclose(fp);

	return true;
}

/*
�A�C�e���̏��̓ǂݍ���.
*/
bool RaceScene::LoadItemsFromFile(char* const filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	int line = 0;

	glm::vec3 pos;
	int id;

	while (fscanf(fp, "%f,%f,%f,%d", &pos.x, &pos.y, &pos.z, &id) != EOF)
	{
		this->m_pItems.push_back(new Item);
		this->m_pItems.back()->Init(pos, id);
	}

	fclose(fp);

	return true;
}

/*
�ǂ�z�u����.

@param	from	�ǂ̔z�u�J�n�ꏊ,
@param	to		�ǂ̔z�u�I���ꏊ.
@param	space	�ǂƎ��̕ǔz�u�܂ł̊Ԋu.
*/
void RaceScene::WallSetPointToPoint(glm::vec3 from, glm::vec3 to, float space)
{
	GameEngine& game = GameEngine::Instance();

	float distance = glm::distance(from, to);

	for (float i = 0; distance > 0; distance -= space, i++) {
		double radian = atan2(to.x - from.x, to.z - from.z);
		double degree = glm::degrees(radian);

		m_pObstacles.push_back(new Obstacle);
		m_pObstacles.back()->Init(game.CalcPosition(from, degree, 0.0f, i * space), space);
	}
}

/*
�X�e�[�W�Ɏg�p���Ă���e�N�X�`����ǂݍ���.
*/
bool RaceScene::LoadStageTextures(char* const filename)
{
	GameEngine& game = GameEngine::Instance();

	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	int line = 0;

	char texfile[64];

	while (fscanf(fp, "%s", &texfile) != EOF)
	{
		if (line == 0) {
			game.SetCubeMap(texfile);
		}
		else if (line >= 1 && line <= 5) {
			game.SetGroundTexture(line - 1, texfile);
		}
		else if (line == 6) {
			this->m_minimap.Init(glm::vec2(0, 0), texfile);
		}
		line++;
	}

	return true;
}

/*
�v���C���[�X�V����,
*/
void RaceScene::PlayerUpdate(float delta)
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		m_pPlayerCharacters[i]->Update(delta);

		m_pPlayerCharacters[i]->AutoDrive(delta, m_pPlayerCharacters);
	}
}

/*
�A�C�e���̍X�V����.
*/
void RaceScene::ItemUpdate(float delta)
{
	for (int i = 0; i < m_pItems.size(); i++) {
		this->m_pItems[i]->Update(delta);
	}

	for (int i = 0; i < m_pGoals.size(); i++) {
		m_pGoals[i]->ToggleVisible(false);
	}

	if (m_pPlayerCharacters[0]->GetCheckPointIndex() != m_pGoals.size() - 1) {
		m_pGoals[m_pPlayerCharacters[0]->GetCheckPointIndex() + 1]->ToggleVisible(true);
	}
	else
		m_pGoals[0]->ToggleVisible(true);

	for (int i = 0; i < m_pMissiles.size(); i++) {
		m_pMissiles[i]->Update(delta, m_pPlayerCharacters);

		for (int j = 0; j < m_pObstacles.size(); j++) {
			m_pMissiles[i]->CollisionCheck(m_pObstacles[j]->Position(), m_pObstacles[j]->Radius());
		}
	}

	for (int i = 0; i < m_pNukes.size(); i++) {
		m_pNukes[i]->Update(delta);
	}

	for (int j = 0; j < m_pBombs.size(); j++) {
		m_pBombs[j]->Update(delta);
	}

	for (int i = 0; i < m_pExplodes.size(); i++) {
		m_pExplodes[i]->Update(delta);
	}

	for (int i = 0; i < m_pSmokes.size(); i++) {
		m_pSmokes[i]->Update(delta);
	}

	for (int i = 0; i < m_pFakeItems.size(); i++) {
		m_pFakeItems[i]->Update(delta);
	}

	for (int i = 0; i < m_pEffectExplodes.size(); i++) {
		m_pEffectExplodes[i]->Update(delta);
	}
}

/*
�G�t�F�N�g�ނ̕`�揈��.
*/
void RaceScene::UpdateEffects(float delta)
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < m_pAccelDusts.size(); i++) {
		if (!m_pausing) {
			m_pAccelDusts[i]->Update(delta);
		}
		m_pAccelDusts[i]->Draw();

		if (!m_pAccelDusts[i]->ActiveCheck()) {
			game.Remove(m_pAccelDusts, i);
			i--;
		}
	}

	for (int i = 0; i < m_pSparkleEffects.size(); i++) {
		if (!m_pausing) {
			m_pSparkleEffects[i]->Update(delta);
		}

		if (!m_pSparkleEffects[i]->IsActive()) {
			game.Remove(m_pSparkleEffects, i);
			i--;
		}
	}

	for (int i = 0; i < m_pSpeedUpEffects.size(); i++) {
		if (!m_pausing) {
			m_pSpeedUpEffects[i]->Update(delta);
		}
		m_pSpeedUpEffects[i]->Draw();

		if (!m_pSpeedUpEffects[i]->IsActive()) {
			game.Remove(m_pSpeedUpEffects, i);
			i--;
		}
	}

	for (int i = 0; i < m_pRespawnEffects.size(); i++) {
		if (!m_pausing) {
			m_pRespawnEffects[i]->Update(delta);
		}
		m_pRespawnEffects[i]->Draw();

		if (!m_pRespawnEffects[i]->IsActive()) {

			SpawnEffectSparkle(m_pRespawnEffects[i]->Position(), glm::vec4(1, 1, 0, 1), 5);
			SpawnEffectSparkle(m_pRespawnEffects[i]->Position(), glm::vec4(0, 1, 1, 1), 5);

			game.Remove(m_pRespawnEffects, i);
			i--;

		}
	}
}

/*
�L���t���O���m�F���|�C���^���폜����֐�.
*/
void RaceScene::DeleteCheck()
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < m_pMissiles.size(); i++) {
		if (!m_pMissiles[i]->IsActive()) {
			game.Remove(m_pMissiles, i);
			i--;
		}
	}

	for (int i = 0; i < m_pElectroTraps.size(); i++) {
		if (!m_pElectroTraps[i]->IsActive()) {
			game.Remove(m_pElectroTraps, i);
			i--;
		}
	}

	for (int i = 0; i < m_pNukes.size(); i++) {
		if (!m_pNukes[i]->IsActive()) {
			game.Remove(m_pNukes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pSmokes.size(); i++) {
		if (!m_pSmokes[i]->IsActive()) {
			game.Remove(m_pSmokes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pBombs.size(); i++) {
		if (!m_pBombs[i]->IsActive()) {
			game.Remove(m_pBombs, i);
			i--;
		}
	}

	for (int i = 0; i < m_pFakeItems.size(); i++) {
		if (!m_pFakeItems[i]->IsActive()) {
			game.Remove(m_pFakeItems, i);
			i--;
		}
	}

	for (int i = 0; i < m_pExplodes.size(); i++) {
		if (!m_pExplodes[i]->IsActive()) {
			game.Remove(m_pExplodes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pEffectExplodes.size(); i++) {
		if (!m_pEffectExplodes[i]->IsActive()) {
			game.Remove(m_pEffectExplodes, i);
			i--;
		}
	}
}

/*
�`�F�b�N�|�C���g�ʉ߂̃`�F�b�N.
*/
void RaceScene::CheckPoint()
{
	for (int i = 0; i < m_pGoals.size(); i++) {
		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			if (m_pPlayerCharacters[j]->GetCheckPointIndex() == i) {
				continue;
			}

			if (m_pGoals[i]->CollisionCheck(m_pPlayerCharacters[j]->Position(), m_pPlayerCharacters[j]->Radius()))
			{
				m_pPlayerCharacters[j]->PassCheckPoint(i, Time);
			}
		}
	}
}

/*
�����蔻��.
*/
void RaceScene::CollisionChecks()
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		
		for (int j = 0; j < m_pItems.size(); j++) {
			int itemId = this->m_pItems[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius());

			if (itemId != -1) {
				this->m_pPlayerCharacters[i]->ObtainItem(itemId);
				SpawnEffectSparkle(m_pItems[j]->Position(), glm::vec4(0, 1, 1, 0.5f), 5);
			}
		}

		for (int j = 0; j < m_pObstacles.size(); j++) {
			m_pPlayerCharacters[i]->CollisionCheckAndBounce(
				m_pObstacles[j]->Position(), m_pObstacles[j]->Radius());
		}
		
		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			if (i != j) {
				if (!m_pPlayerCharacters[i]->IsDead() && !m_pPlayerCharacters[j]->IsDead()) {
					m_pPlayerCharacters[i]->CollisionCheckAndBounce(m_pPlayerCharacters[j]->Position(), m_pPlayerCharacters[j]->Radius());
				}
			}
		}

		for (int j = 0; j < m_pElectroTraps.size(); j++) {
			if (!m_pPlayerCharacters[i]->IsDead() && m_pElectroTraps[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius())) {

				m_pPlayerCharacters[i]->Dead();
			}
		}

		for (int j = 0; j < m_pMissiles.size(); j++) {
			if (!m_pPlayerCharacters[i]->IsDead() && m_pMissiles[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius())) {

				this->m_pPlayerCharacters[i]->Dead();
			}
		}

		for (int j = 0; j < m_pExplodes.size(); j++) {
			if (!m_pPlayerCharacters[i]->IsDead() && m_pExplodes[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius())) {
				this->m_pPlayerCharacters[i]->Dead();
			}
		}

		for (int j = 0; j < m_pFakeItems.size(); j++) {
			if (!m_pPlayerCharacters[i]->IsDead() && m_pFakeItems[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius())) {
				this->m_pPlayerCharacters[i]->Dead();
			}
		}
	}
}

/*
���ʂ̊m�F�p.(�S�[���܂ł̋���)
*/
void RaceScene::RankingCheck()
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		float score = m_pPlayerCharacters[i]->GetLap() * 1000000 + m_pPlayerCharacters[i]->GetCheckPointIndex() * 10000 -
			glm::distance(m_pGoals[m_pPlayerCharacters[i]->GetNextCheckPointIndex()]->Position(), m_pPlayerCharacters[i]->Position());

		this->m_pPlayerCharacters[i]->SetScore(score);
	}

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		
		if (m_pPlayerCharacters[i]->GetFinishedRace())
			continue;

		int rank = 1;

		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			if (i == j)
				continue;

			if (m_pPlayerCharacters[i]->GetScore() < m_pPlayerCharacters[j]->GetScore()) {
				rank++;
			}
		}

		m_pPlayerCharacters[i]->SetPlayerRank(rank);
	}
}

/*
�S�v���C���[���S�[���������`�F�b�N����.
*/
void RaceScene::CheckAllPlayerFinished()
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		if (!m_pPlayerCharacters[i]->IsAutoDrive()) {
			return;
		}

		if (m_pPlayerCharacters.size() - 1 == i) {
			this->m_isInResult = true;

			this->CalcFinishTime();
			this->RankingSortFromTime();
		}
	}
}

/*
�Q�[���I����AI�̃S�[���B���܂ł̂����悻�̎��Ԃ��v�Z����.
*/
void RaceScene::CalcFinishTime()
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		if (!m_pPlayerCharacters[i]->GetFinishedRace()) {

			while (!m_pPlayerCharacters[i]->GetFinishedRace())
			{
				if (m_pPlayerCharacters[i]->GetPlayerLapTime().size() > 0) {
					float time = *m_pPlayerCharacters[i]->GetPlayerLapTime().begin();
					m_pPlayerCharacters[i]->PassGoal(time * (m_pPlayerCharacters[i]->GetLap() + 1));
				}

				else break;
			}
		}
	}
}

/*
�Ō�ɃS�[���������Ԃŏ��ʂ��\�[�g����.
*/
void RaceScene::RankingSortFromTime()
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		m_pPlayerCharacters[i]->SetPlayerRank(0);
	}

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {

		int rank = 1;

		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			if (i == j)
				continue;

			if (m_pPlayerCharacters[j]->GetFinishTime() <= 0.0f)
				continue;

			if (m_pPlayerCharacters[i]->GetFinishTime() > m_pPlayerCharacters[j]->GetFinishTime()) {
				rank++;
			}
		}


		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			if (i == j)
				continue;
			if (rank == m_pPlayerCharacters[j]->GetRank()) {
				rank++;
			}
		}

		printf("%dP  %drank\n", i, rank);

		m_pPlayerCharacters[i]->SetPlayerRank(rank);
	}
}


/*
����̍ō��L�^���X�V����.
*/
void RaceScene::SaveBestLap()
{
	FILE* fp = fopen(bestTimeFileName, "rb");
	if (!fp) {
		return;
	}

	float num = 0.0f;
	fscanf(fp, "%f", &num);


	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		for (int j = 0; j < m_pPlayerCharacters[i]->GetPlayerLapTime().size(); j++) {
			
			if (num > m_pPlayerCharacters[i]->GetPlayerLapTime()[j]) {
				
				num = m_pPlayerCharacters[i]->GetPlayerLapTime()[j];

				std::ofstream File(bestTimeFileName);

				File << num << std::endl;
			}
		}
	}
}

/*
�Ó]��������уV�[���J��.

@param	delta	�X�V����.
*/
void RaceScene::SceneChanger(float delta) 
{
	GameEngine& game = GameEngine::Instance();

	blackFilter.Update(delta);
	
	switch (blackFilter.NextScene())
	{
	case NEXT_SCENE::InRace:
		this->EndFunc();
		this->m_isInitialized = false;
		break;

	case NEXT_SCENE::StageSelect:
		this->EndFunc();
		game.UpdateFunc(StageSelectScene());
		break;

	case NEXT_SCENE::Title:
		this->EndFunc();
		game.UpdateFunc(TitleScene());
		break;

	case NEXT_SCENE::Result:
		
		blackFilter.Init();
		this->m_showRanking = true;
		game.SetCameraNum(1);

	case NEXT_SCENE::None:
	default:
		break;
	}
}

/*
��ʏ��UI�̕\��.
*/
void RaceScene::ShowUI(float delta)
{
	GameEngine& game = GameEngine::Instance();

	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	for (int i = 0; i < PlayerNum; i++) {
		
		game.FontColor(glm::vec4(1.0f));
		game.FontScale(glm::vec2(2.0f));
		game.AddString(glm::vec2(windowSize.x - 430.0f, 24.0f), "Lap", i);
		
		if (m_pPlayerCharacters[i]->GetLap() <= lapNum) {
			snprintf(str, 16, "%d", m_pPlayerCharacters[i]->GetLap());
			game.FontColor(glm::vec4(1, 1, 0, 1));
		}
		else {
			snprintf(str, 16, "%d", lapNum);
			game.FontColor(glm::vec4(1, 0, 0, 1));
		}
		game.AddString(glm::vec2(windowSize.x - 300.0f, 24.0f), str, i);

		game.FontColor(glm::vec4(1, 1, 1, 1));
		game.AddString(glm::vec2(windowSize.x - 225.0f, 24.0f), "/", i);

		snprintf(str, 16, "%d", lapNum);
		game.FontColor(glm::vec4(1, 1, 0, 1));
		game.AddString(glm::vec2(windowSize.x - 150.0f, 24.0f), str, i);

		game.FontScale(glm::vec2(1.5f));
		game.FontColor(glm::vec4(1.0f));
		game.AddString(glm::vec2(windowSize.x - 280.0f, windowSize.y - 110.0f), "Speed", i);

		game.ImageColor(glm::vec4(0, 1, 0, 1));
		game.ImageScale(glm::vec2(0.75));
		game.AddImage(glm::vec2(windowSize.x - 310.0f, windowSize.y - 120.0f), "res/Texture/SpeedMeter.dds", i);

		snprintf(str, 16, "%06.2f", m_pPlayerCharacters[i]->Velocity());
		game.FontScale(glm::vec2(1.5f));
		game.FontColor(glm::vec4(1.0f));
		game.AddString(glm::vec2(windowSize.x - 280.0f, windowSize.y - 60.0f), str, i);
		
		game.ImageScale(glm::vec2(0.75));
		game.AddImage(glm::vec2(windowSize.x - 310.0f, windowSize.y - 70.0f), "res/Texture/SpeedMeter.dds", i);

		snprintf(str, 16, "%d Place", m_pPlayerCharacters[i]->GetRank());

		game.FontColor(glm::vec4(0, 0, 0, 1));
		game.FontScale(glm::vec2(2.0));
		game.AddString(glm::vec2(windowSize.x - 305.0f, windowSize.y - 195.0f), str, i);

		game.FontColor(rankColor[m_pPlayerCharacters[i]->GetRank()]);
		game.FontScale(glm::vec2(2.0));
		game.AddString(glm::vec2(windowSize.x - 310.0f, windowSize.y - 200.0f), str, i);

		if ((ItemsCode)m_pPlayerCharacters[i]->GetItemId() != ItemsCode::NONE && (ItemsCode)m_pPlayerCharacters[i]->GetStockItemId() != ItemsCode::NONE) {

			if ((int)(Time * 10) % 2 == 0) {
				game.FontColor(glm::vec4(1, 0, 1, 1));
				game.ImageColor(glm::vec4(1, 0, 1, 1));
			}
			else {
				game.FontColor(glm::vec4(0, 1, 1, 1));
				game.ImageColor(glm::vec4(0, 1, 1, 1));
			}
			game.FontScale(glm::vec2(1.2f));
			game.AddString(glm::vec2(24.0, windowSize.y - 180.0f), "MIX", i);

			game.FontColor(glm::vec4(1, 1, 0, 1));
			game.AddString(glm::vec2(100.0, windowSize.y - 180.0f), "L + R", i);
		}
		else {
			game.ImageColor(glm::vec4(0, 1, 1, 1));
		}

		this->ItemUI(glm::vec2(36.0, windowSize.y - 130.0f), glm::vec2(1.0f), (ItemsCode)(m_pPlayerCharacters[i]->GetStockItemId()), i);
		game.ImageScale(glm::vec2(0.75f));
		game.AddImage(glm::vec2(8.0f, windowSize.y - 145.0f), "res/Texture/SpeedMeter.dds", i);

		this->ItemUI(glm::vec2(36.0, windowSize.y - 80.0f), glm::vec2(2), (ItemsCode)(m_pPlayerCharacters[i]->GetItemId()), i);
		game.ImageScale(glm::vec2(1.1f, 1.0f));
		game.AddImage(glm::vec2(8.0, windowSize.y - 100.0f), "res/Texture/SpeedMeter.dds", i);


		for (int j = 0, line = 0; j < m_pPlayerCharacters[i]->GetPlayerLapTime().size(); j++, line++) {

			if (line == 0 && m_pPlayerCharacters[i]->GetLap() > 5) {
				j = m_pPlayerCharacters[i]->GetLap() - 5;
			}

			snprintf(str, 16, "%d", j + 1);

			game.FontColor(glm::vec4(1, 0, 0, 1));
			game.FontScale(glm::vec2(1.6f));
			game.AddString(glm::vec2(16, 76 + line * 60), str, i);

			game.AddString(glm::vec2(48, 76 + line * 60), ",", i);

			float second = m_pPlayerCharacters[i]->GetPlayerLapTime()[j];

			float minute = 0.0;
			for (int k = 0; second > 60; k++) {
				second -= 60;
				minute += 1.0;
			}

			snprintf(str, 16, "%02.0f:%05.2f", minute, second);
			game.FontColor(glm::vec4(1, 1, 0, 1));
			game.FontScale(glm::vec2(1.6f));
			game.AddString(glm::vec2(64, 76 + line * 60), str, i);
		}

		this->DrawMapUi(i);
	}

	if (this->CountDown < 3.0 && this->CountDown > -1.0f) {

		float CountDownSize = static_cast<float>((static_cast<int>(CountDown * 100.0f) % 100)) * 0.2f;

		if (CountDownSize > 10) {
			CountDownSize -= 10;
		}
		else {
			CountDownSize = 0;
		}

		if (this->CountDown > 0.0) {

			snprintf(str, 16, "%01.0f", CountDown + 0.5f);

			game.FontColor(glm::vec4(0, 0, 0, 1));
			game.FontScale(glm::vec2(5.0f + CountDownSize));
			game.AddString(glm::vec2(windowSize * 0.45f + CountDownSize * -10 + glm::vec2(10.0f)), str, 0, true);

			game.FontColor(glm::vec4(1, 1, 0, 1));
			game.FontScale(glm::vec2(5.0f + CountDownSize));
			game.AddString(glm::vec2(windowSize * 0.45f + CountDownSize * -10), str, 0, true);
		}
		else
		{
			game.FontColor(glm::vec4(0, 0, 0, 1));
			game.FontScale(glm::vec2(5));
			game.AddString(glm::vec2(windowSize * 0.4f + glm::vec2(10.0f)), "go!", 0, true);

			game.FontColor(glm::vec4(1, 1, 0, 1));
			game.FontScale(glm::vec2(5));
			game.AddString(glm::vec2(windowSize * 0.4f), "go!", 0, true);
		}
	}

	snprintf(str, 16, "%05.2f", 1.0f / delta);
	game.FontColor(glm::vec4(1));
	game.FontScale(glm::vec2(1));
	game.AddString(glm::vec2(100, 700), str, 0 , true);

	game.AddString(glm::vec2(10, 700), "FPS :", 0, true);
}

/*
�������Ă���A�C�e���̕\��.

@param	pos			��ʂɕ\����������W.
@param	size		�\�������镶���̃T�C�Y.
@param	ItemsCode	�������Ă���A�C�e��,
@param	index		�v���C���[�̃C���f�b�N�X.
*/
void RaceScene::ItemUI(glm::vec2 pos, glm::vec2 size, ItemsCode id, int index)
{
	GameEngine& game = GameEngine::Instance();

	char str[16];

	game.FontScale(size);

	switch (id)
	{
	case ItemsCode::WIND:
		game.FontColor(glm::vec4(0, 1, 0, 1));
		snprintf(str, 16, "%s", "Wind");
		break;
	case ItemsCode::FIRE:
		game.FontColor(glm::vec4(1, 0, 0, 1));
		snprintf(str, 16, "%s", "Fire");
		break;
	case ItemsCode::WATER:
		game.FontColor(glm::vec4(0, 0, 1, 1));
		snprintf(str, 16, "%s", "Water");
		break;
	case ItemsCode::ELCTRO:
		game.FontColor(glm::vec4(1, 1, 0, 1));
		snprintf(str, 16, "%s", "Electro");
		break;
	default:
		game.FontColor(glm::vec4(0.6, 0.6, 0.6, 1));
		snprintf(str, 16, "%s", "None");
		break;
	}

	game.AddString(pos, str, index);
}

/*
�~�j�}�b�v�\��.
*/
void RaceScene::DrawMapUi(int playerIndex)
{
	this->m_minimap.Update(m_pPlayerCharacters, playerIndex);
}

/*
�S�[��������点��UI.
*/
void RaceScene::GoalInformUi()
{
	GameEngine& game = GameEngine::Instance();
	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	for (int i = 0; i < PlayerNum; i++) {
		if (m_pPlayerCharacters[i]->GetLap() > this->lapNum) {
			game.FontColor(glm::vec4(1, 0, 0, 1));

			game.FontScale(glm::vec2(4.0));

			game.AddString(glm::vec2(windowSize.x * 0.2f, windowSize.y * 0.2), "GOAL!", i);

			snprintf(str, 16, "%d Place!", m_pPlayerCharacters[i]->GetRank());
			game.FontScale(glm::vec2(4.0));
			game.FontColor(rankColor[0]);
			game.AddString(glm::vec2(windowSize.x * 0.3f + 5, windowSize.y * 0.4 + 5), str, i);

			if ((int)(Time * 10) % 2) {
				game.FontColor(rankColor[m_pPlayerCharacters[i]->GetRank()]);
			}
			else {
				game.FontColor(glm::vec4(1));
			}
			game.AddString(glm::vec2(windowSize.x * 0.3f, windowSize.y * 0.4), str, i);
		}
	}
}

/*
�I�v�V������ʂ��J������Ԃ�UI.
*/
void RaceScene::OptionUI()
{
	GameEngine& game = GameEngine::Instance();

	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	if (m_pausing) {
		snprintf(str, 16, "Player%d pause", PauseIndex + 1);
		game.AddString(glm::vec2(windowSize.x * 0.2f, windowSize.y * 0.2f), str, 0, true);
	}

	if (m_optionCommand.getIndex() == 0) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(windowSize.x * 0.4, windowSize.y -300.0f), "Retry", 0, true);
	game.ImageScale(glm::vec2(1.2, 1));
	game.AddImage(glm::vec2(windowSize.x * 0.4 - 50, windowSize.y -325.0f), "res/Texture/SpeedMeter.dds", 0, true);

	if (m_optionCommand.getIndex() == 1) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(windowSize.x * 0.4, windowSize.y - 200.0f), "Stage Select", 0, true);
	game.AddImage(glm::vec2(windowSize.x * 0.4 - 50, windowSize.y - 225.0f), "res/Texture/SpeedMeter.dds", 0 ,true);

	if (m_optionCommand.getIndex() == 2) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(windowSize.x * 0.4, windowSize.y - 100.0f), "Back To Title", 0, true);
	game.AddImage(glm::vec2(windowSize.x * 0.4 - 50, windowSize.y - 125.0f), "res/Texture/SpeedMeter.dds", 0, true);
}

/*
�Ō�̏��ʕt����UI�̕\��������.
*/
void RaceScene::RankingUI(float delta)
{
	GameEngine& game = GameEngine::Instance();

	glm::vec2 windowSize = game.GetWindowSize();

	char str[64];

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {

		int rank = m_pPlayerCharacters[i]->GetRank();
		float m_time = m_pPlayerCharacters[i]->GetFinishTime();
		int minute = 0;
		float second = 0.0f;

		for (second = m_time; second >= 60.0f; ) {
			minute += 1;
			second -= 60.0f;
		}

		snprintf(str, 64, "%d Place   Player%d -   %02d:%05.2f",rank , i + 1, minute, second);
		if (i <= PlayerNum - 1) {
			game.FontColor(glm::vec4(0, 1, 1, 1));
		}
		else {
			game.FontColor(glm::vec4(1));
		}
		
		game.FontScale(glm::vec2(2));
		game.AddString(glm::vec2(400 + m_resultMover, 50 + rank * 100), str, 0, true);

		game.ImageColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		game.ImageScale(glm::vec2(1.7, 1));
		game.AddImage(glm::vec2(350 + m_resultMover, 130 + i * 100), "res/Texture/SpeedMeter.dds", 0, true);
	}

	if (m_resultMover > 0.0f) {
		m_resultMover -= delta * windowSize.x;
		if (m_resultMover <= 0.0f) {
			m_resultMover = 0.0f;
		}
	}
}