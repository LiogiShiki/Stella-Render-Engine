#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include<vector>

#include"Graphics/Shader.h"
#include"Definitions.h"
#include<algorithm>

namespace Stella
{

	enum class LightType
	{
		DirectionalLight = 0,
		PointLight = 1,
	};

	struct Light
	{
		LightType m_type;
		glm::vec3 m_Strength;

		Light(const LightType& type, const glm::vec3& strength) :m_type(type), m_Strength(strength)
		{

		}
		virtual ~Light() {};
	};

	struct DirectionalLight: Light
	{
		glm::vec3 m_Direction;

		DirectionalLight(const LightType& type, const glm::vec3& strength, const glm::vec3& direction) 
			:Light(type, strength), m_Direction(direction)
		{

		}

		~DirectionalLight() = default;
	};


	class LightSystem
	{

	public:
		LightSystem() = default;
		~LightSystem() = default;
		void AddDirectionalLight(const DirectionalLight& light);
		void SetShaderInputLight(Lumen::Shader& shader);
		void SetShaderInputShadow( Lumen::Shader& shader);
		DirectionalLight& GetDirectionalLight(int number);

	private:

		std::vector<DirectionalLight> m_DirectionalLigths;
		std::vector<Light> m_PointLigths;
	};
}



