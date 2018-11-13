/*
@file GameEngine.cpp
*/
#include "GameEngine.h"
#include "Audio.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdio.h>
#include <time.h>

namespace {

	struct Vertex
	{
		glm::vec3 position;	///< 座標.
		glm::vec4 color;	///< 色.
		glm::vec2 texCoord;	///< テクスチャ座標.
	};

	/// 頂点データ.
	const Vertex vertices[] = {
		{ { -0.5f, -0.3f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
		{ { 0.3f, -0.3f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { 0.3f,  0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { -0.3f,  0.3f, 0.1f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
		{ { -0.3f, -0.5f, 0.1f },{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.1f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.1f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { 0.5f,  0.3f, 0.1f },{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.3f,  0.3f, 0.1f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { -1.0f,-1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
		{ { 1.0f,-1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -1.0f, 1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
	};

	float skyBoxScale = 10.0f;

	GLfloat skyboxVertices[] = {
		// Positions
		-skyBoxScale,  skyBoxScale, -skyBoxScale,
		-skyBoxScale, -skyBoxScale, -skyBoxScale,
		skyBoxScale, -skyBoxScale, -skyBoxScale,
		skyBoxScale, -skyBoxScale, -skyBoxScale,
		skyBoxScale,  skyBoxScale, -skyBoxScale,
		-skyBoxScale,  skyBoxScale, -skyBoxScale,

		-skyBoxScale, -skyBoxScale,  skyBoxScale,
		-skyBoxScale, -skyBoxScale, -skyBoxScale,
		-skyBoxScale,  skyBoxScale, -skyBoxScale,
		-skyBoxScale,  skyBoxScale, -skyBoxScale,
		-skyBoxScale,  skyBoxScale,  skyBoxScale,
		-skyBoxScale, -skyBoxScale,  skyBoxScale,

		skyBoxScale, -skyBoxScale, -skyBoxScale,
		skyBoxScale, -skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale, -skyBoxScale,
		skyBoxScale, -skyBoxScale, -skyBoxScale,

		-skyBoxScale, -skyBoxScale,  skyBoxScale,
		-skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale, -skyBoxScale,  skyBoxScale,
		-skyBoxScale, -skyBoxScale,  skyBoxScale,

		-skyBoxScale,  skyBoxScale, -skyBoxScale,
		skyBoxScale,  skyBoxScale, -skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		skyBoxScale,  skyBoxScale,  skyBoxScale,
		-skyBoxScale,  skyBoxScale,  skyBoxScale,
		-skyBoxScale,  skyBoxScale, -skyBoxScale,

		-skyBoxScale, -skyBoxScale, -skyBoxScale,
		-skyBoxScale, -skyBoxScale,  skyBoxScale,
		skyBoxScale, -skyBoxScale, -skyBoxScale,
		skyBoxScale, -skyBoxScale, -skyBoxScale,
		-skyBoxScale, -skyBoxScale,  skyBoxScale,
		skyBoxScale, -skyBoxScale,  skyBoxScale
	};

	/// インデックスデータ.
	const GLuint indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 7, 8, 9,
		10, 11, 12, 12, 13, 10,
	};

	struct GroundVertex {
		glm::vec3 position;
		glm::vec2 texCoord;
	};

	float groundScale = 800.0f;

	GLfloat tiling = 1;
	GLfloat hight = -1;

	GroundVertex groundVerticies[] = 
	{
		{ { -groundScale,  hight, -groundScale, }, { 0,0 }},
		{ { -groundScale,  hight, groundScale, },{ 0, tiling } },
		{ { groundScale,  hight, -groundScale, },{ tiling, 0 } },
		{ { groundScale,  hight, -groundScale, },{ tiling, 0 } },
		{ { -groundScale,  hight, groundScale, },{ 0, tiling } },
		{ { groundScale,  hight, groundScale, },{ tiling, tiling } },
	};

	/*
	部分描画データ.
	*/
	struct RenderingPart
	{
		GLvoid* offset; ///<	描画開始インデックスのバイトオフセット.
		GLsizei size;	///>	描画するインデックス数.
	};

	/*
	RenderingPartを作成する.

	@param	offset	描画会インデックスのオフセット(インデックス単位).
	@param	size	描画するインデックス数.

	@return	作成した部分描画オブジェクト.
	*/
	constexpr RenderingPart MakeRenderingPart(GLsizei offset, GLsizei size) {
		return { reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)),size };
	}

	/*
	部分描画データリスト.
	*/
	static const RenderingPart renderingParts[] = {
		MakeRenderingPart(0,12),
		MakeRenderingPart(12,6),
	};

	/*
	Vertex Buffer Objectを作成する.

	@param size 頂点データのサイズ.
	@param data 頂点データへのポインタ.

	@return 作成したVBO.
	*/
	GLuint CreateVBO(GLsizeiptr size, const GLvoid* data)
	{
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return vbo;
	}

	/*
	Index Buffer Objectを作成する.

	@param size インデックスデータのサイズ.
	@param data インデックスデータへのポインタ.

	@return 作成したIBO.
	*/
	GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
	{
		GLuint ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return ibo;
	}

	/*
	頂点アトリビュートを設定する.

	@param index 頂点アトリビュートのインデックス.
	@param cls   頂点データ型名.
	@param mbr   頂点アトリビュートに設定するclsのメンバ変数名.
	*/
#define SetVertexAttribPointer(index, cls, mbr) SetVertexAttribPointerI( \
  index, \
  sizeof(cls::mbr) / sizeof(float), \
  sizeof(cls), \
  reinterpret_cast<GLvoid*>(offsetof(cls, mbr)))

	void SetVertexAttribPointerI(
		GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
	}

	/*
	Vertex Array Objectを作成する.

	@param vbo VAOに関連付けられるVBO.
	@param ibo VAOに関連付けられるIBO.

	@return 作成したVAO.
	*/
	GLuint CreateVAO(GLuint vbo, GLuint ibo)
	{
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		SetVertexAttribPointer(0, Vertex, position);
		SetVertexAttribPointer(1, Vertex, color);
		SetVertexAttribPointer(2, Vertex, texCoord);
		glBindVertexArray(0);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		return vao;
	}

	/*
	Uniform Block Objectを作成する.

	@param size Uniform Blockのサイズ.
	@param data Uniform Blockに転送するデータへのポインタ.

	@return 作成したUBO.
	*/
	GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr)
	{
		GLuint ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		return ubo;
	}
}

/*
ゲームエンジンのインスタンスを取得する.

@return	ゲームエンジンのインスタンス.
*/
GameEngine& GameEngine::Instance()
{
	static GameEngine instance;
	return instance;
}

/*
ゲームエンジンを初期化する.

@param w		ウィンドウの描画範囲の幅(ピクセル).
@param h		ウィンドウの描画範囲の高さ(ピクセル)
@param title	ウィンドウタイトル(UTF-8の0終端文字列).

@retval true	初期化成功.
@retval false	初期化失敗.

Run関数を呼び出す前に、一度呼び出す必要がある.
一度初期化に成功すると、以後の呼び出しは何もせずtrueを返す.
*/
bool GameEngine::Init(int w, int h, const char* title, bool fullScreen)
{
	if (isInitialized) {
		return true;
	}
	if (!GLFWEW::Window::instance().Init(w, h, title, fullScreen)) {
		return false;
	}

	width = w;
	height = h;
	vbo = CreateVBO(sizeof(vertices), vertices);
	ibo = CreateIBO(sizeof(indices), indices);
	vao = CreateVAO(vbo, ibo);
	uboLight = UniformBuffer::Create(sizeof(InterfaceBlock::LightData),
		InterfaceBlock::BINDINGPOINT_LIGHTDATA, "LightData");
	uboPostEffect = UniformBuffer::Create(sizeof(InterfaceBlock::PostEffectData),
		InterfaceBlock::BINDINGPOINT_POSTEFFECTDATA, "PostEffectData");
	offscreen = OffscreenBuffer::Create(width, height);

	// Setup skybox VAO
	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vbo2);
	glBindVertexArray(vao2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
	glBindVertexArray(0);

	// Setup ground VAO
	glGenVertexArrays(1, &groundVao);
	glGenBuffers(1, &groundVbo);
	glBindVertexArray(groundVao);
	glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerticies), &groundVerticies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GroundVertex), (GLvoid *)offsetof(GroundVertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GroundVertex), (GLvoid*)offsetof(GroundVertex, texCoord));
	glBindVertexArray(0);

	if (!vbo || !ibo || !vao || !uboLight || !uboPostEffect || !offscreen) {
		std::cerr << "ERROR: GameEngineの初期化に失敗" << std::endl;
		return false;
	}

	static const char* const shaderNameList[][3] = {
		{ "Tutorial", "res/Tutorial.vert", "res/Tutorial.frag"},
		{ "ColorFilter", "res/ColorFilter.vert", "res/ColorFilter.frag"},
		{ "NonLighting", "res/NonLighting.vert", "res/NonLighting.frag"},
		{ "CubeMap", "res/SkyBox.vert", "res/SkyBox.frag" },
		{ "BlendMap", "res/Terrain.vert", "res/Terrain.frag"},
		{ "Effect", "res/Effect.vert", "res/Effect.frag"},
	};
	shaderMap.reserve(sizeof(shaderNameList) / sizeof(shaderNameList[0]));
	for (auto& e : shaderNameList) {
		Shader::ProgramPtr program = Shader::Program::Create(e[1], e[2]);
		if (!program) {
			return false;
		}
		shaderMap.insert(std::make_pair(std::string(e[0]), program));
	}

	meshBuffer = Mesh::Buffer::Create(500000, 500000);
	if (!meshBuffer) {
		std::cerr << "ERROR: GameEngineの初期化に失敗" << std::endl;
		return false;
	}
	entityBuffer = Entity::Buffer::Create(1024, sizeof(InterfaceBlock::VertexData),
		InterfaceBlock::BINDINGPOINT_VERTEXDATA, "VertexData");
	if (!entityBuffer) {
		std::cerr << "ERROR: GameEngineの初期化に失敗" << std::endl;
		return false;
	}
	shaderMap["Tutorial"]->UniformBlockBinding(*entityBuffer->UniformBuffer());
	shaderMap["Tutorial"]->UniformBlockBinding(*uboLight);
	shaderMap["ColorFilter"]->UniformBlockBinding(*uboPostEffect);
	const Shader::ProgramPtr& progColorFilter = shaderMap.find("ColorFilter")->second;

	fontRenderer.Init(1024, glm::vec2(static_cast<float>(w), static_cast<float>(h)), 32);
	if (!uiRenderer.Init(1024, glm::vec2(static_cast<float>(w), static_cast<float>(h))) || 
		!frontUiRenderer.Init(1024, glm::vec2(static_cast<float>(w), static_cast<float>(h))))
		return false;

	m_effect.Init(glm::vec2(width, height));

	srand(time(NULL));

	isInitialized = true;
	return true;
}

/*
ゲームを実行する.
*/
void GameEngine::Run()
{
	GLFWEW::Window& window = GLFWEW::Window::instance();
	double prevTime = glfwGetTime();
	while (!window.ShouldClose() && this->running) {
		//	デバック中など特殊な状態でなければありえないと考えられるしきい値.
		static const double theresholdToInvalidate = 0.25f;
		
		const double curTime = glfwGetTime();
		double delta = curTime - prevTime;
		if (delta >= theresholdToInvalidate) {
			delta = 1.0 / 60.0;
		}
		prevTime = curTime;
		Update(delta);
		Render(static_cast<float>(delta));
		window.SwapBuffers();
	}
}

/*
状態更新関数を設定する.

@param func	設定する更新関数.
*/
void GameEngine::UpdateFunc(const UpdateFuncType& func)
{
	updateFunc = func;
}

/*
状態更新関数を取得する.

@return	設定されている更新関数.
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc() const
{
	return updateFunc;
}

/*
テクスチャを読み込む.

@param filename	テクスチャファイル名.

@retval	true	読み込み成功.
@retval false	読み込み失敗.
*/
bool GameEngine::LoadTextureFromFile(const char* filename)
{
	if (GetTexture(filename)) {
		return true;
	}
	TexturePtr texture = Texture::LoadFromFile(filename);
	if (!texture) {
		return false;
	}
	textureBuffer.insert(std::make_pair(std::string(filename), texture));
	return true;
}

/*
テクスチャを取得する.

@param filename	テクスチャファイル名.

@return filenameに対応するテクスチャへのポインタ.
		対応するテクスチャが見つからない場合はnullptrを返す,
*/
const TexturePtr& GameEngine::GetTexture(const char* filename) const
{
	const auto itr = textureBuffer.find(filename);
	if (itr != textureBuffer.end()) {
		return itr->second;
	}
	static const TexturePtr dummy;
	return dummy;
}

/*
キューブマップ用のテクスチャを読み込む.

@param faces	テクスチャへの6つのファイル名のvector型.
@param name		今後使用の際に使う名前.

@retval	true	読み込み成功.
@ratval false	読み込み失敗.
*/
bool GameEngine::LoadCubeMap(std::vector<const char*> faces, const char* name)
{
	TexturePtr texture = Texture::CreateCubeMap(faces);

	if (!texture) {
		return false;
	}

	textureBuffer.insert(std::make_pair(std::string(name), texture));

	return true;
}

/*
メッシュを読み込む.

@param filename	メッシュファイル名.

@retval true	読み込み成功.
@retval false	読み込み失敗.
*/
bool GameEngine::LoadMeshFromFile(const char* filename)
{
	return meshBuffer->LoadMeshFromFile(filename);
}

/*
エンティティを追加する.

@param groupId	エンティティのグループID.
@param pos		エンティティの座標.
@param meshName	エンティティの表示に使用するメッシュ名.
@param texName	エンティティの表示に使うテクスチャファイル名.
@param func		エンティティの状態を更新する関数(または関数オブジェクト).
@param shader	エンティティの表示に使うシェーダ名.

@return	追加したエンティティへのポインタ.
		これ以上エンティティを追加できない場合はnullptrが返される.
		観点や拡大率はこのポインタ経由で設定する.
		なお、このポインタをアプリケーション側で保持する必要はない.
*/
Entity::Entity* GameEngine::AddEntity(int groupId, const glm::vec3& pos, const char* meshName,
	const char* texName, Entity::Entity::UpdateFuncType func, bool isInitial,
	const char* shader)
{
	decltype(shaderMap)::const_iterator itr = shaderMap.end();
	if (shader) {
		itr = shaderMap.find(shader);
	}
	if (itr == shaderMap.end()) {
		itr = shaderMap.find("Tutorial");
		if (itr == shaderMap.end()) {
			return nullptr;
		}
	}

	const Mesh::MeshPtr& mesh = meshBuffer->GetMesh(meshName);
	const TexturePtr& tex = textureBuffer.find(texName)->second;

	return entityBuffer->AddEntity(groupId, pos, mesh, tex, itr->second, func, isInitial);
}

/*
エンティティを削除する.

@param	削除するエンティティのポインタ.
*/
void GameEngine::RemoveEntity(Entity::Entity* e)
{
	entityBuffer->RemoveEntity(e);
}

/*
ライトを設定する.

@param indes	設定するライトのインデックス.
@param light	ライトデータ,
*/
void GameEngine::Light(int index, const InterfaceBlock::PointLight& light)
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'は不正なライトインデックスです" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/*
ライトを取得する.

@param index	取得するライトのインデックス.

@return ライトデータ.
*/
const InterfaceBlock::PointLight& GameEngine::Light(int index) const
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'は不正なライトインデックスです" << std::endl;
		static const InterfaceBlock::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];
}

/*
環境光を設定する.

@param color	環境光の明るさ.
*/
void GameEngine::AmbientLight(const glm::vec4& color)
{
	lightData.ambientColor = color;
}

/*
環境光を取得する.

@return 環境光の明るさ.
*/
const glm::vec4& GameEngine::AmbientLight() const
{
	return lightData.ambientColor;
}

/*
視点の位置と姿勢を設定する.

@param cam	設定するカメラデータ.
*/
void GameEngine::Camera(const CameraData& cam, int index)
{
	camera[index] = cam;
}

/*
視点の位置と姿勢を取得する.

@return カメラデータ.
*/
const GameEngine::CameraData& GameEngine::Camera(int index) const
{
	return camera[index];
}

const GamePad & GameEngine::GetGamePad(int i) const
{
	return GLFWEW::Window::instance().GetGamePad(i);
}

/**
* 衝突解決ハンドラを設定する.
*
* @param gid0    衝突対象のグループID.
* @param gid1    衝突対象のグループID.
* @param handler 衝突解決ハンドラ.
*
* 衝突が発生し衝突解決ハンドラが呼びされるとき、より小さいグループIDを持つエンティティから先に渡される.
* ここで指定したグループIDの順序とは無関係であることに注意すること.
* ex)
*   CollisionHandler(10, 1, Func)
*   というコードでハンドラを登録したとする. 衝突が発生すると、
*   Func(グループID=1のエンティティ、グループID=10のエンティティ)
*   のように呼び出される.
*/
void GameEngine::CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler)
{
	entityBuffer->CollisionHandler(gid0, gid1, handler);
}

/**
* 衝突解決ハンドラを取得する.
*
* @param gid0 衝突対象のグループID.
* @param gid1 衝突対象のグループID.
*
* @return 衝突解決ハンドラ.
*/
const Entity::CollisionHandlerType & GameEngine::CollisionHandler(int gid0, int gid1) const
{
	return entityBuffer->CollisionHandler(gid0, gid1);
}

/**
* 衝突解決ハンドラのリストをクリアする.
*/
void GameEngine::ClearCollisionHandlerList()
{
	entityBuffer->ClearCollisionHandlerList();
}

/*
背景に用いるキューブマップを設定する.
*/
void GameEngine::SetCubeMap(const char * filename)
{
	cubemapTexture = GetTexture(filename);
}

bool GameEngine::SetGroundTexture(int texNum,const char * filename)
{
	switch (texNum)
	{
	case 0:
		groundTexture0 = GetTexture(filename);
		return true;
		break;

	case 1:
		groundTexture1 = GetTexture(filename);
		return true;
		break;

	case 2:
		groundTexture2 = GetTexture(filename);
		return true;
		break;

	case 3:
		groundTexture3 = GetTexture(filename);
		return true;
		break;

	case 4:
		groundTexture4 = GetTexture(filename);
		
		break;

	default:
		break;
	}
	return false;
}

void GameEngine::RemoveGroundTexture()
{
	groundTexture0 = 0;
	groundTexture1 = 0;
	groundTexture2 = 0;
	groundTexture3 = 0;
	groundTexture4 = 0;
}

void GameEngine::toggleFullScreen()
{
	GLFWEW::Window& window = GLFWEW::Window::instance();

	window.ToggleFullScreen();
	offscreen = nullptr;
	offscreen = OffscreenBuffer::Create(1920,1080);
}

void GameEngine::EndGame()
{
	running = false;
}

/*
@copydoc Audio::Initialize
*/
bool GameEngine::InitAudio(const char * acfPath, const char * acbPath, const char * awbPath, const char * dspBusName, size_t playerCount)
{
	return Audio::Initialize(acfPath, acbPath, awbPath, dspBusName, playerCount);
}

/*
@copydoc Audio::Play
*/
void GameEngine::PlayAudio(int playerId, int cueId)
{
	Audio::Play(playerId, cueId);
}

/*
@copydoc Audio::Resume
*/
void GameEngine::ResumeAudio(int playerId)
{
	Audio::Resume(playerId);
}

/*
@copydoc Audio::Pause
*/
void GameEngine::PauseAudio(int playerId)
{
	Audio::Pause(playerId);
}

/*
@copydoc Audio::Stop
*/
void GameEngine::StopAudio(int playerId) 
{
	Audio::Stop(playerId);
}

/*
@copydoc Audio::StopAllSound
*/
void GameEngine::StopAllSound()
{
	Audio::StopAllSound();
}

void GameEngine::SetCameraNum(int num)
{
	if (num <= 0) {
		num = 1;
	}
	else if (num > 4) {
		num = 4;
	}
	else if (num == 3) {
		num = 4;
	}

	CameraNum = num;
}

void GameEngine::AddEffect(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, const char * imageName)
{
	m_effect.AddEffect(pos, scale, color, imageName);
}

int GameEngine::GetRandomInt(int maxInt, int minInt)
{
	int result = rand() % (maxInt - minInt);

	result += minInt;

	return result;
}

float GameEngine::GetRandomFloat(float maxFloat, float minFloat, float acuracy)
{
	int devide = maxFloat * acuracy - minFloat * acuracy;

	float result = rand() % devide;

	result = result / acuracy + minFloat;

	return result;
}

GameEngine::~GameEngine()
{
	updateFunc = nullptr;
	Audio::Destroy();
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

/*
ゲームの状態を更新する.

@param delta	前回の更新からの経過時間(秒).
*/
void GameEngine::Update(double delta)
{
	GLFWEW::Window::instance().UpdateGamePad();
	fontRenderer.MapBuffer();
	uiRenderer.MapBuffer();
	frontUiRenderer.MapBuffer();
	if (updateFunc) {
		updateFunc(delta);
	}

	const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f),
		static_cast<float>(width) / static_cast<float> (height), 1.0f, 400.0f);
	const glm::mat4x4 matView = glm::lookAt(camera[3].position, camera[3].target, camera[3].up);
	entityBuffer->Update(delta, matView, matProj);
	fontRenderer.UnmapBuffer();
	uiRenderer.UnmapBuffer();
	frontUiRenderer.UnmapBuffer();

