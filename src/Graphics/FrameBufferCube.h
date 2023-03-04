#pragma once
#include <iostream>
#include <string>
#include <glad/glad.h>
#include "Logger.h"
#include <vector>

#include <glm/glm.hpp>


namespace Lumen
{

	class FrameBufferCube
	{
	public:
		FrameBufferCube(int w,int h,GLenum internalFormat = GL_RGB16F):m_FBWidth(w),m_FBHeight(h)
		{

			// Depth
			glGenFramebuffers(1, &m_FBO);
			glGenRenderbuffers(1, &m_RBO);

			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

			// Generate Cube Texture
			glGenTextures(1, &m_cubeTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTexture);
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}

		inline void Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		}

	public:

		GLuint m_FBO = 0; 
		GLuint m_cubeTexture = 0;
		GLuint m_RBO = 0;
		int m_FBWidth;
		int m_FBHeight;
	};
}



