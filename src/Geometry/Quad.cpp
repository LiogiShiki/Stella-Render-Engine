#include "Quad.h"

namespace Stella
{

    Quad::Quad()
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        QuadVAO.Bind();
        QuadVBO.Bind();
        QuadVBO.BufferData(sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        QuadVBO.VertexAttribPointer(0, 3, GL_FLOAT, 0, 5 * sizeof(float), (void*)0);
        QuadVBO.VertexAttribPointer(1, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        QuadVAO.Unbind();
    }

    Quad::~Quad()
    {
    }

    void Quad::Draw(Lumen::Shader& shader)
    {
        QuadVAO.Bind();
        shader.Use();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        QuadVAO.Unbind();

    }

}
