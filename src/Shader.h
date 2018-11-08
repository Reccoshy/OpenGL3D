/*
@file	Shader.h
*/
#pragma once
#pragma warning(disable:4996)
#include <glew.h>
#include <string>
#include <memory>

class UniformBuffer;

namespace Shader {

	class Program;
	using ProgramPtr = std::shared_ptr<Program>; ///<	�v���O�����I�u�W�F�N�g�|�C���^.

	/*
	�V�F�[�_�[�v���O�����N���X.
	*/
	class Program
	{
	public:
		static ProgramPtr Create(const char* vsFilename, const char* fsFilename);

		bool UniformBlockBinding(const char* blockName, GLuint bindingPoint);
		bool UniformBlockBinding(const UniformBuffer& object);
		void UseProgram();
		void BindTexture(GLenum unit, GLenum type, GLuint texture);
		GLuint getProgram() { return program; }

	private:
		Program() = default;
		~Program();
		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

	private:
		GLuint program = 0; ///< �v���O�����I�u�W�F�N�g.
		GLint samplerLocation = -1; ///< �T���v���[�̈ʒu.
		int samplerCount = 0; ///< �T���v���[�̐�.
		std::string name; ///< �v���O������.
	};

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFileName);

}