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

precision highp float;
precision highp int;

#extension GL_ARB_explicit_attrib_location : enable

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

// IN
struct context_data
{
	mat4 _mvp;
};

layout(std140) uniform context
{
	context_data _ctx;
};

layout(location = 13) in ivec4 index_start;
uniform samplerBuffer tb_blocks;

#ifdef USE_TB_FOR_VERTEX_DATA
    uniform isamplerBuffer tb_pos;
    uniform isamplerBuffer tb_nor_uv;
#else
    layout(location = 0) in ivec2 in_pos;    // NOT IMPLEMENTED YET
    layout(location = 1) in ivec2 in_nor_uv;
#endif

ivec2 get_vertex_pos_data(int vertex_id)
{
#ifdef USE_TB_FOR_VERTEX_DATA
    return texelFetch(tb_pos, vertex_id).xy;
#else
    return in_pos;
#endif
}

#if defined(USE_BINDLESS_TEX)
    uniform usamplerBuffer tb_tex_handles;
    out flat uvec2 tex_handle;
#elif defined(USE_ARRAY_TEX)
    out flat float tex_slice;
#endif

//OUT 
out vec3 color;
out vec2 uv;
out vec3 wpos;

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
	int vertex_id = gl_VertexID;
    int index = vertex_id >> 12;
    vertex_id &= 0xfff;
    int idx = (index_start.x + gl_InstanceID) * 16;
	mat4 tm = mat4(
		texelFetch(tb_blocks, idx),
		texelFetch(tb_blocks, idx + 1),
		texelFetch(tb_blocks, idx + 2),
		texelFetch(tb_blocks, idx + 3));

#if defined(USE_BASE_INSTANCE) || defined(USE_INDIRECT_DRAW)
    int inst_id = index_start.z;
#else
    int inst_id = index_start.x + gl_InstanceID;
#endif

    ivec2 tmp0 = get_vertex_pos_data(gl_InstanceID * 96/*54*/ + index_start.y + vertex_id);
    vec3 pos = unpack_position(tmp0.xy, 1.0 / 1048575.0);
    if (pos.z == -1 || pos.z == 1)
        uv = pos.xy;
    else if (pos.x == -1 || pos.x == 1)
        uv = pos.zy;
    else if (pos.y == -1 || pos.y == 1)
        uv = pos.xz;
    uv = uv * 0.5 + 0.5;

    //inst_id >>= 2;

#if defined(USE_BINDLESS_TEX)
    tex_handle = texelFetch(tb_tex_handles, inst_id).xy;
#elif defined(USE_ARRAY_TEX)
    tex_slice = inst_id & 0x7ff;
#endif

    wpos = (tm * vec4(pos, 1)).xyz;
    gl_Position = _ctx._mvp * vec4(wpos,1);
	color = pos * 0.5 + 0.5;
}
