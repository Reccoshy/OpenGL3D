#include "Effects.h"
#include "../../../GameEngine.h"
#include <vector>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>

/*
エフェクト描画の初期化処理.

@param size	ウィンドウの画面サイズ.
*/
void Effect::Init(glm::vec2 size)
{
	this->width = size.x;
	this->height = size.y;

	program = GameEngine::Instance().checkProgram("Effect");
}

/*
エフェクトの追加処理.

@param pos			エフェクトを表示させる位置.
@param s			エフェクト用の画像サイズ.
@param color		エフェクトの色.
@param imageName	エフェクトに使用する画像名.
*/
void Effect::AddEffect(glm::vec3 pos, glm::vec2 s, glm::vec4 color, const char* imageName)
{
	GameEngine& game = GameEngine::Instance();

	TexturePtr tex = game.GetTexture(imageName);

	if (!tex) {
		std::cout << "テクスチャの読み込み失敗" << imageName << std::endl;

		return;
	}
	textures.push_back(tex);

	m_effectNum++;

	positions.push_back(pos);
	scale.push_back(s);

	glm::u8vec4 imageColor = glm::clamp(color, 0.0f, 1.0f) * 255.0f;

	struct effectInfo
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::u8vec4 c;
	}effects[6];

	effects[0].position = glm::vec3(scale[m_effectNum - 1].x / 2, scale[m_effectNum - 1].y / 2, 0);
	effects[0].uv = glm::vec2(0, 0);
	effects[1].position = glm::vec3(scale[m_effectNum - 1].x / 2, -scale[m_effectNum - 1].y / 2, 0);
	effects[1].uv = glm::vec2(0, 1);
	effects[2].position = glm::vec3(-scale[m_effectNum - 1].x / 2, scale[m_effectNum - 1].y / 2, 0);
	effects[2].uv = glm::vec2(1, 0);
	effects[3].position = glm::vec3(-scale[m_effectNum - 1].x / 2, scale[m_effectNum - 1].y / 2, 0);
	effects[3].uv = glm::vec2(1, 0);
	effects[4].position = glm::vec3(scale[m_effectNum - 1].x / 2, -scale[m_effectNum - 1].y / 2, 0);
	effects[4].uv = glm::vec2(0, 1);
	effects[5].position = glm::vec3(-scale[m_effectNum - 1].x / 2, -scale[m_effectNum - 1].y / 2, 0);
	effects[5].uv = glm::vec2(1, 1);

	for (int i = 0; i < 6; i++) {
		effects[i].c = imageColor;
	}

	GLuint VAO;
	GLuint VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(effects), &effects, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(effectInfo), (GLvoid *)offsetof(effectInfo, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(effectInfo), (GLvoid*)offsetof(effectInfo, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(effectInfo), (GLvoid*)offsetof(effectInfo, c));
	glBindVertexArray(0);

	effectsVao.push_back(VAO);
	effectsVbo.push_back(VBO);
}

/*
エフェクトの画像の描画処理.

@param	index	エフェクトを描画するカメラのインデックス.
*/
void Effect::Draw(int index)
{
	GameEngine& game = GameEngine::Instance();
	GameEngine::CameraData camera = game.GetCameraInfo(index);

	program->UseProgram();

	for (int i = 0; i < m_effectNum; i++) {

		double radianFlat = atan2(camera.position.x - positions[i].x, camera.position.z - positions[i].z);
		float degreeFlat = glm::degrees(radianFlat);
		glm::quat q = glm::quat(glm::vec3(0, glm::radians(degreeFlat + 180), 0));

		float distance = glm::distance(camera.position, positions[i]);
		float sizeRate = 1 / distance;
		glm::vec3 scale = glm::vec3(sizeRate);

		glDepthMask(false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		const glm::mat4 t = glm::translate(glm::mat4(), positions[i]);
		const glm::mat4 r = glm::mat4_cast(q);
		const glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(scale));
		glm::mat4 matM = t * r * s;

		const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f),
			static_cast<float>(width) / static_cast<float> (height), 1.0f, 1000.0f);
		const glm::mat4x4 matView = glm::lookAt(camera.position, camera.target, camera.up);

		glm::mat4 matMVP = matProj * matView * matM;

		glUniformMatrix4fv(glGetUniformLocation(program->getProgram(), "matMVP"), 1, GL_FALSE, &matMVP[0][0]);

		glBindVertexArray(effectsVao[i]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[i]->Id());
		glUniform1i(glGetUniformLocation(program->getProgram(), "colorSampler"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	glDepthMask(true);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

/*
終了処理. 画像の情報をすべて消去する.
*/
void Effect::EndClear()
{
	for (int i = 0; i < m_effectNum; i++) {
		glDeleteVertexArrays(1, &effectsVao[i]);
		glDeleteBuffers(1, &effectsVbo[i]);
	}

	m_effectNum = 0;

	positions.clear();
	scale.clear();
	textures.clear();
	
	effectsVao.clear();
	effectsVbo.clear();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}