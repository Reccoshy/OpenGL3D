#include "StageSelectScene.h"
#include "../../../GameEngine.h"
#include "../../../res/Sound/Sound.h"
#include "../TitleScene/TitleScene.h"
#include "../RaceScene.h"

void StageSelectScene::operator()(double delta)
{
	if (!isInitialized) {
		this->Init();
	}

	this->Update(delta);
}

bool StageSelectScene::Init()
{
	isInitialized = true;

	GameEngine& game = GameEngine::Instance();

	game.AmbientLight(glm::vec4(0.005f, 0.1f, 0.2f, 1));
	game.Light(0, { { 120,200,0,1 },{ 24000,24000,24000,1 } });
	game.Light(1, { { 550,200,0,1 },{ 24000,24000,24000,1 } });
	game.Light(2, { { 250,200,500,1 },{ 24000,24000,24000,1 } });
	game.Light(3, { { 400,200,-200,1 },{ 24000,24000,24000,1 } });
	game.Light(4, { { 320,200,750,1 },{ 24000,24000,24000,1 } });
	game.SetCameraNum(1);

	game.SetCubeMap("CubeMap");

	game.Camera({ { 3, 3, -8 },{ 2, 1, 0 },{ 0, 1, 0 } }, 0);

	m_stageSelect.InitCommand(2);
	m_playerNum.InitCommand(4);
	m_lapSelect.InitCommand(99, 2);

	this->LoadStageData();

	return true;
}

void StageSelectScene::Update(float delta)
{
	this->InputFunc();

	this->ShowTextUI(delta);

	this->SceneChanger(delta);

	this->m_time += delta;
}

void StageSelectScene::EndFunc()
{
	GameEngine& game = GameEngine::Instance();

	game.StopAllSound();
}

bool StageSelectScene::LoadStageData()
{
	const char* filename = "res/Data/StageLoader.txt";

	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	int line = 0;
	int repeat = 6;

	char text[100] = " ";
	int getLine = m_stageSelect.getIndex();

	while (fscanf(fp, "%s", text) != EOF)
	{
		if (line == getLine * repeat) {

			for (int i = 0; i < 100; i++) {
				this->scoreFile[i] = text[i];
			}
			snprintf(scoreFile, 100, "%s",text);
			this->LoadHighScore();
		}
		else if (line == getLine * repeat + 1) {
			for (int i = 0; i < 100; i++) {
				this->stageName[i] = text[i];
			}
		}
		else if (line == getLine * repeat + 2) {
			for (int i = 0; i < 100; i++) {
				this->stageDataTexfile[i] = text[i];
			}
		}
		else if (line == getLine * repeat + 3) {
			for (int i = 0; i < 100; i++) {
				this->checkPointTexfile[i] = text[i];
			}
		}
		else if (line == getLine * repeat + 4) {
			for (int i = 0; i < 100; i++) {
				this->itemTexfile[i] = text[i];
			}
		}
		else if (line == getLine * repeat + 5) {
			for (int i = 0; i < 100; i++) {
				this->texFile[i] = text[i];
			}
			break;
		}
		line++;
	}

	fclose(fp);

	return true;
}

bool StageSelectScene::LoadHighScore()
{
	FILE* fp = fopen(scoreFile, "rb");
	if (!fp) {
		return false;
	}

	float num = 0.0f;

	while (fscanf(fp, "%f", &num) != EOF) {
		m_bestLap = num;
	}

	return true;
}

void StageSelectScene::InputFunc()
{
	if (m_activeInput && !blackFilter.IsInAction()) {

		GameEngine& game = GameEngine::Instance();

		GamePad gamepad = game.GetGamePad(0);

		if (m_menu == ShowingMenu::PlayerSelect) {

			if (gamepad.buttonDown & GamePad::DPAD_RIGHT) {
				if (m_playerSelecrMenuIndex == PlayerNumSelect) {
					m_playerNum.IncreaseIndex();
				}
				else {
					m_lapSelect.IncreaseIndex();
				}
			}
			else if (gamepad.buttonDown & GamePad::DPAD_LEFT) {
				if (m_playerSelecrMenuIndex == PlayerNumSelect) {
					m_playerNum.DecreaseIndex();
				}
				else {
					m_lapSelect.DecreaseIndex();
				}
			}

			if (gamepad.buttonDown & GamePad::DPAD_DOWN || gamepad.buttonDown & GamePad::DPAD_UP) {
				if (m_playerSelecrMenuIndex == PlayerNumSelect) {
					m_playerSelecrMenuIndex = LapNumSelect;
				}
				else if (m_playerSelecrMenuIndex == LapNumSelect) {
					m_playerSelecrMenuIndex = PlayerNumSelect;
				}
			}

			if (gamepad.buttonDown & GamePad::A || gamepad.buttonDown & GamePad::START) {
				this->m_menu = ShowingMenu::StageSelect;
				this->m_activeInput = false;
				game.PlayAudio(SEUI, CRI_SOUND_DECISION);
			}

			if (gamepad.buttonDown & GamePad::B) {

				blackFilter.SetNextScene(NEXT_SCENE::Title);
				this->m_activeInput = false;
				game.PlayAudio(SEUI, CRI_SOUND_CANCEL);
			}
		}
		else if (m_menu == ShowingMenu::StageSelect) {

			if (gamepad.buttonDown & GamePad::DPAD_LEFT) {
				m_stageSelect.DecreaseIndex();
				this->LoadStageData();

			}
			else if (gamepad.buttonDown & GamePad::DPAD_RIGHT) {
				m_stageSelect.IncreaseIndex();
				this->LoadStageData();
			}

			if (gamepad.buttonDown & GamePad::A || gamepad.buttonDown & GamePad::START) {
				this->m_menu = ShowingMenu::ReadyCheck;
				this->m_activeInput = false;
				game.PlayAudio(SEUI, CRI_SOUND_DECISION);
			}

			if (gamepad.buttonDown & GamePad::B) {
				m_menu = ShowingMenu::PlayerSelect;
				this->m_activeInput = false;
				game.PlayAudio(SEUI, CRI_SOUND_CANCEL);
			}
		}
		else if (m_menu == ShowingMenu::ReadyCheck) {
			if (gamepad.buttonDown & GamePad::A || gamepad.buttonDown & GamePad::START) {
				this->m_activeInput = false;
				blackFilter.SetNextScene(NEXT_SCENE::InRace);
				game.PlayAudio(SEUI, CRI_SOUND_DECISION);
			}

			if (gamepad.buttonDown & GamePad::B) {
				this->m_activeInput = false;
				m_menu = ShowingMenu::StageSelect;
				game.PlayAudio(SEUI, CRI_SOUND_CANCEL);
			}
		}
	}
}