	if (GLFWEW::Window::instance().GetGamePad(0).buttonDown & GamePad::END) {
		this->EndGame();
	}
}

/*
ゲームの状態を描画する.
*/
void GameEngine::Render(float delta)
{
	for (int i = 0; i < CameraNum; i++) {

		const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f),
			static_cast<float>(width) / static_cast<float> (height), 1.0f, 1000.0f);
		const glm::mat4x4 matView = glm::lookAt(camera[i].position, camera[i].target, camera[i].up);
		entityBuffer->Update(delta, matView, matProj);

		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClearDepth(1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		uboLight->BufferSubData(&lightData);

		this->RenderCubeMap(i);
		this->RenderGround(i);

		entityBuffer->Draw(meshBuffer);
		m_effect.Draw(i);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glBindVertexArray(vao);
		shaderMap.find("ColorFilter")->second->UseProgram();
		InterfaceBlock::PostEffectData postEffect;
		uboPostEffect->BufferSubData(&postEffect);

		if (CameraNum == 1) {
			glViewport(0, 0, width, height);
			shaderMap.find("ColorFilter")->second->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
			glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);
		}
		else if (CameraNum == 2) {
			glViewport(0, (CameraNum - 1 - i) * (height / 2), width, height / 2);

			shaderMap.find("ColorFilter")->second->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
			glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);

			glViewport(0, 0, width, height);
		}
		else if (CameraNum == 3 || CameraNum == 4) {
			glViewport((i % 2) * (width / 2), ((CameraNum - 1 - i) / 2) * (height / 2), width / 2, height / 2);

			shaderMap.find("ColorFilter")->second->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
			glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);

			glViewport(0, 0, width, height);
		}

	}

	uiRenderer.Draw();
	fontRenderer.Draw();
	frontUiRenderer.Draw();

	m_effect.EndClear();
}

