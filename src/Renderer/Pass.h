#pragma once
#include "Graphics/shader.h"
#include "Graphics/FrameBuffer.h"
#include"Graphics/Texture.h"
#include"Graphics/DepthBuffer.h"
#include"Graphics/VertexArray.h"
#include"Graphics/VertexBuffer.h"
#include<vector>

class Pass
{
protected:
	Lumen::Framebuffer PassframeBuffer;
	
public:
	Lumen::Shader PassShader;

	virtual void Init();
	void SetShader(Lumen::Shader& shader);
	


};