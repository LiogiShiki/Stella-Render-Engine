#pragma once

#include "stb_image.h"
#include <unordered_map>
#include <glad/glad.h>

#include <string>
#include <array>

#include<iostream>
namespace Lumen
{
	class CubeTexture
	{
	public:
		CubeTexture() = default;
		
		

		CubeTexture(const std::string& path, GLenum internalFormat, GLenum format, GLenum type, 
			bool hdr = false, bool mipmap = false, bool flip = false) :m_width(0),m_height(0)
		{

		}

		CubeTexture(int w, int h,GLenum internalFormat,GLenum format,GLenum type, bool hdr = false, bool mipmap = false, bool flip = false)
			:m_width(w),m_height(h)
		{
			glGenTextures(1, &m_Texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0, format, type, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

	private:

		int m_width;
		int m_height;
		GLuint m_Texture = 0;
	};
}


