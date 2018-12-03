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

template<typename T>
void Remove(std::vector<T>& vector, int index)
{
	delete vector[index];
	vector.erase(vector.begin() + index);
}

template<typename T>
void DeleteAll(std::vector<T>& vector)
{
	for (int i = 0; i < vector.size(); i++) {
		delete vector[i];
	}

	vector.clear();
}

glm::vec3 calcPosition(glm::vec3 pos, float rotateX, float rotateY, float distance)
{
	rotateX = glm::radians(rotateX);
	rotateY = glm::radians(rotateY);

	float x = sin(rotateX) * distance;
	float z = cos(rotateX) * distance;

	float lengthRatio = cos(rotateY) * x;
	float lengthZ = cos(rotateY) * z;
	float y = sin(rotateY) * distance;

	return glm::vec3(pos.x + lengthRatio, pos.y + y, pos.z + lengthZ);
}

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

	if (!isInitialized) {
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

void RaceScene::SpawnElectroTrap(glm::vec3 pos)
{
	this->m_pElectroTraps.push_back(new ElectroTrap);
	this->m_pElectroTraps.back()->init(pos, this);
}

void RaceScene::SpawnMissile(glm::vec3 pos, float Xrot, bool aiming)
{
	m_pMissiles.push_back(new Missile);
	m_pMissiles.back()->Init(pos, Xrot, this, aiming);
}

void RaceScene::SpawnNuke(glm::vec3 pos, float y_rot)
{
	int index = -1;
	int value = 0;
	glm::vec3* position = nullptr;

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {

		if (index == -1) {
			index = i;
			position = &m_pPlayerCharacters[i]->Position();
			value = m_pPlayerCharacters[i]->GetLap() + m_pPlayerCharacters[i]->GetCheckPointIndex();
		}
		else {
			if (value > m_pPlayerCharacters[i]->GetLap() + m_pPlayerCharacters[i]->GetCheckPointIndex()) {
				index = i;
				position = &m_pPlayerCharacters[i]->Position();
			}
		}
	}

	m_pNukes.push_back(new CNuke);

	m_pNukes.back()->Init(pos, y_rot, this);
}

void RaceScene::SpawnBombs(glm::vec3 pos, float yRot)
{
	m_pBombs.push_back(new Bomb);
	m_pBombs.back()->Init(pos, yRot, this);
}

void RaceScene::SpawnSmoke(glm::vec3 pos, float y_rot)
{
	m_pSmokes.push_back(new Smoke);
	m_pSmokes.back()->Init(pos, y_rot, this);
}

void RaceScene::SpawnFakeItem(glm::vec3 pos)
{
	m_pFakeItems.push_back(new FakeItem);
	m_pFakeItems.back()->Init(pos, this);
}

void RaceScene::SpawnEffectExplode(glm::vec3 pos)
{
	m_pEffectExplodes.push_back(new EffectDestroy);
	m_pEffectExplodes.back()->Init(pos);
}

void RaceScene::SpawnEffectAccelDust(glm::vec3 pos)
{
	m_pAccelDusts.push_back(new AccelDust);
	m_pAccelDusts.back()->Init(pos);
}

void RaceScene::SpawnEffectItemGetSparkle(glm::vec3 pos, glm::vec4 color)
{
	m_pItemGetEffects.push_back(new CitemGetEffect);
	m_pItemGetEffects.back()->Init(pos, color);
}

void RaceScene::SpawnEffectSpeedUp(glm::vec3 pos, float yRot, float speed)
{
	GameEngine& game = GameEngine::Instance();

	m_pSpeedUpEffects.push_back(new SpeedUpEffect);
	glm::vec3 vel = pos - calcPosition(pos, yRot + game.GetRandomFloat(50, -50), game.GetRandomFloat(50, 0), 10);
	m_pSpeedUpEffects.back()->Init(pos, glm::vec4(1, 0, 0, 0.8), vel);
}

void RaceScene::SpawnEffectRespawn(glm::vec3 pos, float lifeTime)
{
	m_pRespawnEffects.push_back(new RespawnEffect);
	m_pRespawnEffects.back()->Init(pos, lifeTime);
}

bool RaceScene::BestTimeCheck(float Time)
{
	FILE *fp = fopen(bestTimeFileName, "wb");//バイナリファイルを開く
	if (fp == NULL) {//エラーが起きたらNULLを返す
		return false;
	}
	fwrite(&Time, sizeof(Time), 1, fp); // SaveData_t構造体の中身を出力
	fclose(fp);

	return true;
}

void RaceScene::OptionCommand(bool isAdd)
{
	if (isAdd) {
		this->resultCommand.IncreaseIndex();
	}
	else {
		this->resultCommand.DecreaseIndex();
	}
}

void RaceScene::ActOptionCommand()
{
	if (resultCommand.getIndex() == 0) {
		blackFilter.SetNextScene(NEXT_SCENE::InRace);

	}
	else if (resultCommand.getIndex() == 1) {
		blackFilter.SetNextScene(NEXT_SCENE::StageSelect);

	}
	else if (resultCommand.getIndex() == 2) {
		blackFilter.SetNextScene(NEXT_SCENE::Title);

	}
}

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

std::vector<Item*> RaceScene::GetFieldItems()
{
	return m_pItems;
}

std::vector<Obstacle*> RaceScene::getObstacles()
{
	return m_pObstacles;
}

void RaceScene::SpawnExplode(glm::vec3 pos)
{
	m_pExplodes.push_back(new Explosion);
	m_pExplodes.back()->Init(pos, this);
}

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

void RaceScene::Init()
{
	GameEngine& game = GameEngine::Instance();

	game.SetCameraNum(PlayerNum);

	isInitialized = true;

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

	game.AddEntity(0, glm::vec3(0, 0, 100), "Tree 1", "res/Model/Tree1.bmp", nullptr, true);

	std::vector<glm::vec3> pos;
	for (int i = 0; i < m_pGoals.size(); i++) {	
		pos.push_back(glm::vec3(m_pGoals[i]->Position()));
	}

	for (int i = 0; i < 4; i++) {
		m_pPlayerCharacters.push_back(new CPlayerCharacter);
		
		m_pPlayerIcons.push_back(new PlayerIcon);

		bool playerCheck = false;
		if (i < PlayerNum) {
			playerCheck = true;
		}
		m_pPlayerCharacters.back()->Init(glm::vec3(i * 5, 0, 0), m_pGoals.size(), 1.5f, i, this, pos, playerCheck);

		m_pPlayerIcons.back()->Init(i, 4.0f, m_pPlayerCharacters[i]->Position());

		m_attackInformer[i].Init(100.0);
	}

	resultCommand.InitCommand(3);

	m_pausing = false;
	m_isInResult = false;
	CountDown = 6.0f;
	Time = 0.0f;

	m_resultWaitTime = 5.0f;
	m_showRanking = false;
	m_resultMover = game.GetWindowSize().x;

	game.PlayAudio(BGM, CRI_SOUND_STAGEBGM2);
}

void RaceScene::EndFunc()
{
	GameEngine& game = GameEngine::Instance();

	this->SaveBestLap();

	int num;

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		m_pPlayerCharacters[i]->DestroyEntity();
	}

	DeleteAll(m_pPlayerCharacters);

	for (int i = 0; i < m_pItems.size(); i++) {
		m_pItems[i]->Destroy();
	}

	DeleteAll(m_pItems);

	for (int i = 0; i < m_pObstacles.size(); i++) {
		m_pObstacles[i]->Entity()->Destroy();
	}
	DeleteAll(m_pObstacles);

	for (int i = 0; i < m_pGoals.size(); i++) {
		m_pGoals[i]->Destroy();
	}
	DeleteAll(m_pGoals);

	for (int i = 0; i < m_pMissiles.size(); i++) {
		m_pMissiles[i]->Destroy();
	}
	DeleteAll(m_pMissiles);

	for (int i = 0; i < m_pElectroTraps.size(); i++) {
		m_pElectroTraps[i]->Destroy();
	}
	DeleteAll(m_pElectroTraps);

	for (int i = 0; i < m_pBombs.size(); i++) {
		m_pBombs[i]->Destroy();
	}
	DeleteAll(m_pBombs);

	for (int i = 0; i < m_pFakeItems.size(); i++) {
		m_pFakeItems[i]->Destroy();
	}
	DeleteAll(m_pFakeItems);

	for (int i = 0; i < m_pNukes.size(); i++) {
		m_pNukes[i]->Destroy();
	}
	DeleteAll(m_pNukes);

	for (int i = 0; i < m_pSmokes.size(); i++) {
		m_pSmokes[i]->Destroy();
	}
	DeleteAll(m_pSmokes);

	for (int i = 0; i < m_pExplodes.size(); i++) {
		m_pExplodes[i]->Destroy();
	}
	DeleteAll(m_pExplodes);

	for (int i = 0; i < m_pEffectExplodes.size(); i++) {
		m_pEffectExplodes[i]->Destroy();
	};
	DeleteAll(m_pEffectExplodes);

	DeleteAll(m_pAccelDusts);
	DeleteAll(m_pItemGetEffects);
	DeleteAll(m_pSpeedUpEffects);
	DeleteAll(m_pRespawnEffects);

	DeleteAll(m_pPlayerIcons);

	game.RemoveGroundTexture();

	game.StopAllSound();
}

