precision highp float;
precision highp int;

uniform isamplerBuffer tb_blocks;
uniform ivec2 tile_offset;
uniform mat4 mvp;
uniform uint total_count;

out OUT{
	vec3 wpos;
} o;

struct Building{
	uint flags;
	vec2 pnts[4];
};

Building unpack_ivec4_to_building(ivec4 packed_block){
	Building result;
	const float scale16 = 2500.0f / 32767.0f;
	const float scale13 = 312.5f / 4095.0f;

	result.pnts[0].x = (packed_block.x >> 16) * scale16;
	result.pnts[0].y = ((packed_block.x << 16) >> 16) * scale16;

	result.pnts[1].x = (packed_block.y >> 19) * scale13;
	result.pnts[1].y = ((packed_block.y << 13) >> 19) * scale13;

	result.pnts[2].x = (((packed_block.y << 26) >> 19) | ((packed_block.z >> 25) & 0x7f)) * scale13;
	result.pnts[2].y = ((packed_block.z << 7) >> 19) * scale13;

	result.pnts[3].x = (((packed_block.z << 20) >> 19) | ((packed_block.w >> 31) & 0x1))* scale13;
	result.pnts[3].y = ((packed_block.w << 1) >> 19)* scale13;

	result.pnts[1] += result.pnts[0];
	result.pnts[2] += result.pnts[0];
	result.pnts[3] += result.pnts[0];

	result.flags = uint(packed_block.w & 0xff);

	return result;
}

vec2 unpack_ivec4_to_p1(ivec4 packed_block){
	vec2 result;
	const float scale16 = 2500.0f / 32767.0f;

	result.x = (packed_block.x >> 16) * scale16;
	result.y = ((packed_block.x << 16) >> 16) * scale16;

	return result;
}

vec2 unpack_ivec4_to_p2(ivec4 packed_block){
	vec2 result;
	const float scale13 = 312.5f / 4095.0f;

	result.x = (packed_block.y >> 19) * scale13;
	result.y = ((packed_block.y << 13) >> 19) * scale13;

	return result;
}

vec2 unpack_ivec4_to_p3(ivec4 packed_block){
	vec2 result;
	const float scale13 = 312.5f / 4095.0f;

	result.x = (((packed_block.y << 26) >> 19) | ((packed_block.z >> 25) & 0x7f)) * scale13;
	result.y = ((packed_block.z << 7) >> 19) * scale13;

	return result;
}

vec2 unpack_ivec4_to_p4(ivec4 packed_block){
	vec2 result;
	const float scale13 = 312.5f / 4095.0f;

	result.x = (((packed_block.z << 20) >> 19) | ((packed_block.w >> 31) & 0x1))* scale13;
	result.y = ((packed_block.w << 1) >> 19)* scale13;

	
	return result;
}

uint unpack_ivec4_to_flags(ivec4 packed_block){
	return uint(packed_block.w & 0xff);
}

void main()
{
	const float t_width = 2500.0f;
	int vtxID = gl_VertexID & 7;
	int index = gl_InstanceID * BLOCKS_PER_IDC + (gl_VertexID >> 3);

	 /*(index >= total_count){
		gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}*/

	ivec4 block = texelFetch(tb_blocks, index);
	
	vec2 xy;
	uint flags = unpack_ivec4_to_flags(block);

	if ((vtxID & 3) == 0){
		xy = unpack_ivec4_to_p1(block) + vec2(tile_offset) * t_width;
	}
	else if((vtxID & 3) == 1){
		xy = unpack_ivec4_to_p1(block) + unpack_ivec4_to_p2(block) + vec2(tile_offset) * t_width;
	}
	else if((vtxID & 3) == 2){
		xy = unpack_ivec4_to_p1(block) + unpack_ivec4_to_p3(block) + vec2(tile_offset) * t_width;
	}
	else if((vtxID & 3) == 3){
		xy = unpack_ivec4_to_p1(block) + unpack_ivec4_to_p4(block) + vec2(tile_offset) * t_width;
	}

	o.wpos = vec3(xy.x + float(total_count) * 0.0000000000001, (vtxID > 3) ? (float(flags) * 2.9) : 0u, -xy.y);
	vec4 opos = mvp * vec4(o.wpos, 1);

	opos.z = log2(max(1e-6, 1.0 + opos.w)) / log(1000000 + 1.0) - 1.0;
	opos.z *= opos.w;
	gl_Position = opos;
}
