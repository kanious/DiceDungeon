#include "pch.h"
#include <sstream>
#include <string>
#include "../Headers/DeferredFBO.h"
#include "../Headers/OpenGLDefines.h"
#include "../Headers/GameMaster.h"
#include "../Headers/Component.h"
#include "../Headers/ComponentMaster.h"
#include "../Headers/Shader.h"
#include "../Headers/TargetTexture.h"
#include "../Headers/LightMaster.h"

USING(Engine)
USING(std)
USING(glm)

CDeferredFBO::CDeferredFBO()
    : m_iFrameBufferID(0), m_iDepthID(0)
    , m_iDiffuseTexture(0), m_iNormalTexture(0), m_iPositionTexture(0)
    , m_iWidth(0), m_iHeight(0), m_iVAO_ID(0), m_iVB_ID(0)
    , m_pShader(nullptr)
    , m_pDiffuseTarget(nullptr), m_pPosTarget(nullptr), m_pNormalTarget(nullptr)
{
}

CDeferredFBO::CDeferredFBO(const CDeferredFBO& rhs)
    : m_iFrameBufferID(rhs.m_iFrameBufferID)
    , m_iDepthID(rhs.m_iDepthID)
    , m_iDiffuseTexture(rhs.m_iDiffuseTexture)
    , m_iNormalTexture(rhs.m_iNormalTexture)
    , m_iPositionTexture(rhs.m_iPositionTexture)
    , m_iWidth(rhs.m_iWidth), m_iHeight(rhs.m_iHeight)
    , m_iVAO_ID(rhs.m_iVAO_ID), m_iVB_ID(rhs.m_iVB_ID)
    , m_pShader(rhs.m_pShader)
    , m_pDiffuseTarget(rhs.m_pDiffuseTarget), m_pNormalTarget(rhs.m_pNormalTarget), m_pPosTarget(rhs.m_pPosTarget)
{
    m_tag = rhs.m_tag;
}

CDeferredFBO::~CDeferredFBO()
{
}

// Call instead of destructor to manage class internal data
void CDeferredFBO::Destroy()
{
    glDeleteTextures(1, &m_iDiffuseTexture);
    glDeleteTextures(1, &m_iPositionTexture);
    glDeleteTextures(1, &m_iNormalTexture);

    glDeleteFramebuffers(1, &m_iFrameBufferID);
    glDeleteRenderbuffers(1, &m_iDepthID);

    glDeleteVertexArrays(1, &m_iVAO_ID);
    glDeleteBuffers(1, &m_iVB_ID);
    SafeDestroy(m_pShader);

    SafeDestroy(m_pDiffuseTarget);
    SafeDestroy(m_pNormalTarget);
    SafeDestroy(m_pPosTarget);

    CComponent::Destroy();
}

// Init Frame Buffer information
RESULT CDeferredFBO::Ready(string ID, _int width, _int height)
{
    m_tag = ID;

    m_iWidth = width;
    m_iHeight = height;

    // Set screen quad VAO
    if (PK_NOERROR != SetVIBuffer())
        return PK_ERROR;

    // Set Screen Shader
    if (PK_NOERROR != SetShader())
        return PK_ERROR;

    // Set FrameBuffer
    if (PK_NOERROR != SetFrameBuffer())
        return PK_ERROR;

    return PK_NOERROR;
}

RESULT CDeferredFBO::SetVIBuffer()
{
    _float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_iVAO_ID);
    glBindVertexArray(m_iVAO_ID);

    glGenBuffers(1, &m_iVB_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_iVB_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return PK_NOERROR;
}

RESULT CDeferredFBO::SetShader()
{
    string dataPath = CGameMaster::GetInstance()->GetDataPath();
    stringstream ss, ss2;
    ss << dataPath << "Shader\\DeferredVertex.fx";
    ss2 << dataPath << "Shader\\DeferredFragment.fx";
    CComponent* pComponent = CShader::Create("DeferredShader", ss.str().c_str(), ss2.str().c_str());
    if (nullptr == pComponent)
        return PK_ERROR;

    CComponentMaster::GetInstance()->AddNewComponent("DeferredShader", pComponent);
    m_pShader = dynamic_cast<CShader*>(pComponent);

	m_pShader->Use();
	m_pShader->SetInt("diffuseTexture", 0);
	m_pShader->SetInt("normalTexture", 1);
	m_pShader->SetInt("positionTexture", 2);

    return PK_NOERROR;
}

