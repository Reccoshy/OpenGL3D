#include "BlackFilter.h"
#include "../../../../GameEngine.h"

/*
�Ó]�����̏�����.
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
�X�V����.

@param	delta ���̍X�V�܂ł̎���.
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
���Ɉړ�����V�[����ݒ肷��.

@param	scene	���̃V�[��.
*/
void BlackFilter::SetNextScene(NEXT_SCENE scene)
{
	this->m_sceneChanging = true;
	this->m_nextScene = scene;
}

/*
���Ɉړ�����V�[����Ԃ��A������m�F���V�[�����ړ�������.

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
�Ó]�������s�����ǂ�����Ԃ�.

@return	�Ó]���������s��.
*/
bool BlackFilter::IsInAction()
{
	return m_sceneChanging || m_sceneOpening;
}

/*
�ݒ肳��Ă���摜�����ׂĕ`��.
*/
void BlackFilter::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.FrontImageScale(glm::vec2(4));
	game.FrontImageColor(glm::vec4(0, 0, 0, m_UIalpha));
	game.FrontAddImage(glm::vec2(0, 0), "res/Model/Player.bmp", 0, true);
}
