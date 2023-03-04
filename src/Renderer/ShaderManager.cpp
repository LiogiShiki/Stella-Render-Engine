#include "ShaderManager.h"
#include <sstream>

static std::unordered_map<std::string, Lumen::Shader> ShaderManager_ShaderMap;

void Lumen::ShaderManager::CreateShaders()
{
	AddShader("Basic_BlinnPhong", "src/Shader/Basic_BlinnPhong.vs", "src/Shader/Basic_BlinnPhong.fs");
	AddShader("PBR", "src/Shader/pbr.vs", "src/Shader/pbr.fs");
	AddShader("PBRModel", "src/Shader/pbrModel.vs", "src/Shader/pbrModel.fs");
	AddShader("GBufferPass", "src/Shader/GBufferPass.vs", "src/Shader/GBufferPass.fs");
	AddShader("LightingPass", "src/Shader/deferred_lighting.vs", "src/Shader/deferred_lighting.fs");
	AddShader("ShdowMapPass", "src/Shader/ShadowMap.vs", "src/Shader/ShadowMap.fs");

	AddShader("CSMShader", "src/Shader/csm.vs", "src/Shader/csm.fs", "src/Shader/csm.gs");

	AddShader("ShadowDebug", "src/Shader/ShadowDebug.vs", "src/Shader/ShadowDebug.fs");
	AddShader("equirectangularToCube", "src/Shader/equirectangular_to_cubemap.vs", "src/Shader/equirectangular_to_cubemap.fs");
	AddShader("skyboxShader", "src/Shader/Skybox.vs", "src/Shader/Skybox.fs");
	AddShader("envPrefilter", "src/Shader/env_prefilter.vs", "src/Shader/env_prefilter.fs");
	AddShader("envPrefilterSpec", "src/Shader/env_prefilter.vs", "src/Shader/prefilter_spec.fs"); 
	AddShader("GBufferPassNontex", "src/Shader/GbufferPass_nontex.vs", "src/Shader/GBufferPass_nontex.fs");
	AddShader("HizPassShader", "src/Shader/HizPass.vs", "src/Shader/HizPass.fs");
	AddShader("RSMGbufferPassShader", "src/Shader/RSMGbufferPass.vs", "src/Shader/RSMGbufferPass.fs");
	AddShader("brdfShader", "src/Shader/brdf.vs", "src/Shader/brdf.fs");
	AddShader("taaResolveShader", "src/Shader/taaResolve.vs", "src/Shader/taaResolve.fs");
	AddShader("finalShader", "src/Shader/final.vs", "src/Shader/final.fs");

}

void Lumen::ShaderManager::AddShader(const std::string& name, const std::string& vert, const std::string& frag, const std::string& geo)
{
	auto exists = ShaderManager_ShaderMap.find(name);

	if (exists == ShaderManager_ShaderMap.end())
	{
		ShaderManager_ShaderMap.emplace(name, Lumen::Shader());
		ShaderManager_ShaderMap.at(name).CreateShaderProgramFromFile(vert, frag);
		ShaderManager_ShaderMap.at(name).CompileShaders();
	}

	else
	{
		std::string err = "A shader with the name : " + name + "  already exists!";
		throw err;
	}
}

Lumen::Shader& Lumen::ShaderManager::GetShader(const std::string& name)
{
	auto exists = ShaderManager_ShaderMap.find(name);

	if (exists != ShaderManager_ShaderMap.end())
	{
		return ShaderManager_ShaderMap.at(name);
	}

	else
	{
		throw "Shader that doesn't exist trying to be accessed!";
	}
}

GLuint Lumen::ShaderManager::GetShaderID(const std::string& name)
{
	auto exists = ShaderManager_ShaderMap.find(name);

	if (exists != ShaderManager_ShaderMap.end())
	{
		return ShaderManager_ShaderMap.at(name).GetProgramID();
	}

	else
	{
		throw "Shader that doesn't exist trying to be accessed!";
	}
}

void Lumen::ShaderManager::RecompileShaders()
{
	for (auto& e : ShaderManager_ShaderMap)
	{
		e.second.Recompile();
	}
}