RESULT CDeferredFBO::SetFrameBuffer()
{
    // Generate frame buffer
    glGenFramebuffers(1, &m_iFrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID);

    // Color + specular color buffer
    glGenTextures(1, &m_iDiffuseTexture);
    glBindTexture(GL_TEXTURE_2D, m_iDiffuseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iDiffuseTexture, 0);

    // Normal color buffer
    glGenTextures(1, &m_iNormalTexture);
    glBindTexture(GL_TEXTURE_2D, m_iNormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_iWidth, m_iHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_iNormalTexture, 0);

    // position color buffer
    glGenTextures(1, &m_iPositionTexture);
    glBindTexture(GL_TEXTURE_2D, m_iPositionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_iWidth, m_iHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_iPositionTexture, 0);

    // tell to openGL which color attachments we will use of this framebuffer for rendering
    _uint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // depth buffer
    glGenRenderbuffers(1, &m_iDepthID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_iDepthID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_iWidth, m_iHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iDepthID);

    // Debug buffer rendering
    m_pDiffuseTarget = CTargetTexture::Create("diffuse", m_iDiffuseTexture, vec3(-0.4f, 0.65f, 0.f));
    m_pNormalTarget = CTargetTexture::Create("normal", m_iNormalTexture, vec3(0.0f, 0.65f, 0.f));
    m_pPosTarget = CTargetTexture::Create("position", m_iPositionTexture, vec3(0.4f, 0.65f, 0.f));

    // Generate IDs
    //glGenRenderbuffers(1, &m_iDiffuseID);
    //glGenRenderbuffers(1, &m_iPositionID);
    //glGenRenderbuffers(1, &m_iNormalID);

    // Diffuse
    //glBindRenderbuffer(GL_RENDERBUFFER, m_iDiffuseID);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_iWidth, m_iHeight);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_iDiffuseID);

    //// Bind Position
    //glBindRenderbuffer(GL_RENDERBUFFER, m_iPositionID);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, m_iWidth, m_iHeight);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, m_iPositionID);

    //// Bind Normal
    //glBindRenderbuffer(GL_RENDERBUFFER, m_iNormalID);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, m_iWidth, m_iHeight);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, m_iNormalID);

    // Check
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE != status)
        printf("Frame buffer is not created.. : error(%d)\n", (int)status);
    else
        printf("Frame buffer is created!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return PK_NOERROR;
}

void CDeferredFBO::RenderStart()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID);
    //glPushAttrib(GL_VIEWPORT_BIT);
    //glViewport(0, 0, m_iWidth, m_iHeight);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, buffers);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void CDeferredFBO::Render()
{
    if (nullptr == m_pShader)
        return;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    m_pShader->Use();
    //CLightMaster::GetInstance()->SetLightInfo();
    mat4x4 matWorld = mat4x4(1.f);
    m_pShader->SetMat4x4("matWorld", matWorld);
    //m_pShader->SetVec4("eyeLocation", 0.f, 80.f, -60.f, 1.f);
    //m_pShader->SetVec4("theLights[0].diffuse", 1.f, 1.f, 1.f, 1.f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iDiffuseTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_iNormalTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_iPositionTexture);

    glBindVertexArray(m_iVAO_ID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void CDeferredFBO::RenderEnd()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CDeferredFBO::Render_Debug()
{
    if (nullptr != m_pDiffuseTarget)
        m_pDiffuseTarget->Render();

    if (nullptr != m_pNormalTarget)
        m_pNormalTarget->Render();

    if (nullptr != m_pPosTarget)
        m_pPosTarget->Render();
}

// Clone component
CComponent* CDeferredFBO::Clone()
{
    return new CDeferredFBO(*this);
}

// Create an instance
CDeferredFBO* CDeferredFBO::Create(string ID, _int width, _int height)
{
    CDeferredFBO* pInstance = new CDeferredFBO();
    if (PK_NOERROR != pInstance->Ready(ID, width, height))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
