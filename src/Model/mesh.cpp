/* 
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-10
*/

//Includes
#include "mesh.h"
#include <string>

//The diffuse texture is assumed to always exist and always loaded in case you want to do alpha
//discard. Lower overhead texture setup is something worth investigating here.
void Mesh::draw(Lumen::Shader &shader, bool textured)
{
        //Diffuse
        glActiveTexture(GL_TEXTURE0);
        shader.SetInteger("albedoMap", 0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[0]);
    if(textured){
            //Emissive
            glActiveTexture(GL_TEXTURE1);
            shader.SetInteger("emissiveMap", 1);
            glBindTexture(GL_TEXTURE_2D, m_Textures[1]);

            //Normals
            if (m_Textures[2] == 0){
                shader.SetBool("normalMapped", false);
            }
            else{
                shader.SetBool("normalMapped", true);
            }
            glActiveTexture(GL_TEXTURE2);
            shader.SetInteger("normalsMap", 2);
            glBindTexture(GL_TEXTURE_2D, m_Textures[2]);

            //Ambient Oclussion
            if (m_Textures[3] == 0){
                shader.SetBool("aoMapped", false);
            }
            else{
                shader.SetBool("aoMapped", true);
            }
            glActiveTexture(GL_TEXTURE3);
            shader.SetInteger("lightMap", 3);
            glBindTexture(GL_TEXTURE_2D, m_Textures[3]);

            //Metal / Roughness
            glActiveTexture(GL_TEXTURE4);
            shader.SetInteger("metalRoughMap", 4);
            glBindTexture(GL_TEXTURE_2D, m_Textures[4]);

    }


    m_VertexArray.Bind();
    glDrawElements(GL_TRIANGLES, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, NULL);
    m_VertexArray.Unbind();

}

void Mesh::drawShadow(Lumen::Shader& shader)
{
    m_VertexArray.Bind();
    glDrawElements(GL_TRIANGLES, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, NULL);
    m_VertexArray.Unbind();
}

void Mesh::DrawRSMGbuffer(Lumen::Shader& shader)
{
    glActiveTexture(GL_TEXTURE0);
    shader.SetInteger("albedoMap", 0);
    glBindTexture(GL_TEXTURE_2D, m_Textures[0]);

    glActiveTexture(GL_TEXTURE1);
    shader.SetInteger("normalsMap", 1);
    glBindTexture(GL_TEXTURE_2D, m_Textures[2]);

    m_VertexArray.Bind();
    glDrawElements(GL_TRIANGLES, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, NULL);
    m_VertexArray.Unbind();
}

//Sending the data to the GPU and formatting it in memory
void Mesh::setupMesh()
{
    m_VertexArray.Bind();
    m_VertexBuffer.Bind();
    m_IndexBuffer.Bind();

    m_VertexBuffer.VertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(Vertex), (void*)0);
    m_VertexBuffer.VertexAttribPointer(1, 3, GL_FLOAT, 0, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
    m_VertexBuffer.VertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
    m_VertexBuffer.VertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    m_VertexBuffer.VertexAttribPointer(4, 3, GL_FLOAT, 0, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));

    m_VertexBuffer.BufferData(m_Vertices.size() * sizeof(Vertex), &m_Vertices.front(), GL_STATIC_DRAW);
    m_IndexBuffer.BufferData(m_Indices.size() * sizeof(GLuint), &m_Indices.front(), GL_STATIC_DRAW);

    m_VertexArray.Unbind();
    m_VertexBuffer.Unbind();
    m_IndexBuffer.Unbind();

    ////Generate Buffers    
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    ////Bind Vertex Array Object and VBO in correct order
    //glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    ////VBO stuff 
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    ////EBO stuff
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    ////Vertex position pointer init
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    ////Vertex normal pointer init
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    ////Vertex texture coord
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    ////Tangent position
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    ////Bittangent position
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));

    ////Unbinding VAO
    //glBindVertexArray(0);
}

