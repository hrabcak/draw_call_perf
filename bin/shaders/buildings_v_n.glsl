precision highp float;
precision highp int;

uniform isamplerBuffer tb_blocks;
uniform ivec2 tile_offset;
uniform mat4 mvp;
uniform uint total_count;

out OUT{
	vec3 wpos;
} o;

/*
	Buiding packing
	base_point - relative to tile
			   - 16bits per component
	ohter 3 points - relative to base point
				   - 13 bits per components

	ivec4{
			upper bits -> lower bits
		x - base_point.x(16 bits), base_point.y(16 bits)
		y - p1.x(13 bits), p1.y(13 bits),p2.x(6 bits)
		z - p2.x(7 bits), p2.y(13 bits), p3.x(12 bits)
		w - p3.x(1 bit),p3.y(13 bits), falgs (18 bits)
	}

*/

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

	 if(index >= total_count){
		gl_Position = vec4(0.0, 0.0, 0.0, 0.0); 
		return;
	}

	ivec4 block = texelFetch(tb_blocks, index);
	
	vec2 xy;
	uint flags = unpack_ivec4_to_flags(block);

	vec2 base_point = unpack_ivec4_to_p1(block) + vec2(tile_offset) * t_width; // base point of buidling

	if ((vtxID & 3) == 0){
		xy = base_point;
	}
	else if((vtxID & 3) == 1){
		xy = base_point + unpack_ivec4_to_p2(block);
	}
	else if((vtxID & 3) == 2){
		xy = base_point + unpack_ivec4_to_p3(block);
	}
	else if((vtxID & 3) == 3){
		xy = base_point + unpack_ivec4_to_p4(block);
	}

	o.wpos = vec3(xy.x, (vtxID > 3) ? (float(flags) * 2.9) * 0.5 : 0.0, -xy.y);
	vec4 opos = mvp * vec4(o.wpos, 1);

	opos.z = log2(max(1e-6, 1.0 + opos.w)) / log(1000000 + 1.0) - 1.0;
	opos.z *= opos.w;
	gl_Position = opos;
}
