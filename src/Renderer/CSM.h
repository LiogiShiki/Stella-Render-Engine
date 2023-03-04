#pragma once
#include"glm/glm.hpp"
#include<vector>
#include<memory>
#include"Renderer/camera.h"
#include"Definitions.h"
#include<iostream>
#include <glm/gtx/string_cast.hpp>



namespace Stella
{
	struct CSMData
	{
		std::vector<float> m_shadowCascadeLevels;
		std::shared_ptr<Camera> m_Camera;
        glm::vec3 m_lightDir;
		unsigned int m_CSMubo;


		CSMData(std::shared_ptr<Camera> camera, glm::vec3 lightDir):m_Camera(camera),m_lightDir(lightDir)
		{
			glGenBuffers(1, &m_CSMubo);
			glBindBuffer(GL_UNIFORM_BUFFER, m_CSMubo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_CSMubo); // 0 for rsm, 1 for camera, 2 for taa
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			m_shadowCascadeLevels.push_back(m_Camera->far / 50.0f);
			m_shadowCascadeLevels.push_back(m_Camera->far / 25.0f);
			m_shadowCascadeLevels.push_back(m_Camera->far / 10.0f);
			m_shadowCascadeLevels.push_back(m_Camera->far / 2.0f);
		}

		void Updata()
		{
			const auto lightMatrices = getLightSpaceMatrices();

			std::cout << "Updata csm" << std::endl;
			for (const auto& lm : lightMatrices)
			{
			
				std::cout << glm::to_string(lm) << std::endl;
			}
			std::cout << "Updata csm end" << std::endl;


			glBindBuffer(GL_UNIFORM_BUFFER, m_CSMubo);
			for (size_t i = 0; i < lightMatrices.size(); ++i)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
			}
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

        std::vector<glm::vec4> CaculateWorldFrustumCorners(const float nearPlane, const float farPlane);


		glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
		std::vector<glm::mat4> getLightSpaceMatrices();

	};
}

