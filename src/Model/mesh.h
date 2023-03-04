#ifndef MESH_H
#define MESH_H


#include "glm/glm.hpp"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/IndexBuffer.h"
#include"Renderer/camera.h"


#include <vector>
#include <string>

struct Vertex
{
   glm::vec3 position; 
   glm::vec3 normal; 
   glm::vec3 tangent; 
   glm::vec3 biTangent; 
   glm::vec2 texCoords; 
};

struct Mesh
{

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<unsigned int> m_Textures;

    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices,
         const std::vector<unsigned int> &textures)
    {
        this->m_Vertices = vertices;
        this->m_Indices = indices;
        this->m_Textures = textures;

        setupMesh();
    }

    void setupMesh();
    void draw(Lumen::Shader& shader, bool textured);
    void drawShadow( Lumen::Shader &shader);

    void DrawRSMGbuffer(Lumen::Shader& shader);


    Lumen::VertexBuffer m_VertexBuffer;
    Lumen::VertexArray m_VertexArray;
    Lumen::IndexBuffer m_IndexBuffer;


    //Lumen::Texture m_AlbedoMap;
    //Lumen::Texture m_NormalMap;
    //Lumen::Texture m_MetalnessMap;
    //Lumen::Texture m_RoughnessMap;
    //Lumen::Texture m_AmbientOcclusionMap;
    //Lumen::Texture m_MetalnessRoughnessMap;

};



#endif