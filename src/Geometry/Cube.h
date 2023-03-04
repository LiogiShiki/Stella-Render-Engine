#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include"Graphics/VertexArray.h"
#include"Graphics/VertexBuffer.h"
#include"Graphics/Shader.h"

namespace Stella
{
	class Cube
	{
	public:
		Cube();
		~Cube();
		void Draw(Lumen::Shader& shader);
	private:
		Lumen::VertexArray CubeVAO;
		Lumen::VertexBuffer CubeVBO;

	};

}



