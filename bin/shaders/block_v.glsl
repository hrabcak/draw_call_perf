/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#version 430
precision highp float;
precision highp int;

#extension GL_ARB_explicit_attrib_location : enable

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

//#define USE_UNIFORM_BUF 1

// IN
struct context_data
{
	mat4 _mvp;
};

layout(std140) uniform context
{
	context_data _ctx;
};

#ifdef USE_UNIFORM_BUF
	struct block_data
	{
		mat4 _tm;
	};

	layout(std140) uniform tb_blocks
	{
		block_data _blocks;
	};
#else
	layout(location=13) in ivec4 index_start;
    uniform samplerBuffer tb_blocks;
#endif

uniform isamplerBuffer tb_pos;

//OUT 
out vec3 color;


vec3 unpack_position(ivec2 pack_pos, float coef)
{
    return vec3(
        pack_pos.x >> 11,
        ((pack_pos.x << 21) >> 11) | (pack_pos.y >> 21),
        (pack_pos.y << 11) >> 11)
        * coef;
}

void main()
{
	int inst_id = 0;
	int vertex_id = gl_VertexID;

#ifdef USE_UNIFORM_BUF
	mat4 tm = _ctx._mvp * _blocks._tm;
#else
    int index = vertex_id >> 12;
    vertex_id &= 0xfff;
    int idx = (index_start.x + index + gl_InstanceID) * 16;
	mat4 tm = _ctx._mvp * mat4(
		texelFetch(tb_blocks, idx),
		texelFetch(tb_blocks, idx + 1),
		texelFetch(tb_blocks, idx + 2),
		texelFetch(tb_blocks, idx + 3));
#endif

    ivec2 tmp0 = texelFetch(tb_pos, gl_InstanceID * 96 + index_start.y + vertex_id /** 2*/).xy;
    //vec4 tmp1 = texelFetch(tb_pos, index_start.y + vertex_id * 2 + 1);

    vec3 pos = unpack_position(tmp0.xy, 1.0 / 1048575.0);
    //vec3 nor = tmp0.xyz;// vec3(tmp0.w, tmp1.xy);
    //vec2 uv = tmp1.zw;

    gl_Position = tm * vec4(pos, 1);
	color=pos * 0.5 + 0.5;
}
