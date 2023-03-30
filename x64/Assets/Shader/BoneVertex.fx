#version 460

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;
uniform mat4 BoneMatrices[100];
uniform mat4 PrevBoneMatrices[100];
uniform bool isBlending;
uniform float fBlendFactor;

in layout(location = 0) vec4 vColour;
in layout(location = 1) vec4 vPos;
in layout(location = 2) vec4 vNormal;
in layout(location = 3) vec4 vTexUV;
in layout(location = 4) vec4 vTangent;
in layout(location = 5) vec4 vBinormal;
in layout(location = 6) vec4 vBoneID;
in layout(location = 7) vec4 vBoneWeight;

out vec4 fColour;
out vec4 fNormal;
out vec4 fVtxWorldPos;
out vec4 fTexUV;
out vec4 fTangent;
out vec4 fBinormal;
out mat4 fInvMat;
out float debugColor;

void main()
{
	mat4 matBone = BoneMatrices[int(vBoneID.x)] * vBoneWeight.x;
	matBone += BoneMatrices[int(vBoneID.y)] * vBoneWeight.y;
	matBone += BoneMatrices[int(vBoneID.z)] * vBoneWeight.z;
	matBone += BoneMatrices[int(vBoneID.w)] * vBoneWeight.w;

	mat4 matFinal = matBone;

	if (isBlending)
	{
		mat4 matPrev = PrevBoneMatrices[int(vBoneID.x)] * vBoneWeight.x;
		matPrev += PrevBoneMatrices[int(vBoneID.y)] * vBoneWeight.y;
		matPrev += PrevBoneMatrices[int(vBoneID.z)] * vBoneWeight.z;
		matPrev += PrevBoneMatrices[int(vBoneID.w)] * vBoneWeight.w;

		matFinal = matBone * fBlendFactor;
		matFinal += matPrev * (1.f - fBlendFactor);
	}

	debugColor = vBoneID.x;
	vec4 position = matFinal * vPos;
	gl_Position = matProj * matView * matWorld * position;
		
	fVtxWorldPos = vec4((matWorld * position).xyz, 1.0f);
	fNormal = vec4(normalize(transpose(inverse(matWorld)) * vNormal).xyz, 1.0f);

	fInvMat = mat4(vec4(normalize(vTangent).xyz, 0.f), vec4(normalize(vBinormal).xyz, 0.f), vec4(normalize(vNormal).xyz, 0.f), vec4(0.f, 0.f, 0.f, 1.f));
	fInvMat = transpose(fInvMat);

	fColour = vColour;
	fTexUV = vTexUV;
	fTangent = vTangent;
	fBinormal = vBinormal;
}