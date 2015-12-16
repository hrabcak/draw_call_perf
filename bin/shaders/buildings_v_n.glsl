#version 330
precision highp float;
precision highp int;

uniform isamplerBuffer tb_blocks;
uniform mat4 mvp;

out OUT{
	vec3 wpos;
} o;

struct Building{
	uint flags;
	vec2 pnts[4];
};

Building unpack_ivec4_to_building(ivec4 packed_block){
	Building result;
	const float interval = 2500.0f;
	const float i2 = 312.5f;
	const float scale16 = 1.0f / 65535.0f;
	const float scale13 = 1.0f / 8191.0f;

	result.pnts[0].x = (packed_block.x >> 16) * scale16 * interval;
	result.pnts[0].y = ((packed_block.x << 16) >> 16) * scale16 * interval;

	result.pnts[1].x = (packed_block.y >> 19) * scale13 * i2;
	result.pnts[1].y = ((packed_block.y << 13) >> 19) * scale13 * i2;

	result.pnts[2].x = (((packed_block.y << 26) >> 19) | ((packed_block.z >> 25) & 0x7f)) * scale13 * i2;
	result.pnts[2].y = ((packed_block.z << 7) >> 19) * scale13 * i2;

	result.pnts[3].x = (((packed_block.z << 20) >> 19) | ((packed_block.w >> 31) & 0x1))* scale13 * i2;
	result.pnts[3].y = ((packed_block.w << 1) >> 19)* scale13 * i2;

	result.pnts[1] += result.pnts[0];
	result.pnts[2] += result.pnts[0];
	result.pnts[3] += result.pnts[0];

	result.flags = uint(packed_block.w & 0xff);

	return result;
}

void main()
{
	ivec4 block = texelFetch(tb_blocks, gl_InstanceID);
	Building b = unpack_ivec4_to_building(block);

	vec2 xy = b.pnts[gl_VertexID & 3];

	o.wpos = vec3(xy.x, (gl_VertexID > 3) ? b.flags >> 4u : 0u, -xy.y);
	vec4 opos = mvp * vec4(o.wpos, 1);

	opos.z = log2(max(1e-6, 1.0 + opos.w)) / log(1000000 + 1.0) - 1.0;
	opos.z *= opos.w;
	gl_Position = opos*0.00001 + block;
}
