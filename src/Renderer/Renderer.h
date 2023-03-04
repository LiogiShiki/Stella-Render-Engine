#pragma once
#include"Graphics/Shader.h"
#include"Graphics/Framebuffer.h"
#include"Graphics/FrameBufferCube.h"
#include"Renderer/ShaderManager.h"
#include"Graphics/Texture.h"
#include"Renderer/camera.h"
#include"Model/model.h"
#include<memory>
#include<iostream>
#include<algorithm>
#include<random>
#include"Geometry/Plane.h"
#include"Geometry/Sphere.h"
#include"Geometry/Cube.h"
#include"Geometry/Quad.h"
#include"Definitions.h"
#include"LightSystem.h"
#include"TAA.h"
#include"CSM.h"
#include"shader.h"

namespace Stella
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void Initialize();
		void Render();
		std::shared_ptr<Camera> m_Camera;
		unsigned int debugLayer = 0;

	private:
		Lumen::Shader& BlinnPhongShader = Lumen::ShaderManager::GetShader("Basic_BlinnPhong");
		Lumen::Shader& PBRShader = Lumen::ShaderManager::GetShader("PBR");
		Lumen::Shader& PBRModelShader = Lumen::ShaderManager::GetShader("PBRModel");
		Lumen::Shader& GBufferShader = Lumen::ShaderManager::GetShader("GBufferPass");
		Lumen::Shader& GBufferNontexShader = Lumen::ShaderManager::GetShader("GBufferPassNontex");
		Lumen::Shader& LightingShader = Lumen::ShaderManager::GetShader("LightingPass");
		Lumen::Shader& ShadowMapShader = Lumen::ShaderManager::GetShader("ShdowMapPass");

		Lumen::Shader& CSMShader = Lumen::ShaderManager::GetShader("CSMShader");


		Lumen::Shader& ShadowDebugShader = Lumen::ShaderManager::GetShader("ShadowDebug");
		Lumen::Shader& equirectangularToCubeShader = Lumen::ShaderManager::GetShader("equirectangularToCube");
		Lumen::Shader& skyboxShader = Lumen::ShaderManager::GetShader("skyboxShader");
		Lumen::Shader& EnvPrefilterShader = Lumen::ShaderManager::GetShader("envPrefilter");
		Lumen::Shader& EnvPrefilterSpecShader = Lumen::ShaderManager::GetShader("envPrefilterSpec");
		Lumen::Shader& HizPassShader = Lumen::ShaderManager::GetShader("HizPassShader");
		Lumen::Shader& RSMGbufferPassShader = Lumen::ShaderManager::GetShader("RSMGbufferPassShader");
		Lumen::Shader& brdfShader = Lumen::ShaderManager::GetShader("brdfShader");


		Lumen::Shader& finalShader = Lumen::ShaderManager::GetShader("finalShader");
		Lumen::Shader& taaResolveShader = Lumen::ShaderManager::GetShader("taaResolveShader");


		Plane m_Plane;
		std::unique_ptr<Lumen::Texture>	PlaneTexture;
		std::unique_ptr<Sphere> m_Sphere;
		std::vector<std::unique_ptr<Sphere>> m_IBLdemoSpheres;
		std::vector<std::unique_ptr<Sphere>> m_CSMdemoSpheres;
		std::unique_ptr<Cube> m_Cube;
		std::unique_ptr<Model> m_TestModel;
		std::unique_ptr<Model> m_SponzaModel;

		std::unique_ptr<Lumen::Framebuffer>m_GBuffer;
		std::unique_ptr<Lumen::Framebuffer>m_ShadowMap;
		std::unique_ptr<Lumen::Framebuffer>m_RSMGbuffer;
		std::unique_ptr<Lumen::FrameBufferCube>m_envmapCaptureFBO;
		std::unique_ptr<Lumen::Framebuffer>m_LightingBuffer;
		std::vector<std::unique_ptr<Lumen::Framebuffer>>m_taaResolve;

		Quad m_ScreenQuad;

		// Lighting
		LightSystem m_LightSystem;

		std::unique_ptr<Lumen::Texture>	m_HDRenvmap;
		unsigned int irradianceMap;
		unsigned int specularEnvmap;
		unsigned int m_BRDFlut;

		// SSR
		unsigned int HizFrameBuffer;

		// RSM
		unsigned int RSM_ubo;

		// TAA
		unsigned int uboMatrices;
		std::unique_ptr<TaaData> m_TaaData;
		long long frameCount = 1;
		unsigned int HistoryTexture;

		// CSM
		unsigned int m_CSMfbo;
		unsigned int m_CSMtex;
		unsigned int m_CSMubo;
		std::unique_ptr<CSMData>m_CSMData;
		std::unique_ptr<Shader>newCSMshader;
	




	private:

		void DebugInit();
		void LightInit();

		void ShaderInputInit();

		void UniformBufferInit();

		void UniformBufferUpdate();

		void FrameBufferInit();
		void EnvMapInit();
		void RSMInit();
		void TAAInit();
		void CSMInit();
			
	};

	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
}



