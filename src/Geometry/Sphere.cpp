#include "Sphere.h"
#include"Renderer/shader.h"
#include<memory>


Stella::Sphere::Sphere(const std::string& path):TexturePath(path)
{

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }


   SphereVAO.Bind();
   SphereVBO.Bind();
   SphereEBO.Bind();
   SphereVBO.BufferData(data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
   SphereEBO.BufferData(indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

   unsigned int stride = (3 + 2 + 3) * sizeof(float);
   SphereVBO.VertexAttribPointer(0, 3, GL_FLOAT, 0, stride, (void*)0);
   SphereVBO.VertexAttribPointer(1, 3, GL_FLOAT, 0, stride, (void*)(3 * sizeof(float)));
   SphereVBO.VertexAttribPointer(2, 2, GL_FLOAT, 0, stride, (void*)(6 * sizeof(float)));
   SphereVAO.Unbind();

   albedoMap = std::make_unique<Lumen::Texture>();
   aoMap = std::make_unique<Lumen::Texture>();
   normalMap = std::make_unique<Lumen::Texture>();
   metallicRoughnessMap = std::make_unique<Lumen::Texture>();

   albedoMap->CreateTexture( "src/assets/textures/" + path + "/albedo.png",false,false);
   aoMap->CreateTexture("src/assets/textures/" + path + "/ao.png", false, true);
   normalMap->CreateTexture("src/assets/textures/" + path + "/normal.png", false, true);
   metallicRoughnessMap->CreateTexture("src/assets/textures/" + path + "/metallicRoughness.png", false, true);

   m_Model = glm::translate(m_Model, glm::vec3(2.0, 2.0, 2.0));
   m_Model = glm::scale(m_Model, glm::vec3(5.0f));

}

Stella::Sphere::~Sphere()
{
    
}

void Stella::Sphere::Draw(Lumen::Shader& shader, std::shared_ptr<Camera> camera)
{

    albedoMap->Bind(0);
    normalMap->Bind(2);
    aoMap->Bind(3);
    metallicRoughnessMap->Bind(4);

    SphereVAO.Bind();

    shader.Use();
    shader.SetInteger("albedoMap", 0);
    shader.SetInteger("emissiveMap", 1);
    shader.SetInteger("normalsMap", 2);
    shader.SetInteger("lightMap", 3);
    shader.SetInteger("metalRoughMap", 4);

    shader.SetMatrix4("model", m_Model);
    shader.SetMatrix4("prevModel", m_Model);
    shader.SetVector3f("camPos", camera->Position);

    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    SphereVAO.Unbind();

    albedoMap->Unbind();
    normalMap->Unbind();
    metallicRoughnessMap->Unbind();
    aoMap->Unbind();
}

void Stella::Sphere::DrawNonTex(Lumen::Shader& shader, std::shared_ptr<Camera> camera)
{

    SphereVAO.Bind();
    shader.Use();
    shader.SetVector3f("albedo", albedo);
    shader.SetFloat("metallic", metallic);
    shader.SetFloat("roughness", roughness);
    shader.SetFloat("ao", ao);

    shader.SetMatrix4("model", m_Model);
    shader.SetMatrix4("prevModel", m_Model);
    shader.SetVector3f("camPos", camera->Position);


    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    SphereVAO.Unbind();
}

void Stella::Sphere::DrawShadow(Lumen::Shader& shader)
{
    SphereVAO.Bind();
    shader.Use();
    shader.SetMatrix4("model", m_Model);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    SphereVAO.Unbind();
}

void Stella::Sphere::DrawShadow(std::unique_ptr<Shader>& shader)
{
    SphereVAO.Bind();
    shader->use();
    shader->setMat4("model", m_Model);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    SphereVAO.Unbind();
}


