#ifndef _ENGINESTRUCT_H_
#define _ENGINESTRUCT_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Engine
{
	typedef struct sVertex_XYZ_N_RGBA_UV
	{
		glm::vec3 vPos;
		glm::vec3 vNormal;
		glm::vec2 vTexUV;
	}VTX;

	typedef struct sIndex
	{
		unsigned int _0;
		unsigned int _1;
		unsigned int _2;
	}IDX;

	enum ModelType
	{
		xyz_index,
		xyz_normal_index,
		xyz_normal_texUV_index,
		xyz_normal_texUV_index_texNum
	};
}

#endif //_ENGINESTRUCT_H_