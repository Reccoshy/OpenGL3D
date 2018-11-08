/*
@file	Texture.h
*/
#pragma once
#pragma warning(disable:4996)

#include <glew.h>
#include <memory>
#include <vector>

class Texture;
using TexturePtr = std::shared_ptr<Texture>; ///< �e�N�X�`���|�C���^.

/*
�e�N�X�`���N���X.
*/
class Texture
{
public:
	static TexturePtr Create(
		int width, int height, GLenum iformat, GLenum format, const void* data);
	static TexturePtr LoadFromFile(const char*);

	static TexturePtr CreateCubeMap(std::vector<const char*> faces);

	GLuint Id() const { return texId; }
	GLsizei Width() const { return width; }
	GLsizei Height() const { return height; }

private:
	Texture() = default;
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	GLuint texId = 0;
	int width = 0;
	int height = 0;
};