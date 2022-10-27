#include "Cube.h"

Cube::Cube()
	: m_pVertices(nullptr), m_pIndices(nullptr), numVertex(0), numIndex(0)
    , m_vPos(0), m_vColor(0), m_matTransform(1.0f)

{

}

Cube::~Cube()
{
	delete[] m_pVertices;
	delete[] m_pIndices;
	numVertex = numIndex = 0;


}

void Cube::Init(vec3 vPos, vec3 vColor, bool player)
{
	m_vPos = vPos;
	m_vColor = vColor;

	if (player)
	{
		Vertex stackVerts[] = {
			vec3(-1.0f, +1.0f, +1.0f), // 0
			vColor, // color
			vec3(+1.0f, +1.0f, +1.0f), // 1
			vColor, // color
			vec3(+1.0f, +1.0f, -1.0f), // 2
            vColor, // color
            vec3(-1.0f, +1.0f, -1.0f), // 3
            vColor, // color
            
            vec3(-1.0f, +1.0f, -1.0f), // 4
            vColor, // color
            vec3(+1.0f, +1.0f, -1.0f), // 5
            vColor, // color
            vec3(+1.0f, -1.0f, -1.0f), // 6
            vColor, // color
            vec3(-1.0f, -1.0f, -1.0f), // 7
            vColor, // color
            
            vec3(+1.0f, +1.0f, -1.0f), // 8
            vColor, // color
            vec3(+1.0f, +1.0f, +1.0f), // 9
            vColor, // color
            vec3(+1.0f, -1.0f, +1.0f), // 10
            vColor, // color
            vec3(+1.0f, -1.0f, -1.0f), // 11
            vColor, // color
            
            vec3(-1.0f, +1.0f, +1.0f), // 12
            vColor, // color
            vec3(-1.0f, +1.0f, -1.0f), // 13
            vColor, // color
            vec3(-1.0f, -1.0f, -1.0f), // 14
            vColor, // color
            vec3(-1.0f, -1.0f, +1.0f), // 15
            vColor, // color
            
            vec3(+1.0f, +1.0f, +1.0f), // 16
            vColor, // color
            vec3(-1.0f, +1.0f, +1.0f), // 17
            vColor, // color
            vec3(-1.0f, -1.0f, +1.0f), // 18
            vColor, // color
            vec3(+1.0f, -1.0f, +1.0f), // 19
            vColor, // color
            
            vec3(+1.0f, -1.0f, -1.0f), // 20
            vColor, // color
            vec3(-1.0f, -1.0f, -1.0f), // 21
            vColor, // color
            vec3(-1.0f, -1.0f, +1.0f), // 22
            vColor, // color
            vec3(+1.0f, -1.0f, +1.0f), // 23
            vColor, // color
		};
		numVertex = sizeof(stackVerts) / sizeof(*stackVerts);
		m_pVertices = new Vertex[numVertex];
		memcpy(m_pVertices, stackVerts, sizeof(stackVerts));
	}
	else
    {
        Vertex stackVerts[] = {
            vec3(-0.5f, +0.5f, +0.5f), // 0
            vColor, // color
            vec3(+0.5f, +0.5f, +0.5f), // 1
            vColor, // color
            vec3(+0.5f, +0.5f, -0.5f), // 2
            vColor, // color
            vec3(-0.5f, +0.5f, -0.5f), // 3
            vColor, // color

            vec3(-0.5f, +0.5f, -0.5f), // 4
            vColor, // color
            vec3(+0.5f, +0.5f, -0.5f), // 5
            vColor, // color
            vec3(+0.5f, -0.5f, -0.5f), // 6
            vColor, // color
            vec3(-0.5f, -0.5f, -0.5f), // 7
            vColor, // color

            vec3(+0.5f, +0.5f, -0.5f), // 8
            vColor, // color
            vec3(+0.5f, +0.5f, +0.5f), // 9
            vColor, // color
            vec3(+0.5f, -0.5f, +0.5f), // 10
            vColor, // color
            vec3(+0.5f, -0.5f, -0.5f), // 11
            vColor, // color

            vec3(-0.5f, +0.5f, +0.5f), // 12
            vColor, // color
            vec3(-0.5f, +0.5f, -0.5f), // 13
            vColor, // color
            vec3(-0.5f, -0.5f, -0.5f), // 14
            vColor, // color
            vec3(-0.5f, -0.5f, +0.5f), // 15
            vColor, // color

            vec3(+0.5f, +0.5f, +0.5f), // 16
            vColor, // color
            vec3(-0.5f, +0.5f, +0.5f), // 17
            vColor, // color
            vec3(-0.5f, -0.5f, +0.5f), // 18
            vColor, // color
            vec3(+0.5f, -0.5f, +0.5f), // 19
            vColor, // color

            vec3(+0.5f, -0.5f, -0.5f), // 20
            vColor, // color
            vec3(-0.5f, -0.5f, -0.5f), // 21
            vColor, // color
            vec3(-0.5f, -0.5f, +0.5f), // 22
            vColor, // color
            vec3(+0.5f, -0.5f, +0.5f), // 23
            vColor, // color
        };
        numVertex = sizeof(stackVerts) / sizeof(*stackVerts);
        m_pVertices = new Vertex[numVertex];
        memcpy(m_pVertices, stackVerts, sizeof(stackVerts));
    }

    GLushort stackIndices[] = {
        0,   1,  2,  0,  2,  3, // Top
        4,   5,  6,  4,  6,  7, // Front
        8,   9, 10,  8, 10, 11, // Right
        12, 13, 14, 12, 14, 15, // Left
        16, 17, 18, 16, 18, 19, // Back
        20, 22, 21, 20, 23, 22, // Bottom
    };
    numIndex = sizeof(stackIndices) / sizeof(*stackIndices);
    m_pIndices = new GLushort[numIndex];
    memcpy(m_pIndices, stackIndices, sizeof(stackIndices));

    //GLuint vB;
    glGenBuffers(1, &m_vB);
    glBindBuffer(GL_ARRAY_BUFFER, m_vB);
    glBufferData(GL_ARRAY_BUFFER, numVertex * sizeof(Vertex), m_pVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));

    //GLuint iB;
    glGenBuffers(1, &m_iB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndex * sizeof(GLushort), m_pIndices, GL_STATIC_DRAW);

    //m_matTransform = scale(m_matTransform, vec3(1.f));
    //m_matTransform = rotate(m_matTransform, 0.f, vec3(0.f, 0.f, 0.f));
    m_matTransform = translate(m_matTransform, vPos);
}
GLuint shaderProgram;
Cube* Cube::Create(vec3 vPos, vec3 vColor, bool player)
{
	Cube* pInstance = new Cube();
	pInstance->Init(vPos, vColor, player);
    
	return pInstance;
}
#include "ConfigurationManager.h"
void Cube::Update(_float& dt, GLuint matLocation, mat4x4& p, mat4x4& v)
{
    // matTransform은 매번 항등행렬로 초기화하고
    // vPos, vScale, vRot 에 정보를 저장해서 합치기
    // 큐브 두개 프린트하는법...
    
    //m_matTransform = scale(m_matTransform, vec3(sin(glfwGetTime() * 10.f) * 0.0005f + 1.f));
    //m_matTransform = rotate(m_matTransform, radians(45.f) * (float)dt, vec3(0.f, 0.f, 1.f));
    m_matTransform = translate(m_matTransform, vec3(m_vPos.x * dt, m_vPos.y * dt, m_vPos.z * dt));

    const mat4x4 matView = ConfigurationManager::GetInstance()->GetViewMatrix();
    const mat4x4 matProj = ConfigurationManager::GetInstance()->GetProjMatrix();

    //mat4x4 mvp = p * v * m_matTransform;
    mat4x4 mvp = matProj * matView * m_matTransform;
    glUniformMatrix4fv(matLocation, 1, GL_FALSE, value_ptr(mvp));
    //glUniform3f(colorL, m_vColor.x, m_vColor.y, m_vColor.z);
    
}

void Cube::Render()
{
    //// 버텍스/인덱스 정보를 렌더시마다 업데이트시켜줘야 함
    glBindBuffer(GL_ARRAY_BUFFER, m_vB);
    glBufferData(GL_ARRAY_BUFFER, numVertex * sizeof(Vertex), m_pVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndex * sizeof(GLushort), m_pIndices, GL_STATIC_DRAW);

    //glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glBindBuffer(GL_ARRAY_BUFFER, m_vB);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iB);

    glDrawElements(GL_TRIANGLES, numIndex, GL_UNSIGNED_SHORT, 0);
}
