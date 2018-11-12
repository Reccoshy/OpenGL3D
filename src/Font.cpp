﻿/*
@file Font.cpp
*/
#include "Font.h"
#include "GameEngine.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/*
フォント描画機能を格納する名前空間.
*/
namespace Font {

	/*
	フォント用頂点データ型.
	*/
	struct Vertex
	{
		glm::vec2 position;
		glm::u16vec2 uv;
		glm::u8vec4 color;
	};

	/*
	フォント描画オブジェクトを初期化する.

	@param maxChar	最大描画文字数(0~16384の間の数値).
	@param screen	描画先スクリーンの大きさ(ピクセル単位).
	@param fontSize	画面に表示されるフォントサイズ(ピクセル単位).

	@retval true	初期化成功.
	@retval false	初期化失敗.
	*/
	bool Renderer::Init(size_t maxChar, const glm::vec2& screen, glm::f32 fontSize)
	{
		if (maxChar > (USHRT_MAX + 1) / 4) {
			std::cerr << "WARNING: " << maxChar << "は設定可能な最大文字数を越えています" << std::endl;
			maxChar = (USHRT_MAX + 1) / 4;
		}
		vboCapacity = static_cast<GLsizei>(4 * maxChar);
		vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vboCapacity, nullptr, GL_STREAM_DRAW);
		{
			std::vector<GLushort> tmp;
			tmp.resize(maxChar * 6);
			GLushort* p = tmp.data();
			for (GLushort i = 0; i < maxChar * 4; i += 4) {
				for (GLushort n : { 0, 1, 2, 2, 3, 0 }) {
					*(p++) = i + n;
				}
			}
			ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * maxChar, tmp.data(),
				GL_STATIC_DRAW);
		}
		vao.Init(vbo.Id(), ibo.Id());
		vao.Bind();
		vao.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			offsetof(Vertex, position));
		vao.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, uv));
		vao.VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, color));
		vao.Unbind();

		progFont = Shader::Program::Create("Res/Font.vert", "Res/Font.frag");
		if (!progFont) {
			return false;
		}

		windowSize = screen;

		normalFontSize = fontSize;
		pixelSizeInClipCoord = 2.0f / screen;
		return true;
	}

	/*
	フォントファイルを読み込む.

	@param filename	ファイル名.

	@retval true	読み込み成功.
	@retval false	読み込み失敗.
	*/
	bool Renderer::LoadFromFile(const char* filename)
	{
		const std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename, "r"), fclose);
		if (!fp) {
			std::cerr << "ERROR: '" << filename << "'のオープンに失敗" << std::endl;
			return false;
		}

		int line = 1; ///< フォントファイルの処理中の行.
		float size;
		int ret = fscanf(fp.get(),
			"info face=%*s size=%f bold=%*d italic=%*d charset=%*s unicode=%*d"
			" stretchH=%*d smooth=%*d aa=%*d padding=%*d,%*d,%*d,%*d spacing=%*d,%*d", &size);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line <<
				")" << std::endl;
			return false;
		}
		baseScale = (normalFontSize / size) * pixelSizeInClipCoord;
		++line;

		glm::vec2 scale;
		ret = fscanf(fp.get(),
			" common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d",
			&scale.x, &scale.y);
		if (ret < 2) {
			std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line <<
				")" << std::endl;
			return false;
		}
		const glm::vec2 reciprocalScale(1.0f / scale);
		++line;

		char tex[128];
		ret = fscanf(fp.get(), " page id=%*d file=%127s", tex);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line <<
				")" << std::endl;
			return false;
		}
		texFilename = "Res/";
		texFilename += std::string(tex + 1, tex + strlen(tex) - 1);
		++line;

		int charCount;
		ret = fscanf(fp.get(), " chars count=%d", &charCount);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line <<
				")" << std::endl;
			return false;
		}
		++line;

		fontList.resize(128); // ASCIIフォントだけの場合128文字あれば十分のはず.
		for (int i = 0; i < charCount; ++i) {
			FontInfo font;
			glm::vec2 uv;
			ret = fscanf(fp.get(),
				" char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f"
				" page=%*d chnl=%*d", &font.id, &uv.x, &uv.y, &font.size.x, &font.size.y,
				&font.offset.x, &font.offset.y, &font.xadvance);
			if (ret < 8) {
				std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line <<
					")" << std::endl;
				return false;
			}
			font.offset.y *= -1;
			uv.y = scale.y - uv.y - font.size.y;
			font.uv[0] = uv * reciprocalScale * 65535.0f;
			font.uv[1] = (uv + font.size) * reciprocalScale * 65535.0f;
			if (font.id < 128) {
				fontList[font.id] = font;
			}
			++line;
		}
		// テクスチャを読み込む.
		if (!GameEngine::Instance().LoadTextureFromFile(texFilename.c_str())) {
			return false;
		}
		return true;
	}

	/*
	文字色を設定する.

	@param c文字色.
	*/
	void Renderer::Color(const glm::vec4& c) 
	{
		color = glm::clamp(c, 0.0f, 1.0f) * 255.0f;
	}

	/*
	文字色を取得する.

	@return 文字色.
	*/
	glm::vec4 Renderer::Color() const
	{
		return glm::vec4(color) * (1.0f / 255.0f);
	}

	/*
	文字列を追加する.

	@param position	表示開始座標(左上を原点とするピクセル単位).
	@param str		追加する文字列.

	@retval true	追加成功.
	@retval false	追加失敗.
	*/
	bool Renderer::AddString(const glm::vec2& position, const char* str, int CameraIndex, int cameraNum, bool SingleCamWrite)
	{
		glm::vec2 camOffset = glm::vec2(0);
		glm::vec2 camScale = glm::vec2(1);
		glm::vec2 posRatio = glm::vec2(1);

		if (!SingleCamWrite) {
			if (cameraNum >= 3) {
				camScale = glm::vec2(0.5f);

				camOffset = glm::vec2(windowSize.x * 0.5 * (CameraIndex % 2), windowSize.y * 0.5 * (CameraIndex / 2));
			}
			else if (cameraNum >= 2) {
				camScale = glm::vec2(0.5f);

				posRatio = glm::vec2(2, 1);
				camOffset = glm::vec2(0, windowSize.y * 0.5 * CameraIndex);
			}
		}

		// 左上を原点とするピクセル座標系からOpenGLのクリップ座標系へ変換.
		glm::vec2 pos = ((position * camScale * posRatio + camOffset) * pixelSizeInClipCoord -1.0f) * glm::vec2(1.0f, -1.0f);
		Vertex* p = pVBO + vboSize;
		for (const char* itr = str; *itr; ++itr) {
			if (vboSize + 4 > vboCapacity) { // VBOに空きがなければ終了.
				break;
			}
			const FontInfo& font = fontList[*itr];
			if (font.id >= 0 && font.size.x && font.size.y) { // 表示できないデータなら無視する.
				const glm::vec2 size = font.size * baseScale * scale * camScale;
				const glm::vec2 offsetedPos = pos + font.offset * baseScale * scale * camScale;
				p[0].position = offsetedPos + glm::vec2(-size.x / 2, -size.y / 2);
				p[0].uv = font.uv[0];
				p[0].color = color;
				p[1].position = offsetedPos + glm::vec2(size.x / 2, -size.y / 2);
				p[1].uv = glm::u16vec2(font.uv[1].x, font.uv[0].y);
				p[1].color = color;
				p[2].position = offsetedPos + glm::vec2(size.x / 2, size.y / 2);
				p[2].uv = font.uv[1];
				p[2].color = color;
				p[3].position = offsetedPos + glm::vec2(-size.x / 2, size.y / 2);
				p[3].uv = glm::u16vec2(font.uv[0].x, font.uv[1].y);
				p[3].color = color;
				p += 4;
				vboSize += 4;
			}
			pos.x += font.xadvance * baseScale.x * scale.x * camScale.x; // 次の文字の表示位置へ移動.
		}

		return true;
	}

	/*
	VBOをシステムメモリにマッピングする.
	*/
	void Renderer::MapBuffer()
	{
		if (pVBO) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		pVBO = static_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0,
			sizeof(Vertex) * vboCapacity, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		vboSize = 0;
	}

	/*
	VBOのマッピングを解除する.
	*/
	void Renderer::UnmapBuffer()
	{
		if (!pVBO || vboSize == 0) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		pVBO = nullptr;
	}

	/*
	フォントを描画する.
	*/
	void Renderer::Draw() const
	{
		if (vboSize == 0) {
			return;
		}
		vao.Bind();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		progFont->UseProgram();
		progFont->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, GameEngine::Instance().GetTexture(texFilename.c_str())->Id());
		glDrawElements(GL_TRIANGLES, (vboSize / 4) * 6, GL_UNSIGNED_SHORT, 0);
		vao.Unbind();
	}
}