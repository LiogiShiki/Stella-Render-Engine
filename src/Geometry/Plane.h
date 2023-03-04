#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include"Graphics/VertexArray.h"
#include"Graphics/VertexBuffer.h"
#include"Graphics/Shader.h"

#include"Renderer/camera.h"

namespace Stella
{
	class Plane
	{
	public:
		Plane();
		~Plane();
		void Draw(Lumen::Shader& shader,std::shared_ptr<Camera> camera);
		void DrawGbuffer(Lumen::Shader& shader);
		void DrawShadow(Lumen::Shader& shader);
	private:
		Lumen::VertexArray PlaneVAO;
		Lumen::VertexBuffer PlaneVBO;
		glm::mat4 m_Model = glm::mat4(1.0f);

		glm::vec3 albedo = {0.9,0.9,0.9};
		float metallic = 0.32;
		float roughness = 0.5;
		float ao = 1.0;

	};
}


