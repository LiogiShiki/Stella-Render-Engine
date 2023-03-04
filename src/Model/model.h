#ifndef MODEL_H
#define MODEL_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-09-08
PURPOSE      : Container for the different meshes in a scene. Also serves as the loading point for mesh files 
               using assimp as the loading library. It also keeps track of and objects position and loads the 
               associated textures for each mesh that it contains.
SPECIAL NOTES: Because it uses assimp to load meshes it is naturally shaped by it in the way it loads and handles 
               data. Currently, all meshes load textures in the exact same way and therefore are limited to effectively
               having just one material type. This is a high priority change, probably the next version after the alpha.
*/

//Includes
#include "Graphics/shader.h"
#include "HBE_texture.h"
#include "mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct TransformParameters
{
    TransformParameters(): scaling(glm::vec3(1.0f)) {};
    glm::vec3 translation;
    float angle;
    glm::vec3 rotationAxis;
    glm::vec3 scaling;
};

struct Model 
{
    Model(const std::string meshPath, const TransformParameters initParameters, bool IBL) : IBL(IBL)
    {
        loadModel(meshPath);
        m_Model = glm::translate(m_Model, m_Position);
        m_Model = glm::scale(m_Model, m_Scale);
    }

    void loadModel(std::string path);
    void update(const unsigned int deltaT);
    void Draw(Lumen::Shader& shader, std::shared_ptr<Camera> p_Camera,const bool textured);
    void drawShadow( Lumen::Shader &shader);

    void DrawRSMGbuffer(Lumen::Shader& shader);

    //Model processing/loading functions
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<unsigned int> processTextures(const aiMaterial *material);

    bool IBL;
    std::vector<Mesh> meshes; //Does it need to be a vector after initialization?

    //To avoid textures being loaded from disk more than once they are indexed into a dictionary
    std::unordered_map<std::string, HRE::Texture> textureAtlas;
    std::string directory, fileExtension;
    bool is_glft;

    // Transform
    glm::vec3 m_Position = glm::vec3(0.0f,0.3f,-0.1f);
    glm::vec3 m_Scale = glm::vec3(10.0f);
    glm::mat4 m_Model = glm::mat4(1.0f);
};

#endif 