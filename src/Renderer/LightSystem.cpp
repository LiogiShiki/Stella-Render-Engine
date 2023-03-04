#include "LightSystem.h"

namespace Stella
{
	void Stella::LightSystem::AddDirectionalLight(const DirectionalLight& light)
	{
		m_DirectionalLigths.push_back(light);
	}

	void Stella::LightSystem::SetShaderInputLight(Lumen::Shader& shader)
	{
		shader.Use();
		for (int i = 0; i < m_DirectionalLigths.size(); ++i)
		{
			DirectionalLight& dirLight = m_DirectionalLigths.at(i);
			shader.SetVector3f("lightDirections[" + std::to_string(i) + "]", dirLight.m_Direction);
			shader.SetVector3f("lightStrengths[" + std::to_string(i) + "]", dirLight.m_Strength);
		}
	}

	void LightSystem::SetShaderInputShadow(Lumen::Shader& shader)
	{
		shader.Use();
		//DirectionalLight& dirLight = m_DirectionalLigths.at(0);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, SHADOW_NEAR, SHADOW_FAR);
		lightProjection = glm::ortho(-120.0f, 120.0f, -120.0f, 120.0f, SHADOW_NEAR, SHADOW_FAR);
		lightView = glm::lookAt(100.0f*-GetDirectionalLight(0).m_Direction,glm::vec3(0.0f),  glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;


		std::cout << "old:"<<glm::to_string(lightSpaceMatrix) << std::endl;

		shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	}

	DirectionalLight& Stella::LightSystem::GetDirectionalLight(int number)
	{
		return m_DirectionalLigths.at(number);
	}
}


