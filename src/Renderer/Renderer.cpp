#include "Renderer.h"



namespace Stella
{

	

	GLenum glCheckError_(const char* file, int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			std::cout << error << " | " << file << " (" << line << ")" << std::endl;
		}
		return errorCode;
	}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;
	}

	Renderer::Renderer()
	{
		Initialize();

	}
	Renderer::~Renderer()
	{
	}

	void Renderer::DebugInit()
	{
		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

	void Renderer::LightInit()
	{

		m_LightSystem.AddDirectionalLight(
			DirectionalLight
		(
			LightType::DirectionalLight,
			glm::vec3(650.0f, 650.0f, 650.0f),
			glm::vec3(0.01f, -0.35f, 0.15f)
		));

		m_LightSystem.SetShaderInputLight(LightingShader);
		m_LightSystem.SetShaderInputLight(BlinnPhongShader);
		m_LightSystem.SetShaderInputLight(PBRModelShader);
		m_LightSystem.SetShaderInputLight(PBRShader);
		m_LightSystem.SetShaderInputLight(RSMGbufferPassShader);
 
		m_LightSystem.SetShaderInputShadow(ShadowMapShader);
		m_LightSystem.SetShaderInputShadow(BlinnPhongShader);
		m_LightSystem.SetShaderInputShadow(LightingShader);
	}

	void Renderer::ShaderInputInit()
	{

	}

	void Renderer::UniformBufferInit()
	{
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboMatrices); // links to binding point 1£¬ 0 already for rRSM
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GBufferShader.BindUBOToBindingPoint("Matrices", 1); //Binding Point is 1 (Where the Matrices ubo is)
		GBufferNontexShader.BindUBOToBindingPoint("Matrices", 1);

		glm::mat4 projection = m_Camera->GetProjectionMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);




	}

	void Renderer::UniformBufferUpdate()
	{
		glm::mat4 view = m_Camera->GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		//std::cout << view[0][0] << view[0][1]<< view[0][2]<< view[0][3]<< std::endl;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}



	void Renderer::FrameBufferInit()
	{
		std::vector<Lumen::FORMAT> gbuffer_format
		{
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // World Position
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // Normals
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // albedo
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true },	// metallic roughness ao
			{ GL_RG16F,   GL_RG, GL_FLOAT, true, true}	// velocity
		};
		m_GBuffer = std::make_unique<Lumen::Framebuffer>(SCR_WIDTH, SCR_HEIGHT, gbuffer_format, true, true,false);

		std::vector<Lumen::FORMAT> shadowmap_format
		{
			// No Color Attachments;
		};
		m_ShadowMap = std::make_unique<Lumen::Framebuffer>(SHADOW_RESOLUTION_WIDTH, SHADOW_RESOLUTION_HEIGHT,
			shadowmap_format, true, true, false);

		std::vector<Lumen::FORMAT> RSM_gbuffer_format
		{
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // World Position
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // Normals
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // albedo
		};

		m_RSMGbuffer = std::make_unique<Lumen::Framebuffer>(RSM_RESOLUTION_WIDTH, RSM_RESOLUTION_HEIGHT,
			RSM_gbuffer_format, true, true, false);

		std::vector<Lumen::FORMAT>Lighting_format
		{
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true },
		};
		m_LightingBuffer = std::make_unique<Lumen::Framebuffer>(SCR_WIDTH, SCR_HEIGHT,
			Lighting_format, true, true, false);

		std::vector<Lumen::FORMAT>TAAResolve_format
		{
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true }, // History
			//{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true } //  Now
		};
		std::vector<Lumen::FORMAT>TAAResolve_format_h
		{
			{ GL_RGBA16F, GL_RGBA, GL_FLOAT, false, false }, // History
			//{ GL_RGBA16F, GL_RGBA, GL_FLOAT, true, true } //  Now
		};
		m_taaResolve.push_back(std::make_unique<Lumen::Framebuffer>(SCR_WIDTH, SCR_HEIGHT,
			TAAResolve_format_h, true, true, false));
		m_taaResolve.push_back(std::make_unique<Lumen::Framebuffer>(SCR_WIDTH, SCR_HEIGHT,
			TAAResolve_format, true, true, false));

	}

	void Renderer::EnvMapInit()
	{
		m_envmapCaptureFBO = std::make_unique<Lumen::FrameBufferCube>(512,512);
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		auto path = std::string("src/assets/skyboxes/monoLake/monoLake.hdr");
		float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
		unsigned int hdrTexture;
		if (data)
		{
			glGenTextures(1, &hdrTexture);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load HDR image." << std::endl;
		}

		glViewport(0, 0, 512, 512);
		m_envmapCaptureFBO->Bind();
		equirectangularToCubeShader.Use();
		equirectangularToCubeShader.SetInteger("equirectangularMap", 0);
		equirectangularToCubeShader.SetMatrix4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		for (unsigned int i = 0; i < 6; ++i)
		{
			equirectangularToCubeShader.SetMatrix4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envmapCaptureFBO->m_cubeTexture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_Cube->Draw(equirectangularToCubeShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmapCaptureFBO->m_cubeTexture);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		
		// Convolution for diffuse IBL
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, m_envmapCaptureFBO->m_FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_envmapCaptureFBO->m_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		EnvPrefilterShader.Use();
		EnvPrefilterShader.SetInteger("environmentMap", 0);
		EnvPrefilterShader.SetMatrix4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmapCaptureFBO->m_cubeTexture);

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, m_envmapCaptureFBO->m_FBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			EnvPrefilterShader.SetMatrix4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_Cube->Draw(EnvPrefilterShader);

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Specular Monte Carlo
		glGenTextures(1, &specularEnvmap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, specularEnvmap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 256, 256, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		EnvPrefilterSpecShader.Use();
		EnvPrefilterSpecShader.SetInteger("environmentMap", 0);
		EnvPrefilterSpecShader.SetMatrix4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmapCaptureFBO->m_cubeTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, m_envmapCaptureFBO->m_FBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth  = static_cast<unsigned int>(256 * std::pow(0.5, mip));
			unsigned int mipHeight = static_cast<unsigned int>(256 * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, m_envmapCaptureFBO->m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			EnvPrefilterSpecShader.SetFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				EnvPrefilterSpecShader.SetMatrix4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularEnvmap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_Cube->Draw(EnvPrefilterSpecShader);

			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// BRDF lut
		//   unsigned int brdfLUTTexture;
		glGenTextures(1, &m_BRDFlut);
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, m_BRDFlut);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFlut, 0);

		glViewport(0, 0, 512, 512);
		brdfShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_ScreenQuad.Draw(brdfShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);



	}

	void Renderer::CSMInit()
	{
		newCSMshader = std::make_unique<Shader>("src/Shader/csm.vs", "src/Shader/csm.fs", "src/Shader/csm.gs");


		auto dir = -m_LightSystem.GetDirectionalLight(0).m_Direction;
		//dir = glm::normalize(dir);
		m_CSMData = std::make_unique<CSMData>(m_Camera, dir);

		glGenFramebuffers(1, &m_CSMfbo);
		glGenTextures(1, &m_CSMtex);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_CSMtex);
		glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_RESOLUTION_WIDTH, SHADOW_RESOLUTION_HEIGHT, 
			int(m_CSMData->m_shadowCascadeLevels.size()) + 1,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

		glBindFramebuffer(GL_FRAMEBUFFER, m_CSMfbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CSMtex, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
			throw 0;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glGenBuffers(1, &m_CSMubo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_CSMubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_CSMubo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Renderer::RSMInit()
	{	

		std::vector<glm::vec4> sampleCoordsandWeights;
		std::default_random_engine e;
		std::uniform_real_distribution<float> u(0, 1);
		for (int i = 0; i < RSM_SAMPLE_COUNT; i++) 
		{
			float xi1 = u(e);
			float xi2 = u(e);
			sampleCoordsandWeights.push_back({ xi1 * sin(2 * PI * xi2), xi1 * cos(2 * PI * xi2), xi1 * xi1, 0 });
		}
		unsigned int uniformBlockIndex = glGetUniformBlockIndex(LightingShader.GetProgramID(), "randomMap");
		glUniformBlockBinding(LightingShader.GetProgramID(), uniformBlockIndex, 0);

		glGenBuffers(1, &RSM_ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, RSM_ubo);
		glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(GL_FLOAT) * sampleCoordsandWeights.size(), 
			sampleCoordsandWeights.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, RSM_ubo, 0, 4 * sizeof(GL_FLOAT) * sampleCoordsandWeights.size());
	}

	void Renderer::TAAInit()
	{
		m_TaaData = std::make_unique<TaaData>();
		m_TaaData->prevView = m_Camera->GetViewMatrix();
		m_TaaData->prevProj = m_Camera->GetProjectionMatrix();

		for (auto& halton : m_TaaData->haltonSequence)
		{
			std::cout << halton.x << ',' << halton.y << std::endl;
		}

		glGenBuffers(1, &m_TaaData->haltonUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_TaaData->haltonUbo);
		glBufferData(GL_UNIFORM_BUFFER,  sizeof(glm::vec4) * m_TaaData->haltonSequence.size(),
			m_TaaData->haltonSequence.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_TaaData->haltonUbo);
		// link to binding point 2

		GBufferShader.BindUBOToBindingPoint("Halton_23", 2);
		GBufferNontexShader.BindUBOToBindingPoint("Halton_23", 2);

	}

	void Renderer::Initialize()
	{
		// objects
		m_Sphere = std::make_unique<Sphere>("rusted_iron");
		m_Cube = std::make_unique<Cube>();
		
		int nrRows = 5;
		int nrColumns = 5;
		for (int i = 0;i< nrRows * nrColumns;++i)
		{
			m_IBLdemoSpheres.push_back(std::make_unique<Sphere>("rusted_iron"));
		}

		for (int i = 0; i < 10; ++i)
		{
			m_CSMdemoSpheres.push_back(std::make_unique<Sphere>("rusted_iron"));
			m_CSMdemoSpheres[i]->m_Model = glm::translate(m_CSMdemoSpheres[i]->m_Model, glm::vec3(2.0, -12.0, 6.0 * i));
			m_CSMdemoSpheres[i]->m_Model = glm::scale(m_CSMdemoSpheres[i]->m_Model, glm::vec3(1.5f));
		}

		glm::mat4 model = glm::mat4(1.0f);
		float spacing = 2.5;

		for (int row = 0; row < 5; ++row)
		{

			for (int col = 0; col < 5; ++col)
			{
				m_IBLdemoSpheres[5 * row + col]->metallic = (float)row / (float)nrRows;
				m_IBLdemoSpheres[5 * row + col]->roughness = glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f);
				//std::cout <<"metallic"<< m_IBLdemoSpheres[5 * row + col]->metallic << std::endl;
				//std::cout <<"rpughness"<< m_IBLdemoSpheres[5 * row + col]->roughness << std::endl;

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing,
					-2.0f
				));
				m_IBLdemoSpheres[5 * row + col]->m_Model = model;

			}
		}

		TransformParameters t;
		m_TestModel = std::make_unique<Model>(
			("D:/VSProject/ToyEngine/src/assets/models/DamagedHelmet/DamagedHelmet.gltf"), t, false);
		m_SponzaModel = std::make_unique<Model>(
			("D:/VSProject/ToyEngine/src/assets/models/Sponza/Sponza.gltf"), t, false);
		m_SponzaModel->m_Model = glm::scale(m_SponzaModel->m_Model, glm::vec3(0.005f));
		m_TestModel->m_Model = glm::scale(m_TestModel->m_Model, glm::vec3(2.0f));
		m_TestModel->m_Model = glm::translate(m_TestModel->m_Model, glm::vec3(0.0f,0.2f,0.0f));

		m_Camera = std::make_shared<Camera>();
		PlaneTexture = std::make_unique<Lumen::Texture>();
		PlaneTexture->CreateTexture("src/assets/textures/wood.png", false, false);
		m_HDRenvmap = std::make_unique<Lumen::Texture>();
		m_HDRenvmap->CreateTexture("src/assets/hdr/newport_loft.hdr", true, false,true);


		// FrameBuffers
		FrameBufferInit();

		// UniformBuffers
		UniformBufferInit();


		// Render States
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Debug
		DebugInit();

		//Light and Shadow
		LightInit();

		// shader input
		ShaderInputInit();

		// EnvMap 
		EnvMapInit();

		// RSM
		RSMInit();

		// TAA
		TAAInit();

		// CSM
		CSMInit();

	}


	void Renderer::Render()
	{
		UniformBufferUpdate();
		//m_CSMData->Updata();



		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		//// Shadow Pass





		m_ShadowMap->Bind();
			ShadowMapShader.Use();
			//ShadowMapShader.BindUBOToBindingPoint("LightSpaceMatrices",3);
			glViewport(0, 0, SHADOW_RESOLUTION_WIDTH, SHADOW_RESOLUTION_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			//m_SponzaModel->drawShadow(ShadowMapShader);
			
			//m_TestModel->drawShadow(ShadowMapShader);
			//m_Plane.Draw(ShadowMapShader);
			m_Sphere->DrawShadow(ShadowMapShader);

		m_ShadowMap->Unbind();

		// CSM

		const auto lightMatrices = m_CSMData->getLightSpaceMatrices();
		glBindBuffer(GL_UNIFORM_BUFFER, m_CSMubo);
		//std::cout << "update csm" << std::endl;
		for (size_t i = 0; i < lightMatrices.size(); ++i)
		{
			//std::cout << glm::to_string(lightMatrices[i]) << std::endl;
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), &lightMatrices[i]);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		//std::cout << "update csm end" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, m_CSMfbo);
		newCSMshader->use();
		//CSMShader.BindUBOToBindingPoint("LightSpaceMatrices", 3);
			glViewport(0, 0, SHADOW_RESOLUTION_WIDTH, SHADOW_RESOLUTION_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_FRONT);  // peter panning
			m_Sphere->DrawShadow(newCSMshader);
			for (const auto& sphere : m_CSMdemoSpheres)
			{
				sphere->DrawShadow(newCSMshader);
			}
			glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// RSM Pass
		m_RSMGbuffer->Bind();
			glViewport(0, 0, RSM_RESOLUTION_WIDTH, RSM_RESOLUTION_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RSMGbufferPassShader.Use();
			//DirectionalLight& dirLight = m_LightSystem.GetDirectionalLight(0);
			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, SHADOW_NEAR, SHADOW_FAR);
			lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, RSM_NEAR, RSM_FAR);
			lightView = glm::lookAt(50.0f * -m_LightSystem.GetDirectionalLight(0).m_Direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;
			RSMGbufferPassShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
			LightingShader.Use();
			LightingShader.SetMatrix4("lightSpaceMatrixRSM", lightSpaceMatrix);  // for lighitng shader too
			m_SponzaModel->DrawRSMGbuffer(RSMGbufferPassShader);
		m_RSMGbuffer->Unbind();



		// Gbuffer Pass

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		m_GBuffer->Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
			GBufferShader.Use();
			GBufferShader.SetFloat("screenWidth", float(SCR_WIDTH));
			GBufferShader.SetFloat("screenHeight", float(SCR_HEIGHT));
			GBufferShader.SetInteger("frameCount", frameCount); // for taa halton index
			GBufferShader.SetMatrix4("prevView", m_TaaData->prevView);
			GBufferShader.SetMatrix4("prevProj", m_TaaData->prevProj);

			GBufferNontexShader.Use();
			GBufferNontexShader.SetFloat("screenWidth", float(SCR_WIDTH));
			GBufferNontexShader.SetFloat("screenHeight", float(SCR_HEIGHT));
			GBufferNontexShader.SetInteger("frameCount", frameCount); // for taa halton index
			GBufferNontexShader.SetMatrix4("prevView", m_TaaData->prevView);
			GBufferNontexShader.SetMatrix4("prevProj", m_TaaData->prevProj);


			//m_SponzaModel->Draw(GBufferShader, m_Camera,true);
			//m_Sphere->Draw(GBufferShader, m_Camera);
			//m_TestModel->Draw(GBufferShader, m_Camera,true);
			m_Sphere->DrawNonTex(GBufferNontexShader, m_Camera);
			for (const auto& sphere : m_IBLdemoSpheres)
			{
				//sphere->DrawNonTex(GBufferNontexShader,m_Camera);
			}
			for (const auto& sphere : m_CSMdemoSpheres)
			{
				sphere->DrawNonTex(GBufferNontexShader,m_Camera);
			}
			m_Plane.Draw(GBufferNontexShader,m_Camera);
		m_GBuffer->Unbind();

		// SSR HiZ Pass
		m_GBuffer->Bind();
		HizPassShader.Use();
		HizPassShader.SetInteger("u_depthBuffer",0);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetDepthBuffer());
		glGenerateMipmap(GL_TEXTURE_2D); // Generate mimap for depthBuffer

		glDepthFunc(GL_ALWAYS);
		// calculate the number of mipmap levels for NPOT texture
		int numLevels = 1 + (int)floorf(log2f(fmaxf(SCR_WIDTH, SCR_HEIGHT)));
		int currentWidth = SCR_WIDTH;
		int currentHeight = SCR_HEIGHT;

		for (int i = 1; i < numLevels; i++) 
		{
			glUniform2i(glGetUniformLocation(HizPassShader.GetProgramID(), "u_previousLevelDimensions"), currentWidth, currentHeight);
			HizPassShader.SetInteger("u_previousLevel", i - 1);
			// calculate next viewport size
			currentWidth /= 2;
			currentHeight /= 2;
			// ensure that the viewport size is always at least 1x1
			currentWidth = currentWidth > 0 ? currentWidth : 1;
			currentHeight = currentHeight > 0 ? currentHeight : 1;
			glViewport(0, 0, currentWidth, currentHeight);
			// bind next level for rendering but first restrict fetches only to previous level
	 /*       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i - 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i - 1);*/
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBuffer->GetDepthBuffer(), i);
			// dummy draw command as the full screen quad is generated completely by a geometry shader
			m_ScreenQuad.Draw(HizPassShader);

		}

		//// reset mipmap level range for the depth image
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numLevels - 1);
		// reset the framebuffer configuration
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GBuffer->GetTexture(0), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GBuffer->GetTexture(1), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GBuffer->GetTexture(2), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_GBuffer->GetTexture(3), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBuffer->GetDepthBuffer(), 0);

		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		m_GBuffer->Unbind();





		//// Lighting Pass

		// Generate mipmap for shadow map otherwis there will be artifact
		glBindTexture(GL_TEXTURE_2D, m_ShadowMap->GetDepthBuffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(0));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(1));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(2));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(3));
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_ShadowMap->GetDepthBuffer());
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetDepthBuffer()); // HiZ map


		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_RSMGbuffer->GetTexture(0));
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, m_RSMGbuffer->GetTexture(1));
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, m_RSMGbuffer->GetTexture(2));
		//glActiveTexture(GL_TEXTURE9);
		//glBindTexture(GL_TEXTURE_2D, m_RSMGbuffer->GetDepthBuffer());

		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_CUBE_MAP, specularEnvmap);
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, m_BRDFlut);
		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(4));
		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_CSMtex);

		
		//glBindFramebuffer(GL_FRAMEBUFFER,0);
		m_LightingBuffer->Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			LightingShader.Use();
			LightingShader.SetMatrix4("viewMatrix", m_Camera->GetViewMatrix());
			LightingShader.SetMatrix4("projectionMatrix", m_Camera->GetProjectionMatrix());
			LightingShader.SetInteger("gPosition", 0);
			LightingShader.SetInteger("gNormal", 1);
			LightingShader.SetInteger("gAlbedo", 2);
			LightingShader.SetInteger("gMetallicRoughnessAo", 3);
			LightingShader.SetInteger("shadowMap", 4);
			LightingShader.SetInteger("HizMap", 5);
			LightingShader.SetInteger("maxLevel", 1 + (int)floorf(log2f(fmaxf(SCR_WIDTH, SCR_HEIGHT))) - 1);

		
			LightingShader.SetInteger("rsmPosition", 6);
			LightingShader.SetInteger("rsmNormal", 7);
			LightingShader.SetInteger("rsmFlux", 8);

			LightingShader.SetInteger("irradianceMap", 10);
			LightingShader.SetInteger("specularEnvmap", 11);
			LightingShader.SetInteger("m_BRDFlut", 12);

			LightingShader.SetInteger("gVelocity", 13);
			LightingShader.SetInteger("CSM", 14);
			LightingShader.SetFloat("far", m_Camera->far);

			LightingShader.SetInteger("cascadeCount", m_CSMData->m_shadowCascadeLevels.size());
			LightingShader.BindUBOToBindingPoint("LightSpaceMatrices", 3);
			for (size_t i = 0; i < m_CSMData->m_shadowCascadeLevels.size(); ++i)
			{
				LightingShader.SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]",
					m_CSMData->m_shadowCascadeLevels[i]);
			}


			LightingShader.SetVector3f("viewPos", m_Camera->Position);
			m_ScreenQuad.Draw(LightingShader);
		m_LightingBuffer->Unbind();


		//  TAA Pass
		//auto nowIndex = frameCount % 2;
		//auto historyIndex = nowIndex == 1 ? 0 : 1;
		////std::cout << "history: " << historyIndex << std::endl;
		////std::cout << "now; " << nowIndex << std::endl;
		//m_taaResolve[nowIndex]->Bind();
		////m_taaResolve[0]->Bind();
		//	taaResolveShader.Use();
		//	taaResolveShader.SetFloat("ScreenWidth", float(SCR_WIDTH));
		//	taaResolveShader.SetFloat("ScreenHeight", float(SCR_HEIGHT));
		//	taaResolveShader.SetInteger("frameCount", frameCount); 

		//	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	//glDisable(GL_DEPTH_TEST);
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_LightingBuffer->GetTexture(0)); // Now Texture
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D, m_taaResolve[historyIndex]->GetTexture(0)); // Previous/History Texture
		//	//glBindTexture(GL_TEXTURE_2D, m_taaResolve[0]->GetTexture(0)); // Previous/History Texture
		//	glActiveTexture(GL_TEXTURE2);
		//	glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetTexture(4)); // Velocity Texture
		//	glActiveTexture(GL_TEXTURE3);
		//	glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetDepthBuffer()); // Scene Depth Texture
		//	m_ScreenQuad.Draw(taaResolveShader);

		//	//glEnable(GL_DEPTH_TEST);
		////	m_taaResolve[nowIndex]->Unbind();
		//m_taaResolve[0]->Unbind();



		

		glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.0f, 0.5f, 1.0f);
		glViewport(0, 0, SCR_WIDTH / 1, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_LightingBuffer->GetTexture(0));
		//glBindTexture(GL_TEXTURE_2D, m_taaResolve[nowIndex]->GetTexture(0));
		//glBindTexture(GL_TEXTURE_2D, m_LightingBuffer->GetTexture(0));
		//glBindTexture(GL_TEXTURE_2D, m_taaResolve[0]->GetTexture(0));
		finalShader.Use();
		finalShader.SetInteger("finalColor", 0);
		m_ScreenQuad.Draw(finalShader);


		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		////glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_ShadowMap->GetDepthBuffer());
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D_ARRAY, m_CSMtex);

		//ShadowDebugShader.Use();

		//ShadowDebugShader.SetInteger("layer", debugLayer);
		////std::cout << "layer: "<<debugLayer << std::endl;
		//ShadowDebugShader.SetFloat("near_plane", SHADOW_NEAR);
		//ShadowDebugShader.SetFloat("far_plane", SHADOW_FAR);
		//ShadowDebugShader.SetInteger("shadowMap", 0);
		//ShadowDebugShader.SetInteger("csm", 1);
		//m_ScreenQuad.Draw(ShadowDebugShader);

	/*	glViewport(SCR_WIDTH / 2  + 50, 0, SCR_WIDTH / 2, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_LightingBuffer->GetTexture(0));
		finalShader.Use();
		finalShader.SetInteger("finalColor", 0);
		m_ScreenQuad.Draw(finalShader);*/


		


		// copy content of geometry's depth buffer to default framebuffer's depth buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->GetFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//// Forward Pass
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glActiveTexture(GL_TEXTURE4);
		//glBindTexture(GL_TEXTURE_2D, m_ShadowMap->GetDepthBuffer());
		//	//Draw a plane
		//BlinnPhongShader.Use();
		//BlinnPhongShader.SetMatrix4("projection", m_Camera->GetProjectionMatrix());
		//BlinnPhongShader.SetMatrix4("view", m_Camera->GetViewMatrix());
		//BlinnPhongShader.SetVector3f("viewPos", m_Camera->Position);
		//BlinnPhongShader.SetInteger("blinn", 1);
		//BlinnPhongShader.SetInteger("shadowMap", 4);
		//m_Plane.Draw(BlinnPhongShader);

		// skybox pass
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		skyboxShader.Use();
		skyboxShader.SetInteger("environmentMap", 0);
		skyboxShader.SetMatrix4("view", m_Camera->GetViewMatrix());
		skyboxShader.SetMatrix4("projection", m_Camera->GetProjectionMatrix());
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmapCaptureFBO->m_cubeTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, specularEnvmap);
		//m_Cube->Draw(skyboxShader);

		++frameCount;
		m_TaaData->prevView = m_Camera->GetViewMatrix();
		m_TaaData->prevProj = m_Camera->GetProjectionMatrix();
	}
	
}

