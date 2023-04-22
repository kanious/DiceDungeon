#include "pch.h"
#include "../Headers/TargetTexture.h"
#include "../Headers/OpenGLDefines.h"
#include "../Headers/Component.h"
#include "../Headers/ComponentMaster.h"
#include "../Headers/Shader.h"

USING(Engine)
USING(std)
USING(glm)

CTargetTexture::CTargetTexture()
    : m_iTextureID(0), m_iVAO_ID(0), m_iVB_ID(0)
    , m_pShader(nullptr)
{
}

CTargetTexture::CTargetTexture(const CTargetTexture& rhs)
    : m_iTextureID(rhs.m_iTextureID), m_iVAO_ID(rhs.m_iVAO_ID), m_iVB_ID(rhs.m_iVB_ID)
    , m_pShader(rhs.m_pShader)
{
    m_tag = rhs.m_tag;
}

CTargetTexture::~CTargetTexture()
{
}

// Call instead of destructor to manage class internal data
void CTargetTexture::Destroy()
{
    glDeleteVertexArrays(1, &m_iVAO_ID);
    glDeleteBuffers(1, &m_iVB_ID);
    SafeDestroy(m_pShader);

    CComponent::Destroy();
}

void CTargetTexture::Render()
{
    m_pShader->Use();
    m_pShader->SetMat4x4("matWorld", m_matWorld);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Load texture information from file
RESULT CTargetTexture::Ready(string ID, _uint texID, vec3 vPos)
{
    m_tag = ID;
    m_iTextureID = texID;

    m_matWorld = mat4x4(1.f);
    m_matWorld = translate(m_matWorld, vPos);
    m_matWorld = scale(m_matWorld, vec3(0.2f));

    SetVIBuffer();
    SetShader();

    return PK_NOERROR;
}

void CTargetTexture::SetVIBuffer()
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
}

void CTargetTexture::SetShader()
{
    CComponent* pComponent = CloneComponent<CShader*>("ScreenShader");
    if (nullptr != pComponent)
    {
        m_pShader = dynamic_cast<CShader*>(pComponent);
        m_pShader->Use();
        m_pShader->SetInt("screenTexture", 0);
    }
}

// Clone component
CComponent* CTargetTexture::Clone()
{
    //return new CTargetTexture(*this);
    return this;
}

// Create an instance
CTargetTexture* CTargetTexture::Create(string ID, _uint texID, vec3 vPos)
{
    CTargetTexture* pInstance = new CTargetTexture();
    if (PK_NOERROR != pInstance->Ready(ID, texID, vPos))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
