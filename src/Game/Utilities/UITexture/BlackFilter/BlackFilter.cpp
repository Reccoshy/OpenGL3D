#include "BlackFilter.h"
#include "../../../../GameEngine.h"

bool BlackFilter::Init()
{
	m_sceneOpening = true;
	m_sceneChanging = false;
	m_changeNow = false;

	m_UIalpha = 1.0f;
	m_waitTime = 1.0f;

	return true;
}

void BlackFilter::Update(float delta)
{
	if (m_sceneChanging) {
		if (m_UIalpha < 1.0f) {
			m_UIalpha += delta;
			if (m_UIalpha >= 1.0f) {
				m_UIalpha = 1.0f;
			}
		}
	}
	else {
		if (m_UIalpha > 0.0f) {
			m_UIalpha -= delta;
			if (m_UIalpha < 0.0f) {
				m_UIalpha = 0.0f;
				m_sceneOpening = false;
			}
		}
	}

	this->Draw();

	if (m_sceneChanging && m_UIalpha >= 1.0f) {
		if (m_waitTime >= 0.0f) {
			m_waitTime -= delta;
		}

		else {

			m_changeNow = true;

			//if (m_nextScene == FromRaceSceneToNextScene::Retry) {
			//	this->EndFunc();
			//	this->isInitialized = false;
			//}
			//if (m_nextScene == FromRaceSceneToNextScene::ToTitleScene) {
			//	this->EndFunc();
			//	game.UpdateFunc(TitleScene());
			//}
			//else if (m_nextScene == FromRaceSceneToNextScene::ToStageSelectScene) {
			//	this->EndFunc();
			//	game.UpdateFunc(StageSelectScene());
			//}
			//else if (m_nextScene == FromRaceSceneToNextScene::ShowResultTime) {
			//	this->m_sceneChanging = false;
			//	this->m_nextScene = FromRaceSceneToNextScene::NONE;
			//	this->m_showRanking = true;
			//	game.SetCameraNum(1);
			//}
		}
	}

	return;
}

void BlackFilter::SetNextScene(NEXT_SCENE scene)
{
	this->m_sceneChanging = true;
	this->m_nextScene = scene;
}

NEXT_SCENE BlackFilter::NextScene()
{
	if (m_changeNow) {
		return m_nextScene;
	}

	return NEXT_SCENE::None;
}

bool BlackFilter::IsInAction()
{
	return m_sceneChanging || m_sceneOpening;
}

void BlackFilter::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.FrontImageScale(glm::vec2(4));
	game.FrontImageColor(glm::vec4(0, 0, 0, m_UIalpha));
	game.FrontAddImage(glm::vec2(0, 0), "res/Model/Player.bmp", 0, true);
}
