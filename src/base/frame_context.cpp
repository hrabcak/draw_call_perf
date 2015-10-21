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

#include "frame_context.h"
#include "base/base.h"
#include "base/pixelfmt.h"

#include "canvas.h"
#include "../scene.h"

#include <gl/glew.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const int base::frame_context::POOL_SIZE = 4;

bool __buffers_created = false;
GLuint __scene_buffer = 0;
GLuint __canvas_buffer = 0;
GLuint __scene_tb = 0;
GLuint __elem_buffer = 0;
GLuint __vert_buffer = 0;
GLuint __ctx_buffer = 0;
GLuint __drawid_buffer = 0;
GLuint __cmd_buffer = 0;
base::cmd * __cmd_data_ptr = 0;
base::dc_gpu_data * __drawid_data_ptr = 0;
base::block_data * __scene_data_ptr = 0;
base::ctx_data * __ctx_data_ptr = 0;

unsigned ctx_counter = 0;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context::frame_context()
    : _scene_data_ptr(0)
    , _scene_data_ptr_size(0)
    , _elements(0)
    , _elements_begin(0)
    , _fence(0)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::create_buffers()
{
	first = true;

    _ctx_id = ctx_counter++;

    if (!__buffers_created) {
        glGenBuffers(1, &__drawid_buffer);
        _drawid_vbo = __drawid_buffer;
        glGenBuffers(1, &__cmd_buffer);
        _cmd_vbo = __cmd_buffer;

        _ctx_vbo = __ctx_buffer = create_buffer(1 * POOL_SIZE, &__ctx_data_ptr);
        _ctx_data_ptr = __ctx_data_ptr;

        _scene_vbo = __scene_buffer = create_buffer(
            scene::MAX_BLOCK_COUNT * POOL_SIZE,
            &__scene_data_ptr);
        _scene_data_ptr = __scene_data_ptr;
        _scene_data_offset = 0;

        _drawid_vbo = __drawid_buffer = create_buffer<dc_gpu_data>(
            scene::MAX_BLOCK_COUNT * POOL_SIZE,
            &__drawid_data_ptr);
        _drawid_data_ptr = __drawid_data_ptr;
        _drawid_data_offset = 0;
        
        // bind drawid buffer to 13
        glBindBuffer(GL_ARRAY_BUFFER, _drawid_vbo);
        glVertexAttribIPointer(13, 4, GL_INT, 0, (GLvoid*)0);
        glVertexAttribDivisor(13, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // create COMMAND buffer for multi draw indirect
        {
            const int size = scene::MAX_BLOCK_COUNT * sizeof(base::cmd) * 4;
            unsigned flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

            glBindBuffer(GL_ARRAY_BUFFER, __cmd_buffer);
            glBufferStorage(GL_ARRAY_BUFFER, size, 0, flags);
            _cmd_data_ptr = __cmd_data_ptr =
                reinterpret_cast<base::cmd*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, flags));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // scene buffer
        glGenTextures(1, &__scene_tb);
        _scene_tb = __scene_tb;
        glBindTexture(GL_TEXTURE_BUFFER, _scene_tb);
        glTexBuffer(
            GL_TEXTURE_BUFFER,
            base::get_pfd(base::PF_RGBA32F)->_internal,
            _scene_vbo);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        __buffers_created = true;
	}
	else {
		_scene_vbo = __scene_buffer;
        _scene_tb = __scene_tb;
        _drawid_vbo = __drawid_buffer;
        _cmd_vbo = __cmd_buffer;

        _scene_data_offset = scene::MAX_BLOCK_COUNT * _ctx_id;
        _scene_data_ptr = __scene_data_ptr + _scene_data_offset;

        _cmd_data_ptr = __cmd_data_ptr + scene::MAX_BLOCK_COUNT * _ctx_id;

        _ctx_vbo = __ctx_buffer;
        _ctx_data_ptr = __ctx_data_ptr + _ctx_id;

        _drawid_data_ptr = __drawid_data_ptr;
        _drawid_data_offset = scene::MAX_BLOCK_COUNT * _ctx_id;
    }

    // create canvas buffer
    glGenBuffers(1, &_canvas_vbo);
    _canvas_vbo = base::create_buffer<glm::float4>(
        base::canvas::ELEMENTS_VBO_SIZE,
        &_elements,
        0);
    _elements_begin = _elements;

    // canvas buffer TB
    glGenTextures(1, &_canvas_tb);
    glBindTexture(GL_TEXTURE_BUFFER, _canvas_tb);
    glTexBuffer(
        GL_TEXTURE_BUFFER,
        base::get_pfd(base::PF_RGBA32F)->_internal,
        _canvas_vbo);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    glGenQueries(sizeof(_time_queries) / sizeof(*_time_queries), _time_queries);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::put_fence()
{
	if(_fence) glDeleteSync(_fence);

	_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool base::frame_context::check_fence()
{
	if(_fence==0) return true;

	GLenum res = glClientWaitSync(_fence, 0, 500000);
	
	// AMD returns GL_CONDITION_SATISFIED only BUG?
	return res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
