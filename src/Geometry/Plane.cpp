#include "Plane.h"

Stella::Plane::Plane()
{
    float planeVertices[] =
    {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    PlaneVAO.Bind();
    PlaneVBO.Bind();
    PlaneVBO.BufferData(sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    PlaneVBO.VertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)0);
    PlaneVBO.VertexAttribPointer(1, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    PlaneVBO.VertexAttribPointer(2, 2, GL_FLOAT, 0, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    PlaneVAO.Unbind();

    m_Model = glm::translate(m_Model, glm::vec3(2.0, 0.0, 5.0));
    m_Model = glm::scale(m_Model, glm::vec3(150.0f));
}

Stella::Plane::~Plane()
{
}

void Stella::Plane::Draw(Lumen::Shader& shader, std::shared_ptr<Camera> camera)
{

    PlaneVAO.Bind();
    shader.Use();
    shader.SetVector3f("albedo", albedo);
    shader.SetFloat("metallic", metallic);
    shader.SetFloat("roughness", roughness);
    shader.SetFloat("ao", ao);

    shader.SetMatrix4("model", m_Model);
    shader.SetMatrix4("prevModel", m_Model);
    shader.SetMatrix4("view", camera->GetViewMatrix());
    shader.SetVector3f("camPos", camera->Position);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    PlaneVAO.Unbind();

}

void Stella::Plane::DrawGbuffer(Lumen::Shader& shader)
{
}

void Stella::Plane::DrawShadow(Lumen::Shader& shader)
{
    PlaneVAO.Bind();
    shader.Use();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    PlaneVAO.Unbind();

}
