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
		glm::vec3 position;	///< ���W.
		glm::vec4 color;	///< �F.
		glm::vec2 texCoord;	///< �e�N�X�`�����W.
	};

	/// ���_�f�[�^.
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

	/// �C���f�b�N�X�f�[�^.
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
	�����`��f�[�^.
	*/
	struct RenderingPart
	{
		GLvoid* offset; ///<	�`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g.
		GLsizei size;	///>	�`�悷��C���f�b�N�X��.
	};

	/*
	RenderingPart���쐬����.

	@param	offset	�`���C���f�b�N�X�̃I�t�Z�b�g(�C���f�b�N�X�P��).
	@param	size	�`�悷��C���f�b�N�X��.

	@return	�쐬���������`��I�u�W�F�N�g.
	*/
	constexpr RenderingPart MakeRenderingPart(GLsizei offset, GLsizei size) {
		return { reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)),size };
	}

	/*
	�����`��f�[�^���X�g.
	*/
	static const RenderingPart renderingParts[] = {
		MakeRenderingPart(0,12),
		MakeRenderingPart(12,6),
	};

	/*
	Vertex Buffer Object���쐬����.

	@param size ���_�f�[�^�̃T�C�Y.
	@param data ���_�f�[�^�ւ̃|�C���^.

	@return �쐬����VBO.
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
	Index Buffer Object���쐬����.

	@param size �C���f�b�N�X�f�[�^�̃T�C�Y.
	@param data �C���f�b�N�X�f�[�^�ւ̃|�C���^.

	@return �쐬����IBO.
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
	���_�A�g���r���[�g��ݒ肷��.

	@param index ���_�A�g���r���[�g�̃C���f�b�N�X.
	@param cls   ���_�f�[�^�^��.
	@param mbr   ���_�A�g���r���[�g�ɐݒ肷��cls�̃����o�ϐ���.
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
	Vertex Array Object���쐬����.

	@param vbo VAO�Ɋ֘A�t������VBO.
	@param ibo VAO�Ɋ֘A�t������IBO.

	@return �쐬����VAO.
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
	Uniform Block Object���쐬����.

	@param size Uniform Block�̃T�C�Y.
	@param data Uniform Block�ɓ]������f�[�^�ւ̃|�C���^.

	@return �쐬����UBO.
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
�Q�[���G���W���̃C���X�^���X���擾����.

@return	�Q�[���G���W���̃C���X�^���X.
*/
GameEngine& GameEngine::Instance()
{
	static GameEngine instance;
	return instance;
}

/*
�Q�[���G���W��������������.

@param w		�E�B���h�E�̕`��͈͂̕�(�s�N�Z��).
@param h		�E�B���h�E�̕`��͈͂̍���(�s�N�Z��)
@param title	�E�B���h�E�^�C�g��(UTF-8��0�I�[������).

@retval true	����������.
@retval false	���������s.

Run�֐����Ăяo���O�ɁA��x�Ăяo���K�v������.
��x�������ɐ�������ƁA�Ȍ�̌Ăяo���͉�������true��Ԃ�.
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
		std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
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
		std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}
	entityBuffer = Entity::Buffer::Create(1024, sizeof(InterfaceBlock::VertexData),
		InterfaceBlock::BINDINGPOINT_VERTEXDATA, "VertexData");
	if (!entityBuffer) {
		std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
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
�Q�[�������s����.
*/
void GameEngine::Run()
{
	GLFWEW::Window& window = GLFWEW::Window::instance();
	double prevTime = glfwGetTime();
	while (!window.ShouldClose() && this->running) {
		//	�f�o�b�N���ȂǓ���ȏ�ԂłȂ���΂��肦�Ȃ��ƍl�����邵�����l.
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
��ԍX�V�֐���ݒ肷��.

@param func	�ݒ肷��X�V�֐�.
*/
void GameEngine::UpdateFunc(const UpdateFuncType& func)
{
	updateFunc = func;
}

/*
��ԍX�V�֐����擾����.

@return	�ݒ肳��Ă���X�V�֐�.
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc() const
{
	return updateFunc;
}

/*
�e�N�X�`����ǂݍ���.

@param filename	�e�N�X�`���t�@�C����.

@retval	true	�ǂݍ��ݐ���.
@retval false	�ǂݍ��ݎ��s.
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
�e�N�X�`�����擾����.

@param filename	�e�N�X�`���t�@�C����.

@return filename�ɑΉ�����e�N�X�`���ւ̃|�C���^.
		�Ή�����e�N�X�`����������Ȃ��ꍇ��nullptr��Ԃ�,
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
�L���[�u�}�b�v�p�̃e�N�X�`����ǂݍ���.

@param faces	�e�N�X�`���ւ�6�̃t�@�C������vector�^.
@param name		����g�p�̍ۂɎg�����O.

@retval	true	�ǂݍ��ݐ���.
@ratval false	�ǂݍ��ݎ��s.
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
���b�V����ǂݍ���.

@param filename	���b�V���t�@�C����.

@retval true	�ǂݍ��ݐ���.
@retval false	�ǂݍ��ݎ��s.
*/
bool GameEngine::LoadMeshFromFile(const char* filename)
{
	return meshBuffer->LoadMeshFromFile(filename);
}

/*
�G���e�B�e�B��ǉ�����.

@param groupId	�G���e�B�e�B�̃O���[�vID.
@param pos		�G���e�B�e�B�̍��W.
@param meshName	�G���e�B�e�B�̕\���Ɏg�p���郁�b�V����.
@param texName	�G���e�B�e�B�̕\���Ɏg���e�N�X�`���t�@�C����.
@param func		�G���e�B�e�B�̏�Ԃ��X�V����֐�(�܂��͊֐��I�u�W�F�N�g).
@param shader	�G���e�B�e�B�̕\���Ɏg���V�F�[�_��.

@return	�ǉ������G���e�B�e�B�ւ̃|�C���^.
		����ȏ�G���e�B�e�B��ǉ��ł��Ȃ��ꍇ��nullptr���Ԃ����.
		�ϓ_��g�嗦�͂��̃|�C���^�o�R�Őݒ肷��.
		�Ȃ��A���̃|�C���^���A�v���P�[�V�������ŕێ�����K�v�͂Ȃ�.
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
�G���e�B�e�B���폜����.

@param	�폜����G���e�B�e�B�̃|�C���^.
*/
void GameEngine::RemoveEntity(Entity::Entity* e)
{
	entityBuffer->RemoveEntity(e);
}

/*
���C�g��ݒ肷��.

@param indes	�ݒ肷�郉�C�g�̃C���f�b�N�X.
@param light	���C�g�f�[�^,
*/
void GameEngine::Light(int index, const InterfaceBlock::PointLight& light)
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/*
���C�g���擾����.

@param index	�擾���郉�C�g�̃C���f�b�N�X.

@return ���C�g�f�[�^.
*/
const InterfaceBlock::PointLight& GameEngine::Light(int index) const
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		static const InterfaceBlock::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];
}

