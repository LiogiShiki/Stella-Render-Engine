#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include"Graphics/VertexArray.h"
#include"Graphics/VertexBuffer.h"
#include"Graphics/IndexBuffer.h"
#include"Graphics/Shader.h"
#include"Graphics/Texture.h"
#include"Renderer/camera.h"
#include"Definitions.h"
#include<string>
#include<memory>
#include"Renderer/shader.h"

namespace Stella
{
	class Sphere
	{
	public:
		Sphere(const std::string& path);
		~Sphere();
		void Draw(Lumen::Shader& shader, std::shared_ptr<Camera> camera);
		void DrawNonTex(Lumen::Shader& shader,std::shared_ptr<Camera> camera);
		void DrawShadow(Lumen::Shader& shader);
		void DrawShadow(std::unique_ptr<Shader>& shader);
		glm::mat4 m_Model = glm::mat4(1.0f);
	private:
		Lumen::VertexArray SphereVAO;
		Lumen::VertexBuffer SphereVBO;
		Lumen::IndexBuffer SphereEBO;
		unsigned int indexCount;
		

		// Textures
		std::string TexturePath;
		std::unique_ptr<Lumen::Texture> albedoMap;
		std::unique_ptr<Lumen::Texture> aoMap;
		std::unique_ptr<Lumen::Texture> normalMap;
		std::unique_ptr<Lumen::Texture> metallicRoughnessMap;

		// matrial parameters
	public:
		glm::vec3 albedo = { 0.78f, 0.78f, 0.78f };
		float metallic = 0.32;
		float roughness = 0.1;
		float ao = 1.0;
		
	};
}


