#pragma once
#include <glm/glm.hpp>
#include "../../../BufferObject.h"
#include "../../../Shader.h"
#include <glew.h>
#include <vector>
#include "../../../Texture.h"

class GameEngine;
struct CameraData;

class Effect {
public:

	void Init(glm::vec2 size);

	void AddEffect(glm::vec3 pos, glm::vec2 s, glm::vec4 color, const char* imageName);

	void Draw(int index);

	void EndClear();

private:
	//��ʂ̕�.
	int width = 0;
	//��ʂ̍���.
	int height = 0;
	//�\������G�t�F�N�g�̐�.
	int m_effectNum = 0;
	
	//�摜�̕\��������ʒu�̃x�N�^�[
	std::vector<glm::vec3> positions;
	//�摜�̕\������g�嗦�̃x�N�^�[.
	std::vector<glm::vec2> scale;
	//�摜�̃e�N�X�`���̃x�N�^�[.
	std::vector<TexturePtr> textures;
	
	std::vector<int> showIndex;

	//�G�t�F�N�g��VBO�̃x�N�^�[.
	std::vector<GLuint> effectsVbo;
	//�G�t�F�N�g��VAO�̃x�N�^�[.
	std::vector<GLuint> effectsVao;

	//�G�t�F�N�g�Ɏg�p����v���O����.
	Shader::ProgramPtr program = 0;
};