#include "BlackFilter.h"
#include "../../../../GameEngine.h"

/*
暗転処理の初期化.
*/
bool BlackFilter::Init()
{
	m_sceneOpening = true;
	m_sceneChanging = false;
	m_changeNow = false;

	m_UIalpha = 1.0f;
	m_waitTime = 1.0f;

	return true;
}

/*
更新処理.

@param	delta 次の更新までの時間.
*/
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
		}
	}

	return;
}

/*
次に移動するシーンを設定する.

@param	scene	次のシーン.
*/
void BlackFilter::SetNextScene(NEXT_SCENE scene)
{
	this->m_sceneChanging = true;
	this->m_nextScene = scene;
}

/*
次に移動するシーンを返し、それを確認しシーンを移動させる.

@return	NEXT_SCENE
*/
NEXT_SCENE BlackFilter::NextScene()
{
	if (m_changeNow) {
		return m_nextScene;
	}

	return NEXT_SCENE::None;
}

/*
暗転処理実行中かどうかを返す.

@return	暗転処理が実行中.
*/
bool BlackFilter::IsInAction()
{
	return m_sceneChanging || m_sceneOpening;
}

/*
設定されている画像をすべて描画.
*/
void BlackFilter::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.FrontImageScale(glm::vec2(4));
	game.FrontImageColor(glm::vec4(0, 0, 0, m_UIalpha));
	game.FrontAddImage(glm::vec2(0, 0), "res/Model/Player.bmp", 0, true);
}