void RaceScene::UpdateInGame(float delta)
{
	GameEngine& game = GameEngine::Instance();

	this->PlayerUpdate(delta);

	this->CheckPoint();

	this->ItemUpdate(delta);

	this->UpdateEffects(delta);

	this->CheckAllPlayerFinished();

	this->CollisionChecks();

	this->RankingCheck();

	this->ShowUI(delta);

	Time += delta;
}

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
		cameraPos[i] = calcPosition(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Yrot() + 180, 15, 20 + adder);

		game.Camera({ { cameraPos[i] },{ m_pPlayerCharacters[i]->Position() },{ 0, 1, 0 } }, i);
	}

	this->ItemUpdate(delta);
	this->UpdateEffects(delta);
	this->RankingCheck();

	this->ShowUI(delta);
}

void RaceScene::UpdatePause(float delta)
{
	this->UpdateEffects(delta);
	this->m_pPlayerCharacters[PauseIndex]->inputOptionFunc(delta);
	this->RankingUI();

	this->OptionUI();
	this->ShowUI(delta);
}

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
		this->RankingUI();
		Time += delta;

	}
	else
	{
		if (!m_showRanking) {
			blackFilter.SetNextScene(NEXT_SCENE::Result);
		}
	}

	if (m_showRanking) {
		GameEngine& game = GameEngine::Instance();

		game.Camera({ {-20, 20, -100},{0, 0, 0}, {0, 1, 0} }, 0);

		if (!blackFilter.IsInAction()) {
			this->ShowResult(delta);

			if (m_resultMover <= 0) {
				this->m_pPlayerCharacters[0]->inputOptionFunc(delta);
				OptionUI();
			}
		}
	}
}

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

