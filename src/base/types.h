#ifndef __ASYNC_VBO_TRANSFERS_TYPES_H__
#define __ASYNC_VBO_TRANSFERS_TYPES_H__

#include <glm/glm.hpp>
#include <glm/ext.hpp>

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned __int64 uint64;
typedef __int64 int64;

//typedef glm::vec4 float4;
//typedef glm::vec3 float3;
//typedef glm::vec2 float2;
//
//typedef glm::ivec4 int4;
//typedef glm::ivec3 int3;
//typedef glm::ivec2 int2;
//
typedef glm::uvec4 uint4;
typedef glm::uvec3 uint3;
typedef glm::uvec2 uint2;
//
//typedef glm::u16vec4 ushort4;
//typedef glm::u16vec3 ushort3;
//typedef glm::u16vec2 ushort2;
//
//typedef glm::i16vec4 short4;
//typedef glm::i16vec3 short3;
//typedef glm::i16vec2 short2;
//
typedef glm::u8vec4 uchar4;
typedef glm::u8vec3 uchar3;
typedef glm::u8vec2 uchar2;
//
//typedef glm::i8vec4 char4;
//typedef glm::i8vec3 char3;
//typedef glm::i8vec2 char2;

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