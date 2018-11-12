/*
@file UITexture.cpp
*/
#include "UITexture.h"
#include "../../../GameEngine.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/*
フォント描画機能を格納する名前空間.
*/
namespace UITexture {

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

	@retval true	初期化成功.
	@retval false	初期化失敗.
	*/
	bool Renderer::Init(size_t maxImage, const glm::vec2& screen)
	{
		if (maxImage > (USHRT_MAX + 1) / 4) {
			std::cerr << "WARNING: " << maxImage << "は設定可能な最大画像数を越えています" << std::endl;
			maxImage = (USHRT_MAX + 1) / 4;
		}
		vboCapacity = static_cast<GLsizei>(4 * maxImage);
		vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vboCapacity, nullptr, GL_STREAM_DRAW);
		{
			std::vector<GLushort> tmp;
			tmp.resize(maxImage * 6);
			GLushort* p = tmp.data();
			for (GLushort i = 0; i < maxImage * 4; i += 4) {
				for (GLushort n : { 0, 1, 2, 2, 3, 0 }) {
					*(p++) = i + n;
				}
			}
			ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * maxImage, tmp.data(),
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

		m_progUITex = Shader::Program::Create("Res/UITexture.vert", "Res/UITexture.frag");
		if (!m_progUITex) {
			return false;
		}

		windowSize = screen;

		pixelSizeInClipCoord = 2.0f / screen;
		
		baseScale = 2.0f / windowSize;

		return true;
	}

	

	bool Renderer::SetTexture(const glm::vec2& position, const char* str, int CameraIndex, int cameraNum, bool SingleCamWrite)
	{
		if (!GameEngine::Instance().LoadTextureFromFile(str)) {
			return false;
		}
		texFilename.push_back(str);
		
		glm::vec2 camOffset = glm::vec2(0);
		glm::vec2 camScale = glm::vec2(1);
		glm::vec2 posRatio = glm::vec2(1);

		if (!SingleCamWrite) {
			if (cameraNum >= 3) {
				camScale = glm::vec2(0.5f);

				camOffset = glm::vec2(windowSize.x * 0.5 * (CameraIndex % 2), windowSize.y * 0.5 * (CameraIndex / 2));
			}
			else if (cameraNum >= 2) {
				camScale = glm::vec2(1.0f, 0.5f);

				posRatio = glm::vec2(2, 1);
				camOffset = glm::vec2(0, windowSize.y * 0.5 * CameraIndex);
			}
		}

		TexturePtr tex = GameEngine::Instance().GetTexture(texFilename.back().c_str());

		glm::vec2 texSize = glm::vec2(tex->Width() * scale.x, tex->Height() * scale.y);
		const glm::vec2 reciprocalScale(1.0f / scale);

		glm::u16vec2 uv[2];
		uv[0] = glm::vec2(0,0) * reciprocalScale * 65535.0f;
		uv[1] = texSize * camScale * reciprocalScale * 65535.0f;

		// 左上を原点とするピクセル座標系からOpenGLのクリップ座標系へ変換.
		glm::vec2 pos = ((position * camScale * posRatio + camOffset) * pixelSizeInClipCoord - 1.0f) * glm::vec2(1.0f, -1.0f);
		Vertex* p = pVBO + vboSize;
		if (vboSize + 4 > vboCapacity) { // VBOに空きがなければ終了.
			return false;
		}

		if (tex && texSize.x && texSize.y) { // 表示できないデータなら無視する.
			const glm::vec2 size = texSize * baseScale * scale * camScale;
			const glm::vec2 offsetedPos = pos + baseScale * scale * camScale;
			p[0].position = offsetedPos + glm::vec2(0, -size.y);
			p[0].uv = uv[0];
			p[0].color = color;
			p[1].position = offsetedPos + glm::vec2(size.x, -size.y);
			p[1].uv = glm::u16vec2(uv[1].x, uv[0].y);
			p[1].color = color;
			p[2].position = offsetedPos + glm::vec2(size.x, 0);
			p[2].uv = uv[1];
			p[2].color = color;
			p[3].position = offsetedPos;
			p[3].uv = glm::u16vec2(uv[0].x, uv[1].y);
			p[3].color = color;
			p += 4;
			vboSize += 4;
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
	void Renderer::Draw()
	{
		if (vboSize == 0) {
			return;
		}
		vao.Bind();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_progUITex->UseProgram();
		for (int i = 0; i < texFilename.size(); i++) {
			m_progUITex->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, GameEngine::Instance().GetTexture(texFilename[i].c_str())->Id());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, reinterpret_cast<const GLint*>(12 * i));
		}
		vao.Unbind();

		this->texFilename.clear();
	}
}
