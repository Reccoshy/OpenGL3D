/*
@file UniformBuffer.h
*/
#pragma once

#include <glew.h>
#include <string>
#include <memory>

class UniformBuffer;
using UniformBufferPtr = std::shared_ptr<UniformBuffer>;

/*
UBO�N���X.
*/
class UniformBuffer {
public:
	static UniformBufferPtr Create(
		GLsizeiptr size, GLuint bindingPoint, const char* name);
	bool BufferSubData(const GLvoid* data, GLintptr offset = 0, GLsizei = 0);
	void BindBufferRange(GLintptr offset, GLsizeiptr size) const;
	void* MapBuffer() const;
	void UnmapBuffer() const;
	GLsizeiptr Size() const { return size; }
	GLuint BindingPoint() const { return bindingPoint; }
	const std::string& Name() const { return name; }

private:
	UniformBuffer() = default;
	~UniformBuffer();
	UniformBuffer(const UniformBuffer&) = default;
	UniformBuffer& operator= (const UniformBuffer&) = default;

private:
	GLuint ubo = 0;			/// <Uniform Buffer Object.
	GLsizeiptr size = 0;	/// <UBO�̃o�C�g�T�C�Y.
	GLuint bindingPoint;	/// <UBO�̊��蓖�Đ�o�C���f�B���O�E�|�C���g.
	std::string name;		/// <UBO��.
};