/*
������ݒ肷��.

@param color	�����̖��邳.
*/
void GameEngine::AmbientLight(const glm::vec4& color)
{
	lightData.ambientColor = color;
}

/*
�������擾����.

@return �����̖��邳.
*/
const glm::vec4& GameEngine::AmbientLight() const
{
	return lightData.ambientColor;
}

/*
���_�̈ʒu�Ǝp����ݒ肷��.

@param cam	�ݒ肷��J�����f�[�^.
*/
void GameEngine::Camera(const CameraData& cam, int index)
{
	camera[index] = cam;
}

/*
���_�̈ʒu�Ǝp�����擾����.

@return �J�����f�[�^.
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
* �Փˉ����n���h����ݒ肷��.
*
* @param gid0    �ՓˑΏۂ̃O���[�vID.
* @param gid1    �ՓˑΏۂ̃O���[�vID.
* @param handler �Փˉ����n���h��.
*
* �Փ˂��������Փˉ����n���h�����Ăт����Ƃ��A��菬�����O���[�vID�����G���e�B�e�B�����ɓn�����.
* �����Ŏw�肵���O���[�vID�̏����Ƃ͖��֌W�ł��邱�Ƃɒ��ӂ��邱��.
* ex)
*   CollisionHandler(10, 1, Func)
*   �Ƃ����R�[�h�Ńn���h����o�^�����Ƃ���. �Փ˂���������ƁA
*   Func(�O���[�vID=1�̃G���e�B�e�B�A�O���[�vID=10�̃G���e�B�e�B)
*   �̂悤�ɌĂяo�����.
*/
void GameEngine::CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler)
{
	entityBuffer->CollisionHandler(gid0, gid1, handler);
}

/**
* �Փˉ����n���h�����擾����.
*
* @param gid0 �ՓˑΏۂ̃O���[�vID.
* @param gid1 �ՓˑΏۂ̃O���[�vID.
*
* @return �Փˉ����n���h��.
*/
const Entity::CollisionHandlerType & GameEngine::CollisionHandler(int gid0, int gid1) const
{
	return entityBuffer->CollisionHandler(gid0, gid1);
}

/**
* �Փˉ����n���h���̃��X�g���N���A����.
*/
void GameEngine::ClearCollisionHandlerList()
{
	entityBuffer->ClearCollisionHandlerList();
}

/*
�w�i�ɗp����L���[�u�}�b�v��ݒ肷��.
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
�Q�[���̏�Ԃ��X�V����.

@param delta	�O��̍X�V����̌o�ߎ���(�b).
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
�Q�[���̏�Ԃ�`�悷��.
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