void StageSelectScene::ShowTextUI(float delta)
{
	GameEngine& game = GameEngine::Instance();
	glm::vec2 windowSize = game.GetWindowSize();

	if ((float)m_menu * -windowSize.x * 0.1 == m_uIAdder) {
		m_activeInput = true;
	}

	if (m_uIAdder < (float)m_menu * -windowSize.x * 0.1) {
		m_uIAdder += windowSize.x * 0.01;
	}
	else if (m_uIAdder > (float)m_menu * -windowSize.x * 0.1) {
		m_uIAdder -= windowSize.x * 0.01;
	}

	float arrow = (int)(m_time * 50) % 20;

	char str[32];
	if (m_playerSelecrMenuIndex == PlayerNumSelect) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
	}
	snprintf(str, 32, "Number of player");

	game.ImageColor(glm::vec4(1));
	game.ImageScale(glm::vec2(0.5));

	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(100 + m_uIAdder, 200), str);

	snprintf(str, 32, "%d", m_playerNum.getIndex() + 1);
	game.FontColor(glm::vec4(1, 1, 0, 1));
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(900 + m_uIAdder, 200), str);
	if (m_playerSelecrMenuIndex == PlayerNumSelect) {
		game.AddImage(glm::vec2(800 + m_uIAdder - arrow, 190), "res/Texture/ArrorL.dds");
		game.AddImage(glm::vec2(950 + m_uIAdder + arrow, 190), "res/Texture/ArrorR.dds");
	}

	if (m_playerSelecrMenuIndex == LapNumSelect) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
	}
	else
		game.FontColor(glm::vec4(1));
	snprintf(str, 32, "Number of lap");
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(100 + m_uIAdder, 600), str);

	snprintf(str, 32, "%d", m_lapSelect.getIndex() + 1);
	game.FontColor(glm::vec4(1, 1, 0, 1));
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(900 + m_uIAdder, 600), str);
	if (m_playerSelecrMenuIndex == LapNumSelect) {
		game.AddImage(glm::vec2(800 + m_uIAdder - arrow, 590), "res/Texture/ArrorL.dds");
		game.AddImage(glm::vec2(950 + m_uIAdder + arrow, 590), "res/Texture/ArrorR.dds");
	}
	
	snprintf(str, 32, "%d: %s", m_stageSelect.getIndex(), stageName);
	game.FontColor(glm::vec4(1));
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(windowSize.x + 250/*1400*/ + m_uIAdder, 200), str);
	game.AddImage(glm::vec2(windowSize.x + 100 + m_uIAdder - arrow, 190), "res/Texture/ArrorL.dds");
	game.AddImage(glm::vec2(windowSize.x + 950 + m_uIAdder + arrow, 190), "res/Texture/ArrorR.dds");

	float second = m_bestLap;

	int minute = 0.0;
	for (int k = 0; second >= 60; k++) {
		second -= 60;
		minute += 1;
	}
	snprintf(str, 32, "Best Lap Time : %d:%05.2f", minute, second);
	game.FontColor(glm::vec4(1, 1, 0, 1));
	game.AddString(glm::vec2(windowSize.x + 100/*1400*/ + m_uIAdder, windowSize.y - 200/*800*/), str);


	snprintf(str, 32, "%d: %s", m_stageSelect.getIndex(), stageName);
	game.FontColor(glm::vec4(1));
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(windowSize.x * 2 + 100/*2500*/ + m_uIAdder, 200), "PRESS A TO CONTINUE");

}

void StageSelectScene::SceneChanger(float delta)
{
	GameEngine& game = GameEngine::Instance();

	blackFilter.Update(delta);
	
	switch (blackFilter.NextScene())
	{
	case None:
		break;

	case Title:
		this->EndFunc();
		game.UpdateFunc(TitleScene());
		break;

	case InRace:
	{
		this->EndFunc();
		int num = m_playerNum.getIndex();
		int lap = m_lapSelect.getIndex();
		game.UpdateFunc(RaceScene(scoreFile, stageDataTexfile, checkPointTexfile, itemTexfile, texFile, num, lap));
		break;
	}
	default:
		break;
	}

}