void GameEngine::RenderCubeMap(int cameraNum) const
{
	if (!cubemapTexture) {
		return;
	}

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	shaderMap.find("CubeMap")->second->UseProgram();

	glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera[cameraNum].position, camera[cameraNum].target, camera[cameraNum].up)));	// Remove any translation component of the view matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float> (height), 1.0f, 200.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderMap.find("CubeMap")->second->getProgram(), "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderMap.find("CubeMap")->second->getProgram(), "projection"), 1, GL_FALSE, &projection[0][0]);
	
	glBindVertexArray(vao2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->Id());
	glDrawArrays(GL_TRIANGLES, 0, sizeof(skyboxVertices) / 3);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

	glEnable(GL_DEPTH_TEST);
}

void GameEngine::RenderGround(int cameraNum) const
{
	if (groundTexture0 == 0 || groundTexture1 == 0 || groundTexture2 == 0 || groundTexture3 == 0 || groundTexture4 == 0) {
		return;
	}
	
	shaderMap.find("BlendMap")->second->UseProgram();

	const glm::mat4 t = glm::translate(glm::mat4(), glm::vec3(0, 0 ,100));
	const glm::mat4 r = glm::mat4_cast(glm::quat(glm::vec3(0)));
	const glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(1));
	glm::mat4 matM = t * r * s;

	const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f),
		static_cast<float>(width) / static_cast<float> (height), 1.0f, 1000.0f);
	const glm::mat4x4 matView = glm::lookAt(camera[cameraNum].position, camera[cameraNum].target, camera[cameraNum].up);

	glm::mat4 matMVP = matProj * matView * matM;

	glUniformMatrix4fv(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "matMVP"), 1, GL_FALSE, &matMVP[0][0]);

	glBindVertexArray(groundVao);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture0->Id());
	glUniform1i(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "backGroundTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundTexture1->Id());
	glUniform1i(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "rTexture"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, groundTexture2->Id());
	glUniform1i(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "gTexture"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, groundTexture3->Id());
	glUniform1i(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "bTexture"), 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, groundTexture4->Id());
	glUniform1i(glGetUniformLocation(shaderMap.find("BlendMap")->second->getProgram(), "blendMap"), 4);
	
	glActiveTexture(GL_TEXTURE0);
	
	glDrawArrays(GL_TRIANGLES, 0, sizeof(GroundVertex) / 3);
	glBindVertexArray(0);
}
