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
バイト列から数値を復元する.

@param	p		バイト列へのポインタ
@param	offset	数値のオフセット
@param	size	数値のバイト数（１〜４）.

@return	復元した数値.
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
FOURCCを作成する。
*/
#define MAKE_FOURCC(a,b,c,d) \
	static_cast<uint32_t>(a + (b << 8) + (c << 16) + (d << 24))

/*
DDS 画像情報.
*/
struct DDSPixelFormat
{
	uint32_t size; ///< この構造体のバイト数(32).
	uint32_t flags; ///< 画像に含まれるデータの種類を示すフラグ.
	uint32_t fourCC; ///< 画像フォーマットを示すFOURCC.
	uint32_t rgbBitCount; ///< 1ピクセルのビット数.
	uint32_t redBitMask; ///< 赤要素が使う部分を示すビット.
	uint32_t greenBitMask; ///< 緑要素が使う部分を示すビット.
	uint32_t blueBitMask; ///< 青要素が使う部分を示すビット.
	uint32_t alphaBitMask; ///< 透明要素が使う部分を示すビット.
};

/*
バッファからDDS画像情報を読み出す.

@param buf 読み出し元バッファ.

@return 読み出したDDS画像提供.
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
DDSファイルヘッダ.
*/
struct DDSHeader
{
	uint32_t size;				///< この構造体のバイト数(124).
	uint32_t flags;				///< どのパラメーターが有効かを示すフラグ.
	uint32_t height;			///< 画像の高さ(ピクセル数).
	uint32_t width;				///< 画像の幅(ピクセル数).
	uint32_t pitchOrLinearSize;	///< 横のバイト数または画像1枚のバイト数.
	uint32_t depth;				///< 画像の奥行き(枚数)(3次元テクスチャ等で使用).
	uint32_t mipMapCount;		///< 含まれているミニマップレベル数.
	uint32_t reserved1[11];		///< (将来のために予約されている).
	DDSPixelFormat ddspf;		///< DDS画像情報.
	uint32_t caps[4];			///< 含まれている画像の種類.
	uint32_t reserved2;			///< (将来のために予約されている).
};

/*
バッファからDDSファイルヘッダを読み出す.

@param buf 読み出し元バッファ.

@return 読み出したDDSファイルヘッダ.
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
DDS ファイルからテクスチャを作成する.

@param filename		DDSファイル名.
@param st			DDSファイルステータス.
@param buf			ファイルを読み込んだバッファ.
@param header		DDSヘッダ格納先へのポインタ.

@retval 0 以外		作成したテクスチャID.
@retval 0			作成失敗.
*/
GLuint LoadDDS(const char* filename, const struct stat& st, const uint8_t* buf, DDSHeader* pHeader)
{
	if (st.st_size < 128) {
		std::cerr << "WARNING: " << filename << "はDDSファイルではありません." << std::endl;
		return 0;
	}
	const DDSHeader header = ReadDDSHeader(buf + 4);
	if (header.size != 124) {
		std::cerr << "WARNING: " << filename << "はDDSファイルではありません." << std::endl;
		return 0;
	}

	GLenum iformat = GL_RGBA8;
	GLenum format = GL_RGBA;
	uint32_t blockSize = 16;
	bool isCompressed = false;
	if (header.ddspf.flags & 0x04) {
		// 圧縮フォーマット
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
			std::cerr << "WARNING: " << filename << "は未対応のDDSファイルです." << std::endl;
			return 0;
		}
		isCompressed = true;
	}
	else if (header.ddspf.flags & 0x40) {
		//無圧縮フォーマット
		if (header.ddspf.redBitMask == 0xff) {
			iformat = header.ddspf.alphaBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_RGBA : GL_RGB;
		}
		else if (header.ddspf.blueBitMask == 0xff) {
			iformat = header.ddspf.blueBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_BGRA : GL_BGR;
		}
		else {
			std::cerr << "WARNING: " << filename << "は未対応のDDSファイルです." << std::endl;
			return 0;
		}
	}
	else {
		std::cerr << "WARNING: " << filename << "は未対応のDDSファイルです." << std::endl;
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
				//				std::cerr << "WARNING: " << filename << "の読み込みに失敗(" << std::hex << result << ")." << std::endl;
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
デストラクタ.
*/
Texture::~Texture()
{
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/*
2Dテクスチャを作成する.

@param width   テクスチャの幅(ピクセル数).
@param height  テクスチャの高さ(ピクセル数).
@param iformat テクスチャのデータ形式.
@param format  アクセスする要素.
@param data    テクスチャデータへのポインタ.

@return	作成に成功した場合はテクスチャポインタを返す.
		失敗した場合はnullptr返す.
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
		std::cerr << "ERROR テクスチャ作成に失敗: 0x" << std::hex << result << std::endl;
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
ファイルから2Dテクスチャを読み込む.

@param	fileName	ファイル名.

@return	作成に成功した場合はテクスチャポインタを返す.
		失敗した場合はnullptrを返す.
*/
TexturePtr Texture::LoadFromFile(const char* filename)
{
	//ファイルサイズを取得する.
	struct stat st;
	if (stat(filename, &st)) {
		return {};
	}

	// ファイルサイズがBMPファイルの情報を保持できるサイズ未満なら空のオブジェクトを返す.
	const size_t bmpFileHeaderSize = 14; // ビットマップファイルヘッダのバイト数
	const size_t windowsV1HeaderSize = 40; // ビットマップ情報ヘッダのバイト数.
	if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
		return {};
	}
	//ファイルを開く.
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return {};
	}
	//ファイルを読み込む.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return {};
	}
	//BMPファイルでなければ空のオブジェクトを返す.
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
	//BMPファイルの情報を取得する,
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
	const size_t stride = ((width * pixelBytes + 3) / 4) * 4; //	実際の横バイト数.
	const size_t imageSize = stride * height;
	if (buf.size() < offsetBytes + imageSize) {
		return {};	
	}
	return Create(width, height, GL_RGB8, GL_BGR, buf.data() + offsetBytes);
}

/*
ファイルから2Dテクスチャを読み込む.

@param	fileName	ファイル名.

@return	作成に成功した場合はテクスチャポインタを返す.
失敗した場合はnullptrを返す.
*/
TexturePtr Texture::CreateCubeMap(std::vector<const char*> faces)
{
	struct Impl : Texture {};
	TexturePtr p = std::make_shared<Impl>();

	glGenTextures(1, &p->texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, p->texId);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		//ファイルサイズを取得する.
		struct stat st;
		if (stat(faces[i], &st)) {
			return {};
		}

		// ファイルサイズがBMPファイルの情報を保持できるサイズ未満なら空のオブジェクトを返す.
		const size_t bmpFileHeaderSize = 14; // ビットマップファイルヘッダのバイト数
		const size_t windowsV1HeaderSize = 40; // ビットマップ情報ヘッダのバイト数.
		if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
			return {};
		}
		//ファイルを開く.
		FILE* fp = fopen(faces[i], "rb");
		if (!fp) {
			return {};
		}
		//ファイルを読み込む.
		std::vector<uint8_t> buf;
		buf.resize(st.st_size);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return {};
		}
		//BMPファイルでなければ空のオブジェクトを返す.
		const uint8_t* pHeader = buf.data();
		if (pHeader[0] != 'B' || pHeader[1] != 'M') {
			return {};
		}
		//BMPファイルの情報を取得する,
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
		const size_t stride = ((width * pixelBytes + 3) / 4) * 4; //	実際の横バイト数.
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
