#pragma once

#include "Define.h"
#include "OpenGLDefines.h"

class Cube
{
private:
	struct Vertex
	{
		vec3 vPos;
		vec3 vColor;
	};

private:
	Vertex* m_pVertices;
	GLushort* m_pIndices;

	GLuint numVertex;
	GLuint numIndex;

	GLuint m_vB;
	GLuint m_iB;

	vec3 m_vPos;
	vec3 m_vColor;
	mat4x4 m_matTransform;

private:
	Cube();
	void Init(vec3, vec3, bool);

public:
	~Cube();
	
public:
	static Cube* Create(vec3, vec3, bool);
	void Update(_float&, GLuint, mat4x4&, mat4x4&);
	void Render();
	void Update();
	void TestTest();
};