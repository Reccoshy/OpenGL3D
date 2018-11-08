/*
@file	Texture.cpp
*/
#include "Texture.h"
#include <iostream>

#include <algorithm>

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

/*
�o�C�g�񂩂琔�l�𕜌�����.

@param	p		�o�C�g��ւ̃|�C���^
@param	offset	���l�̃I�t�Z�b�g
@param	size	���l�̃o�C�g���i�P�`�S�j.

@return	�����������l.
*/
uint32_t Get(const uint8_t* p, size_t offset, size_t size)
{
	uint32_t n = 0;
	p += offset;
	for (size_t i = 0; i < size; ++i) {
		n += p[i] << (i * 8);
	}
	return n;
}

/*
FOURCC���쐬����B
*/
#define MAKE_FOURCC(a,b,c,d) \
	static_cast<uint32_t>(a + (b << 8) + (c << 16) + (d << 24))

/*
DDS �摜���.
*/
struct DDSPixelFormat
{
	uint32_t size; ///< ���̍\���̂̃o�C�g��(32).
	uint32_t flags; ///< �摜�Ɋ܂܂��f�[�^�̎�ނ������t���O.
	uint32_t fourCC; ///< �摜�t�H�[�}�b�g������FOURCC.
	uint32_t rgbBitCount; ///< 1�s�N�Z���̃r�b�g��.
	uint32_t redBitMask; ///< �ԗv�f���g�������������r�b�g.
	uint32_t greenBitMask; ///< �Ηv�f���g�������������r�b�g.
	uint32_t blueBitMask; ///< �v�f���g�������������r�b�g.
	uint32_t alphaBitMask; ///< �����v�f���g�������������r�b�g.
};

/*
�o�b�t�@����DDS�摜����ǂݏo��.

@param buf �ǂݏo�����o�b�t�@.

@return �ǂݏo����DDS�摜��.
*/
DDSPixelFormat ReadDDSPixelFormat(const uint8_t* buf)
{
	DDSPixelFormat tmp;
	tmp.size = Get(buf, 0, 4);
	tmp.flags = Get(buf, 4, 4);
	tmp.fourCC = Get(buf, 8, 4);
	tmp.rgbBitCount = Get(buf, 12, 4);
	tmp.redBitMask = Get(buf, 16, 4);
	tmp.greenBitMask = Get(buf, 20, 4);
	tmp.blueBitMask = Get(buf, 24, 4);
	tmp.alphaBitMask = Get(buf, 28, 4);
	return tmp;
}

/*
DDS�t�@�C���w�b�_.
*/
struct DDSHeader
{
	uint32_t size;				///< ���̍\���̂̃o�C�g��(124).
	uint32_t flags;				///< �ǂ̃p�����[�^�[���L�����������t���O.
	uint32_t height;			///< �摜�̍���(�s�N�Z����).
	uint32_t width;				///< �摜�̕�(�s�N�Z����).
	uint32_t pitchOrLinearSize;	///< ���̃o�C�g���܂��͉摜1���̃o�C�g��.
	uint32_t depth;				///< �摜�̉��s��(����)(3�����e�N�X�`�����Ŏg�p).
	uint32_t mipMapCount;		///< �܂܂�Ă���~�j�}�b�v���x����.
	uint32_t reserved1[11];		///< (�����̂��߂ɗ\�񂳂�Ă���).
	DDSPixelFormat ddspf;		///< DDS�摜���.
	uint32_t caps[4];			///< �܂܂�Ă���摜�̎��.
	uint32_t reserved2;			///< (�����̂��߂ɗ\�񂳂�Ă���).
};

/*
�o�b�t�@����DDS�t�@�C���w�b�_��ǂݏo��.

@param buf �ǂݏo�����o�b�t�@.

@return �ǂݏo����DDS�t�@�C���w�b�_.
*/
DDSHeader ReadDDSHeader(const uint8_t* buf)
{
	DDSHeader tmp = {};
	tmp.size = Get(buf, 0, 4);
	tmp.flags = Get(buf, 4, 4);
	tmp.height = Get(buf, 8, 4);
	tmp.width = Get(buf, 12, 4);
	tmp.pitchOrLinearSize = Get(buf, 16, 4);
	tmp.depth = Get(buf, 20, 4);
	tmp.mipMapCount = Get(buf, 24, 4);
	tmp.ddspf = ReadDDSPixelFormat(buf + 28 + 4 * 11);
	for (int i = 0; i < 4; ++i) {
		tmp.caps[i] = Get(buf, 28 + 4 * 11 + 32 + 1 * 4, 4);
	}
	return tmp;
}

