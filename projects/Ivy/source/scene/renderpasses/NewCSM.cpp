#include "ivypch.h"
#include "NewCSM.h"

void Ivy::NewCSM::RenderToShadowmaps(VecI2 windowSize, Vector<Ptr<Entity>>& entities)
{
    mShader->Bind();
    // 0. UBO setup
    const auto lightMatrices = GetLightSpaceMatrices(windowSize);
    glBindBuffer(GL_UNIFORM_BUFFER, mMatricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        mShader->SetUniformMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightMatrices[i]);

        //glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }

    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    // render scene from light's point of view

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthMaps, 0);
    glViewport(0, 0, mDepthMapResolution, mDepthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // peter panning

    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i]->IsActive())
        {
            Ptr<Transform> trans = entities[i]->GetComponent<Transform>();
            Mat4 model = trans->getComposed();
            mShader->SetUniformMat4("model", model);

            Vector<Ptr<Mesh>> meshes = entities[i]->GetComponents<Mesh>();
            for (int j = 0; j < meshes.size(); j++)
            {
                meshes[j]->Draw(mShader, false);
            }
        }
    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Ivy::NewCSM::CreateFramebuffer()
{
    glGenFramebuffers(1, &mFBO);

    glGenTextures(1, &mDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mDepthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, mDepthMapResolution, mDepthMapResolution, int(mShadowCascadeLevels.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Debug::Error("CSM Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure UBO
    // --------------------
    glGenBuffers(1, &mMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, mMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

Ivy::Vector<Ivy::Vec4> Ivy::NewCSM::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
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

Ivy::Mat4 Ivy::NewCSM::GetLightSpaceMatrix(VecI2 windowSize, const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(mCamera->GetFOV()), (float)windowSize.x / (float)windowSize.y, nearPlane,
        farPlane);

    const auto corners = GetFrustumCornersWorldSpace(proj, mCamera->GetViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightDir = mDirLight.direction;

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
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

    // Tune this parameter according to the scene
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

    const glm::mat4 lpMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, minZ, maxZ);

    const float scaleX = 2.0f / (maxX - minX);
    const float scaleY = 2.0f / (maxY - minY);
    const float offsetX = -0.5f * (minX + maxX) * scaleX;
    const float offsetY = -0.5f * (minY + maxY) * scaleY;

    glm::mat4 cropMatrix(1.0f);
    cropMatrix[0][0] = scaleX;
    cropMatrix[1][1] = scaleY;
    cropMatrix[3][0] = offsetX;
    cropMatrix[3][1] = offsetY;

    return cropMatrix * lpMatrix * lightView;
}

Ivy::Vector<Ivy::Mat4> Ivy::NewCSM::GetLightSpaceMatrices(VecI2 windowSize)
{
    Vector<Mat4> ret;
    for (size_t i = 0; i < mShadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(GetLightSpaceMatrix(windowSize, mCamera->GetNearPlane(), mShadowCascadeLevels[i]));
        }
        else if (i < mShadowCascadeLevels.size())
        {
            ret.push_back(GetLightSpaceMatrix(windowSize, mShadowCascadeLevels[i - 1], mShadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(GetLightSpaceMatrix(windowSize, mShadowCascadeLevels[i - 1], mCamera->GetFarPlane()));
        }
    }
    return ret;
}



