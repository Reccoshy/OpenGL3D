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
	//画面の幅.
	int width = 0;
	//画面の高さ.
	int height = 0;
	//表示するエフェクトの数.
	int m_effectNum = 0;
	
	//画像の表示をする位置のベクター
	std::vector<glm::vec3> positions;
	//画像の表示する拡大率のベクター.
	std::vector<glm::vec2> scale;
	//画像のテクスチャのベクター.
	std::vector<TexturePtr> textures;
	
	std::vector<int> showIndex;

	//エフェクトのVBOのベクター.
	std::vector<GLuint> effectsVbo;
	//エフェクトのVAOのベクター.
	std::vector<GLuint> effectsVao;

	//エフェクトに使用するプログラム.
	Shader::ProgramPtr program = 0;
};