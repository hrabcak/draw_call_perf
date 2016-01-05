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

#ifndef __ASYNC_VBO_TRANSFERS_FRAME_CONTEXT_H__
#define __ASYNC_VBO_TRANSFERS_FRAME_CONTEXT_H__

#include "types.h"
#include "base.h"

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

namespace base {

struct cmd
{
    unsigned _count;
    unsigned _instanceCount;
    unsigned _firstIndex;
    unsigned _baseVertex;
    unsigned _baseInstance;

    cmd(
        const unsigned count,
        const unsigned instanceCount,
        const unsigned firstIndex,
        const unsigned baseVertex,
        const unsigned baseInstance)
        : _count(count)
        , _instanceCount(instanceCount)
        , _firstIndex(firstIndex)
        , _baseVertex(baseVertex)
        , _baseInstance(baseInstance)
    {}
};

struct ctx_data
{
    glm::mat4 _mvp;
    int _mesh_size;
    int _tex_freq;
	char _res[256 - 64 - 4 - 4];
};

struct block_data
{
    glm::mat4 _tm;
};

/// batch represents group of canvas elements using same texture/font
struct batch {
	glm::vec2 pos;
	glm::vec2 size;
	int index;
	int count;
	unsigned tex;

	batch(
		const glm::vec2 &pos,
		const glm::vec2 &size,
		const int index,
		const int count,
		const int tex)
		: pos(pos)
		, size(size)
		, index(index)
		, count(count)
		, tex(tex)
	{}
};

typedef std::vector<batch> batches_t;

// draw call gpu data
struct dc_gpu_data
{
    const glm::uint _inst_data_index;
    const glm::uint _first_vertex;
    const glm::uint _inst_id;
    const glm::uint _res;

    dc_gpu_data(
        const glm::uint inst_data_index,
        const glm::uint first_vertex,
        const glm::uint inst_id)
        : _inst_data_index(inst_data_index)
        , _first_vertex(first_vertex)
        , _inst_id(inst_id)
        , _res(0)
    {}
};

struct frame_context
{
	static const int POOL_SIZE;

	frame_context();

	glm::mat4 _mprj;
	glm::mat4 _view;
	glm::mat4 _mvp;
	glm::mat4 _imvp;

	unsigned _canvas_vbo;
	unsigned _canvas_tb;

    unsigned _ctx_vbo;
	unsigned _scene_vbo;
	unsigned _scene_tb;
    unsigned _ctx_id;
    unsigned _drawid_vbo;
    unsigned _cmd_vbo;

    base::cmd* _cmd_data_ptr;

    base::ctx_data* _ctx_data_ptr;
    dc_gpu_data * _drawid_data_ptr;
    unsigned _drawid_data_offset;

	base::block_data* _scene_data_ptr;
	unsigned _scene_data_ptr_size;
    unsigned _scene_data_offset;

	int _num_visible_blocks[4];

	batches_t _batches;

	glm::vec4* _elements;
	glm::vec4* _elements_begin;

	glm::mat4 _showtime_mvp;
	unsigned _showtime_tex;

    GLuint _time_queries[2];
    double _cpu_render_time;

	GLsync _fence;

	void create_buffers();

	void put_fence();
	bool check_fence();

	bool first;

    float _fovy;
    float _aspect;

    base::stats_data _stats;
    __int64 _time;
};

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_FRAME_CONTEXT_H__
