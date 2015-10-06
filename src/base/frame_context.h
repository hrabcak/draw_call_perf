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

#include <glm/glm.hpp>

#include <vector>

#include <GL/glew.h>

namespace base {

//typedef  struct {
//    uint  count;
//    uint  instanceCount;
//    uint  firstIndex;
//    uint  baseVertex;
//    uint  baseInstance;
//} DrawElementsIndirectCommand;

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
    float _num_sides;
    char _res[256 - 64 - 4];
};

struct block_data
{
    glm::mat4 _tm;
    char _res[256 - 64];
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

enum Mode {
	ModeInvalidateBuffer,
	ModeFlushExplicit,
	ModeUnsynchronized,
	ModeBufferData,
	ModeBufferSubData,
	ModeWrite,
	ModeAMDpinned,
    ModePersistent,
};

struct frame_context
{
	static const int POOL_SIZE;

	frame_context();

	Mode _mode;

	glm::mat4 _mprj;
	glm::mat4 _view;
	glm::mat4 _mvp;

	unsigned _canvas_vbo;
	unsigned _canvas_tb;

    unsigned _ctx_vbo;
	unsigned _scene_vbo;
    unsigned _elem_vbo;
	unsigned _scene_tb;
    unsigned _ctx_id;
    unsigned _drawid_vbo;
    unsigned _cmd_vbo;

    base::cmd* _cmd_data_ptr;

    base::ctx_data* _ctx_data_ptr;
    int * _drawid_data_ptr;
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
	
	void map_scene();
	void map_canvas();

	void flush_scene_data();
	void flush_canvas_data();

	void create_buffers();
	void assign_buffer_range(const int index);

	void test_transfer();

	void put_fence();
	bool check_fence();

	bool first;
};

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_FRAME_CONTEXT_H__