void RaceScene::WallSetPointToPoint(glm::vec3 from, glm::vec3 to, float space)
{
	GameEngine& game = GameEngine::Instance();

	float distance = glm::distance(from, to);

	for (float i = 0; distance > 0; distance -= space, i++) {
		double radian = atan2(to.x - from.x, to.z - from.z);
		double degree = glm::degrees(radian);

		m_pObstacles.push_back(new Obstacle);
		m_pObstacles.back()->Init(calcPosition(from, degree, 0.0f, i * space), space);
	}
}

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

void RaceScene::PlayerUpdate(float delta)
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		m_pPlayerCharacters[i]->Update(delta, Time);

		std::vector<glm::vec3> positions;

		for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
			positions.push_back(m_pPlayerCharacters[j]->Position());
		}

		m_pPlayerCharacters[i]->AutoDrive(delta, m_pPlayerCharacters);
		positions.clear();
	}
}

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

void RaceScene::UpdateEffects(float delta)
{
	for (int i = 0; i < m_pAccelDusts.size(); i++) {
		if (!m_pausing) {
			m_pAccelDusts[i]->Update(delta);
		}
		m_pAccelDusts[i]->Draw();

		if (!m_pAccelDusts[i]->ActiveCheck()) {
			Remove(m_pAccelDusts, i);
			i--;
		}
	}

	for (int i = 0; i < m_pItemGetEffects.size(); i++) {
		if (!m_pausing) {
			m_pItemGetEffects[i]->Update(delta);
		}
		m_pItemGetEffects[i]->Draw();

		if (!m_pItemGetEffects[i]->IsActive()) {
			Remove(m_pItemGetEffects, i);
			i--;
		}
	}

	for (int i = 0; i < m_pSpeedUpEffects.size(); i++) {
		if (!m_pausing) {
			m_pSpeedUpEffects[i]->Update(delta);
		}
		m_pSpeedUpEffects[i]->Draw();

		if (!m_pSpeedUpEffects[i]->IsActive()) {
			Remove(m_pSpeedUpEffects, i);
			i--;
		}
	}

	for (int i = 0; i < m_pRespawnEffects.size(); i++) {
		if (!m_pausing) {
			m_pRespawnEffects[i]->Update(delta);
		}
		m_pRespawnEffects[i]->Draw();

		if (!m_pRespawnEffects[i]->IsActive()) {

			SpawnEffectItemGetSparkle(m_pRespawnEffects[i]->Position(), glm::vec4(1, 1, 0, 1));
			SpawnEffectItemGetSparkle(m_pRespawnEffects[i]->Position(), glm::vec4(0, 1, 1, 1));

			Remove(m_pRespawnEffects, i);
			i--;

		}
	}

	for (int i = 0; i < m_pPlayerIcons.size(); i++) {
		m_pPlayerIcons[i]->Update(m_pPlayerCharacters[i]->Position());
	}
}