/*
DDS �t�@�C������e�N�X�`�����쐬����.

@param filename		DDS�t�@�C����.
@param st			DDS�t�@�C���X�e�[�^�X.
@param buf			�t�@�C����ǂݍ��񂾃o�b�t�@.
@param header		DDS�w�b�_�i�[��ւ̃|�C���^.

@retval 0 �ȊO		�쐬�����e�N�X�`��ID.
@retval 0			�쐬���s.
*/
GLuint LoadDDS(const char* filename, const struct stat& st, const uint8_t* buf, DDSHeader* pHeader)
{
	if (st.st_size < 128) {
		std::cerr << "WARNING: " << filename << "��DDS�t�@�C���ł͂���܂���." << std::endl;
		return 0;
	}
	const DDSHeader header = ReadDDSHeader(buf + 4);
	if (header.size != 124) {
		std::cerr << "WARNING: " << filename << "��DDS�t�@�C���ł͂���܂���." << std::endl;
		return 0;
	}

	GLenum iformat = GL_RGBA8;
	GLenum format = GL_RGBA;
	uint32_t blockSize = 16;
	bool isCompressed = false;
	if (header.ddspf.flags & 0x04) {
		// ���k�t�H�[�}�b�g
		switch (header.ddspf.fourCC)
		{
		case MAKE_FOURCC('D', 'X', 'T', '1'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case MAKE_FOURCC('D', 'X', 'T', '2'):
		case MAKE_FOURCC('D', 'X', 'T', '3'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case MAKE_FOURCC('D', 'X', 'T', '4'):
		case MAKE_FOURCC('D', 'X', 'T', '5'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		case MAKE_FOURCC('B', 'C', '4', 'U'):
			iformat = GL_COMPRESSED_RED_RGTC1;
			break;
		case MAKE_FOURCC('B', 'C', '4', 'S'):
			iformat = GL_COMPRESSED_SIGNED_RED_RGTC1;
			break;
		case MAKE_FOURCC('B', 'C', '5', 'U'):
			iformat = GL_COMPRESSED_RG_RGTC2;
			break;
		case MAKE_FOURCC('B', 'C', '5', 'S'):
			iformat = GL_COMPRESSED_SIGNED_RG_RGTC2;
			break;
		default:
			std::cerr << "WARNING: " << filename << "�͖��Ή���DDS�t�@�C���ł�." << std::endl;
			return 0;
		}
		isCompressed = true;
	}
	else if (header.ddspf.flags & 0x40) {
		//�����k�t�H�[�}�b�g
		if (header.ddspf.redBitMask == 0xff) {
			iformat = header.ddspf.alphaBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_RGBA : GL_RGB;
		}
		else if (header.ddspf.blueBitMask == 0xff) {
			iformat = header.ddspf.blueBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_BGRA : GL_BGR;
		}
		else {
			std::cerr << "WARNING: " << filename << "�͖��Ή���DDS�t�@�C���ł�." << std::endl;
			return 0;
		}
	}
	else {
		std::cerr << "WARNING: " << filename << "�͖��Ή���DDS�t�@�C���ł�." << std::endl;
		return 0;
	}

	const bool isCubemap = header.caps[1] & 0x200;
	const GLenum target = isCubemap ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_2D;
	const int faceCount = isCubemap ? 6 : 1;

	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texId);
	const uint8_t* data = buf + 128;
	for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
		GLsizei curWidth = header.width;
		GLsizei curHeight = header.height;
		for (int mipLevel = 0; mipLevel < static_cast<int>(header.mipMapCount); ++mipLevel) {
			uint32_t imageBytes;
			if (isCompressed) {
				imageBytes = ((curWidth + 3) / 4) * ((curHeight + 3) / 4) * blockSize;
				glCompressedTexImage2D(target + faceIndex, mipLevel, iformat,
					curWidth, curHeight, 0, imageBytes, data);
			}
			else
			{
				imageBytes = curWidth + curHeight * 4;
				glTexImage2D(target + faceIndex, mipLevel, iformat,
					curWidth, curHeight, 0, format, GL_UNSIGNED_BYTE, data);
			}
			const GLenum result = glGetError();
			if (result != GL_NO_ERROR) {
				//				std::cerr << "WARNING: " << filename << "�̓ǂݍ��݂Ɏ��s(" << std::hex << result << ")." << std::endl;
			}
			curWidth = std::max(1, curWidth / 2);
			curHeight = std::max(1, curHeight / 2);
			data += imageBytes;
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, header.mipMapCount - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		header.mipMapCount <= 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	*pHeader = header;
	return texId;
}

/*
�f�X�g���N�^.
*/
Texture::~Texture()
{
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/*
2D�e�N�X�`�����쐬����.

@param width   �e�N�X�`���̕�(�s�N�Z����).
@param height  �e�N�X�`���̍���(�s�N�Z����).
@param iformat �e�N�X�`���̃f�[�^�`��.
@param format  �A�N�Z�X����v�f.
@param data    �e�N�X�`���f�[�^�ւ̃|�C���^.

@return	�쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�.
		���s�����ꍇ��nullptr�Ԃ�.
*/
TexturePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data)
{
	struct Impl : Texture {};
	TexturePtr p = std::make_shared<Impl>();

	p->width = width;
	p->height = height;
	glGenTextures(1, &p->texId);
	glBindTexture(GL_TEXTURE_2D, p->texId);
	glTexImage2D(
		GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERROR �e�N�X�`���쐬�Ɏ��s: 0x" << std::hex << result << std::endl;
		return {};	
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return p;
}

/*
�t�@�C������2D�e�N�X�`����ǂݍ���.

@param	fileName	�t�@�C����.

@return	�쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�.
		���s�����ꍇ��nullptr��Ԃ�.
*/
TexturePtr Texture::LoadFromFile(const char* filename)
{
	//�t�@�C���T�C�Y���擾����.
	struct stat st;
	if (stat(filename, &st)) {
		return {};
	}

	// �t�@�C���T�C�Y��BMP�t�@�C���̏���ێ��ł���T�C�Y�����Ȃ��̃I�u�W�F�N�g��Ԃ�.
	const size_t bmpFileHeaderSize = 14; // �r�b�g�}�b�v�t�@�C���w�b�_�̃o�C�g��
	const size_t windowsV1HeaderSize = 40; // �r�b�g�}�b�v���w�b�_�̃o�C�g��.
	if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
		return {};
	}
	//�t�@�C�����J��.
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return {};
	}
	//�t�@�C����ǂݍ���.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return {};
	}
	//BMP�t�@�C���łȂ���΋�̃I�u�W�F�N�g��Ԃ�.
	const uint8_t* pHeader = buf.data();

	if (pHeader[0] == 'D' || pHeader[1] == 'D' || pHeader[2] == 'S' || pHeader[3] == ' ') {
		DDSHeader header;
		const GLuint texId = LoadDDS(filename, st, buf.data(), &header);
		if (texId) {
			struct impl : Texture {};
			TexturePtr p = std::make_shared<impl>();
			p->width = header.width;
			p->height = header.height;
			p->texId = texId;
			return p;
		}
	}

	if (pHeader[0] != 'B' || pHeader[1] != 'M') {
		return {};
	}
	//BMP�t�@�C���̏����擾����,
	const size_t offsetBytes = Get(pHeader, 10, 4);
	const uint32_t infoSize = Get(pHeader, 14, 4);
	const uint32_t width = Get(pHeader, 18, 4);
	const uint32_t height = Get(pHeader, 22, 4);
	const uint32_t bitCount = Get(pHeader, 28, 2);
	const uint32_t compression = Get(pHeader, 30, 4);
	const size_t pixelBytes = bitCount / 8;
	if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
		return{};
	}
	const size_t stride = ((width * pixelBytes + 3) / 4) * 4; //	���ۂ̉��o�C�g��.
	const size_t imageSize = stride * height;
	if (buf.size() < offsetBytes + imageSize) {
		return {};	
	}
	return Create(width, height, GL_RGB8, GL_BGR, buf.data() + offsetBytes);
}

/*
�t�@�C������2D�e�N�X�`����ǂݍ���.

@param	fileName	�t�@�C����.

@return	�쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�.
���s�����ꍇ��nullptr��Ԃ�.
*/
TexturePtr Texture::CreateCubeMap(std::vector<const char*> faces)
{
	struct Impl : Texture {};
	TexturePtr p = std::make_shared<Impl>();

	glGenTextures(1, &p->texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, p->texId);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		//�t�@�C���T�C�Y���擾����.
		struct stat st;
		if (stat(faces[i], &st)) {
			return {};
		}

		// �t�@�C���T�C�Y��BMP�t�@�C���̏���ێ��ł���T�C�Y�����Ȃ��̃I�u�W�F�N�g��Ԃ�.
		const size_t bmpFileHeaderSize = 14; // �r�b�g�}�b�v�t�@�C���w�b�_�̃o�C�g��
		const size_t windowsV1HeaderSize = 40; // �r�b�g�}�b�v���w�b�_�̃o�C�g��.
		if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
			return {};
		}
		//�t�@�C�����J��.
		FILE* fp = fopen(faces[i], "rb");
		if (!fp) {
			return {};
		}
		//�t�@�C����ǂݍ���.
		std::vector<uint8_t> buf;
		buf.resize(st.st_size);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return {};
		}
		//BMP�t�@�C���łȂ���΋�̃I�u�W�F�N�g��Ԃ�.
		const uint8_t* pHeader = buf.data();
		if (pHeader[0] != 'B' || pHeader[1] != 'M') {
			return {};
		}
		//BMP�t�@�C���̏����擾����,
		const size_t offsetBytes = Get(pHeader, 10, 4);
		const uint32_t infoSize = Get(pHeader, 14, 4);
		const uint32_t width = Get(pHeader, 18, 4);
		const uint32_t height = Get(pHeader, 22, 4);
		const uint32_t bitCount = Get(pHeader, 28, 2);
		const uint32_t compression = Get(pHeader, 30, 4);
		const size_t pixelBytes = bitCount / 8;
		if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
			return{};
		}
		const size_t stride = ((width * pixelBytes + 3) / 4) * 4; //	���ۂ̉��o�C�g��.
		const size_t imageSize = stride * height;
		if (buf.size() < offsetBytes + imageSize) {
			return {};
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, buf.data() + offsetBytes
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return p;
}
