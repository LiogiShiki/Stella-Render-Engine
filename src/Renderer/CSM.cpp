#include"CSM.h"
#include<iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
namespace Stella
{

    std::vector<glm::vec4> CSMData::CaculateWorldFrustumCorners(const float nearPlane, const float farPlane)
    {
        const auto view = m_Camera->GetViewMatrix();
        const auto proj = glm::perspective(
            glm::radians(m_Camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane,
            farPlane);
        const auto inv = glm::inverse(proj * view);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    glm::mat4 CSMData::getLightSpaceMatrix (const float nearPlane, const float farPlane)
    {

        const auto corners = CaculateWorldFrustumCorners(nearPlane,farPlane);
        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();
        //std::cout<<glm::to_string(center)<<std::endl;
        auto lightView = glm::lookAt(center + 100.0f * m_lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));


        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : corners)
        {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        
        glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);


       // lightProjection = glm::ortho(-120.0f, 120.0f, -120.0f, 120.0f, SHADOW_NEAR, SHADOW_FAR);
       // lightView = glm::lookAt(100.0f * m_lightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        return lightProjection * lightView;

    }

    std::vector<glm::mat4> CSMData::getLightSpaceMatrices()
    {
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < m_shadowCascadeLevels.size() + 1; ++i)
        {
            if (i == 0)
            {
                ret.push_back(getLightSpaceMatrix(m_Camera->near, m_shadowCascadeLevels[i]));
            }
            else if (i < m_shadowCascadeLevels.size())
            {
                ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i]));
            }
            else
            {
                ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_Camera->far));
            }
        }
        return ret;
    }
}