void RaceScene::DeleteCheck()
{
	for (int i = 0; i < m_pMissiles.size(); i++) {
		if (!m_pMissiles[i]->IsActive()) {
			Remove(m_pMissiles, i);
			i--;
		}
	}

	for (int i = 0; i < m_pElectroTraps.size(); i++) {
		if (!m_pElectroTraps[i]->IsActive()) {
			Remove(m_pElectroTraps, i);
			i--;
		}
	}

	for (int i = 0; i < m_pNukes.size(); i++) {
		if (!m_pNukes[i]->IsActive()) {
			Remove(m_pNukes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pSmokes.size(); i++) {
		if (!m_pSmokes[i]->IsActive()) {
			Remove(m_pSmokes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pBombs.size(); i++) {
		if (!m_pBombs[i]->IsActive()) {
			Remove(m_pBombs, i);
			i--;
		}
	}

	for (int i = 0; i < m_pFakeItems.size(); i++) {
		if (!m_pFakeItems[i]->IsActive()) {
			Remove(m_pFakeItems, i);
			i--;
		}
	}

	for (int i = 0; i < m_pExplodes.size(); i++) {
		if (!m_pExplodes[i]->IsActive()) {
			Remove(m_pExplodes, i);
			i--;
		}
	}

	for (int i = 0; i < m_pEffectExplodes.size(); i++) {
		if (!m_pEffectExplodes[i]->IsActive()) {
			Remove(m_pEffectExplodes, i);
			i--;
		}
	}
}

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

void RaceScene::CollisionChecks()
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		
		for (int j = 0; j < m_pItems.size(); j++) {
			int itemId = this->m_pItems[j]->CollisionCheck(m_pPlayerCharacters[i]->Position(), m_pPlayerCharacters[i]->Radius());

			if (itemId != -1) {
				this->m_pPlayerCharacters[i]->ObtainItem(itemId);
				SpawnEffectItemGetSparkle(m_pItems[j]->Position(), glm::vec4(0, 1, 1, 0.5f));
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

void RaceScene::CheckAllPlayerFinished()
{
	for (int i = 0; i < m_pPlayerCharacters.size(); i++) {
		if (!m_pPlayerCharacters[i]->IsAutoDrive()) {
			return;
		}

		if (m_pPlayerCharacters.size() - 1 == i) {
			this->m_isInResult = true;
			for (int j = 0; j < m_pPlayerCharacters.size(); j++) {
				if (!m_pPlayerCharacters[j]->GetFinishedRace()) {
					//while (!m_pPlayerCharacters[j]->GetFinishedRace())
					//{
					//	if (m_pPlayerCharacters[j]->GetPlayerLapTime().size() > 0) {
					//		float time = *m_pPlayerCharacters[j]->GetPlayerLapTime().begin();
					//	//	m_pPlayerCharacters[j]->PassGoal()
					//	}
					//}
				}
			}
		}
	}
}

void RaceScene::SaveBestLap()
{
	FILE* fp = fopen(bestTimeFileName, "rb");
	if (!fp) {
		return;
	}

	float num = 0.0f;

	while (fscanf(fp, "%f", &num) != EOF) {
		
	}

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

void RaceScene::SceneChanger(float delta) 
{
	GameEngine& game = GameEngine::Instance();

	blackFilter.Update(delta);
	
	switch (blackFilter.NextScene())
	{
	case NEXT_SCENE::InRace:
		this->EndFunc();
		this->isInitialized = false;
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

void RaceScene::ShowUI(float delta)
{
	GameEngine& game = GameEngine::Instance();

	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	for (int i = 0; i < PlayerNum; i++) {
		
		game.FontColor(glm::vec4(1.0f));
		game.FontScale(glm::vec2(2.0f));
		game.AddString(glm::vec2(windowSize.x - 430.0f/*770.0f*/, 24.0f), "Lap", i);
		
		if (m_pPlayerCharacters[i]->GetLap() <= lapNum) {
			snprintf(str, 16, "%d", m_pPlayerCharacters[i]->GetLap());
			game.FontColor(glm::vec4(1, 1, 0, 1));
		}
		else {
			snprintf(str, 16, "%d", lapNum);
			game.FontColor(glm::vec4(1, 0, 0, 1));
		}
		game.AddString(glm::vec2(windowSize.x - 300.0f/*900.0f*/, 24.0f), str, i);

		game.FontColor(glm::vec4(1, 1, 1, 1));
		game.AddString(glm::vec2(windowSize.x - 225.0f/*975.0f*/, 24.0f), "/", i);

		snprintf(str, 16, "%d", lapNum);
		game.FontColor(glm::vec4(1, 1, 0, 1));
		game.AddString(glm::vec2(windowSize.x - 150.0f/*1050.0f*/, 24.0f), str, i);

		game.FontScale(glm::vec2(1.5f));
		game.FontColor(glm::vec4(1.0f));
		game.AddString(glm::vec2(windowSize.x - 280.0f/*920.0f*/, windowSize.y - 110.0f/*800.0f*/), "Speed", i);

		game.ImageColor(glm::vec4(0, 1, 0, 1));
		game.ImageScale(glm::vec2(0.75));
		game.AddImage(glm::vec2(windowSize.x - 310.0f, windowSize.y - 120.0f), "res/Texture/SpeedMeter.dds", i);

		snprintf(str, 16, "%06.2f", m_pPlayerCharacters[i]->Velocity());
		game.FontScale(glm::vec2(1.5f));
		game.FontColor(glm::vec4(1.0f));
		game.AddString(glm::vec2(windowSize.x - 280.0f/*920.0f*/, windowSize.y - 60.0f/*840.0f*/), str, i);
		
		game.ImageScale(glm::vec2(0.75));
		game.AddImage(glm::vec2(windowSize.x - 310.0f, windowSize.y - 70.0f), "res/Texture/SpeedMeter.dds", i);

		snprintf(str, 16, "%d Place", m_pPlayerCharacters[i]->GetRank());

		game.FontColor(glm::vec4(0, 0, 0, 1));
		game.FontScale(glm::vec2(2.0));
		game.AddString(glm::vec2(windowSize.x - 305.0f/*920.0f*/, windowSize.y - 195.0f/*720.0f*/), str, i);

		game.FontColor(rankColor[m_pPlayerCharacters[i]->GetRank()]);
		game.FontScale(glm::vec2(2.0));
		game.AddString(glm::vec2(windowSize.x - 310.0f/*920.0f*/, windowSize.y - 200.0f/*720.0f*/), str, i);

		this->RankingCheck();

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
			game.AddString(glm::vec2(24.0, windowSize.y - 180.0f/*750.0*/), "MIX", i);

			game.FontColor(glm::vec4(1, 1, 0, 1));
			game.AddString(glm::vec2(100.0, windowSize.y - 180.0f/*750.0*/), "L + R", i);
		}
		else {
			game.ImageColor(glm::vec4(0, 1, 1, 1));
		}

		this->ItemUI(glm::vec2(36.0, windowSize.y - 130.0f/*800.0*/), glm::vec2(1.0f), (ItemsCode)(m_pPlayerCharacters[i]->GetStockItemId()), i);
		game.ImageScale(glm::vec2(0.75f));
		game.AddImage(glm::vec2(8.0f, windowSize.y - 145.0f), "res/Texture/SpeedMeter.dds", i);

		this->ItemUI(glm::vec2(36.0, windowSize.y - 80.0f/*830.0*/), glm::vec2(2), (ItemsCode)(m_pPlayerCharacters[i]->GetItemId()), i);
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

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < m_pMissiles.size(); j++) {
			m_attackInformer[i].Update(i, m_pPlayerCharacters[i]->Position(), m_pMissiles[j]->Position(), m_pPlayerCharacters[i]->Yrot());
		}
	}
}

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

void RaceScene::DrawMapUi(int playerIndex)
{
	this->m_minimap.Update(m_pPlayerCharacters, playerIndex);
}

void RaceScene::RankingUI()
{
	GameEngine& game = GameEngine::Instance();
	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	for (int i = 0; i < PlayerNum; i++) {
		if (m_pPlayerCharacters[i]->GetLap() > this->lapNum) {
			game.FontColor(glm::vec4(1, 0, 0, 1));

			game.FontScale(glm::vec2(4.0));

			game.AddString(glm::vec2(windowSize.x * 0.2f/*200.0f*/, windowSize.y * 0.2 /*120.0f*/), "GOAL!", i);

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
			game.AddString(glm::vec2(windowSize.x * 0.3f/*300.0f*/, windowSize.y * 0.4 /*300.0f*/), str, i);
		}
	}
}

void RaceScene::OptionUI()
{
	GameEngine& game = GameEngine::Instance();

	glm::vec2 windowSize = game.GetWindowSize();

	char str[16];

	if (m_pausing) {
		snprintf(str, 16, "Player%d pause", PauseIndex + 1);
		game.AddString(glm::vec2(windowSize.x * 0.2f, windowSize.y * 0.2f /*400, 100*/), str, 0, true);
	}

	if (resultCommand.getIndex() == 0) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(/*300*/ windowSize.x * 0.4, /*600*/windowSize.y -300.0f), "Retry", 0, true);
	game.ImageScale(glm::vec2(1.2, 1));
	game.AddImage(glm::vec2(/*250*/ windowSize.x * 0.4 - 50, windowSize.y -325.0f), "res/Texture/SpeedMeter.dds", 0, true);

	if (resultCommand.getIndex() == 1) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(/*300*/ windowSize.x * 0.4, /*700*/ windowSize.y - 200.0f), "Stage Select", 0, true);
	game.AddImage(glm::vec2(/*250*/ windowSize.x * 0.4 - 50, windowSize.y - 225.0f), "res/Texture/SpeedMeter.dds", 0 ,true);

	if (resultCommand.getIndex() == 2) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(/*300*/ windowSize.x * 0.4, /*800*/ windowSize.y - 100.0f), "Back To Title", 0, true);
	game.AddImage(glm::vec2(/*250*/ windowSize.x * 0.4 - 50, windowSize.y - 125.0f), "res/Texture/SpeedMeter.dds", 0, true);
}

void RaceScene::ShowResult(float delta)
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
		game.AddString(glm::vec2(400 + m_resultMover, 50 + m_pPlayerCharacters[i]->GetRank() * 100), str, 0, true);

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