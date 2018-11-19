#include "TitleScene.h"
#include "../../../GameEngine.h"
#include "../../../res/Sound/Sound.h"
#include "../StageSelectScene/StageSelectScene.h"
#include "../RaceScene.h"

void TitleScene::operator()(double delta)
{
	if (!isInitialized) {
		this->Init();
	}

	this->Update(delta);
}

bool TitleScene::Init()
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
	game.PlayAudio(BGM, CRI_SOUND_TITLEBGM);

	this->commandSelect.InitCommand(2);

	blackFilter.Init();

	m_pPlayerCharacter = game.AddEntity(0, glm::vec3(0, 0, 0), "Aircraft", "res/Model/Player.bmp", nullptr);

	game.Camera({ { 3, 3, -8 },{ 2, 1, 0 },{ 0, 1, 0 } }, 0);
	
	return true;
}

void TitleScene::Update(float delta)
{
	GameEngine& game = GameEngine::Instance();

	CamY += delta;

	if (CamY > 360) {
		CamY -= 360;
	}

	this->m_pPlayerCharacter->Rotation(glm::quat(glm::vec3(0, CamY, 0)));

	this->InputFunc();
	this->ShowTextUI();

	this->SceneChanger(delta);
}

void TitleScene::EndFunc()
{
	GameEngine& game = GameEngine::Instance();

	m_pPlayerCharacter->Destroy();

	game.StopAllSound();
}

void TitleScene::InputFunc()
{
	GameEngine& game = GameEngine::Instance();

	if (blackFilter.IsInAction()) {
		return;
	}

	GamePad gamepad = game.GetGamePad(0);

	if (gamepad.buttonDown & GamePad::A || gamepad.buttonDown & GamePad::START) {

		game.PlayAudio(SEUI, CRI_SOUND_DECISION);

		if (commandSelect.getIndex() == 0) {
			blackFilter.SetNextScene(NEXT_SCENE::StageSelect);
		}
		else if (commandSelect.getIndex() == 1) {
			
			blackFilter.SetNextScene(NEXT_SCENE::EndGame);
		}
	}

	if (gamepad.buttonDown & GamePad::DPAD_UP) {

		this->commandSelect.DecreaseIndex();
	}

	if (gamepad.buttonDown & GamePad::DPAD_DOWN) {

		this->commandSelect.IncreaseIndex();
	}
}

void TitleScene::ShowTextUI()
{
	GameEngine& game = GameEngine::Instance();

	game.FontColor(glm::vec4(0.5, 0, 1,1));
	game.FontScale(glm::vec2(5));
	game.AddString(glm::vec2(60,60),"Elemental");

	game.FontColor(glm::vec4(0.5, 0, 1, 1));
	game.FontScale(glm::vec2(5));
	game.AddString(glm::vec2(60, 200), "Driver");

	game.ImageScale(glm::vec2(1));
	if (commandSelect.getIndex() == 0) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(300,500), "start");

	game.AddImage(glm::vec2(250, 475), "res/Texture/SpeedMeter.dds");

	if (commandSelect.getIndex() == 1) {
		game.FontColor(glm::vec4(1, 0, 0, 1));
		game.ImageColor(glm::vec4(1, 1, 0, 1));
	}
	else {
		game.FontColor(glm::vec4(1));
		game.ImageColor(glm::vec4(0.2, 0.2, 0.2, 1));
	}
	game.FontScale(glm::vec2(2));
	game.AddString(glm::vec2(300,600), "end");

	game.AddImage(glm::vec2(250, 575), "res/Texture/SpeedMeter.dds");
}

void TitleScene::SceneChanger(float delta)
{
	GameEngine& game = GameEngine::Instance();

	blackFilter.Update(delta);

	switch (blackFilter.NextScene())
	{
	case StageSelect:
		this->EndFunc();
		game.UpdateFunc(StageSelectScene());
		break;

	case EndGame:
		this->EndFunc();
		game.EndGame();
		break;
	default:
		break;
	}
}
