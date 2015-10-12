#ifndef __ASYNC_VBO_TRANSFERS_TYPES_H__
#define __ASYNC_VBO_TRANSFERS_TYPES_H__

#include <glm/glm.hpp>
#include <glm/ext.hpp>

typedef unsigned short ushort;
typedef unsigned int uint32;
typedef unsigned char uint8;

namespace gen{

	struct vert{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	struct vert_packed{
		glm::ivec2 pos;
		glm::ivec4 norm;
	};

	struct triangle_pair_vert_indices{
		ushort t1_i1;
		ushort t1_i2;
		ushort t1_i3;
		ushort t2_i1;
		ushort t2_i2;
		ushort t2_i3;
	};
};

#endif