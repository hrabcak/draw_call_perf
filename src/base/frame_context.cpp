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

#define GL_EXTERNAL_VIRTUAL_MEMORY_AMD 37216 // AMD_pinned_memory

const int base::frame_context::POOL_SIZE = 4;

bool __buffers_created = false;
GLuint __scene_buffer = 0;
GLuint __canvas_buffer = 0;
GLuint __scene_tb = 0;
GLuint __elem_buffer = 0;
GLuint __ctx_buffer = 0;
GLuint __drawid_buffer = 0;
GLuint __cmd_buffer = 0;
base::cmd * __cmd_data_ptr = 0;
int * __drawid_data_ptr = 0;
base::block_data * __scene_data_ptr = 0;
base::ctx_data * __ctx_data_ptr = 0;

unsigned ctx_counter = 0;

void base::frame_context::create_buffers()
{
	first = true;

    _ctx_id = ctx_counter++;

    if (!__buffers_created) {
        glGenBuffers(1, &__scene_buffer);
        _scene_vbo = __scene_buffer;
        glGenBuffers(1, &__elem_buffer);
        _elem_vbo = __elem_buffer;
        glGenBuffers(1, &__ctx_buffer);
        _ctx_vbo = __ctx_buffer;
        glGenBuffers(1, &__drawid_buffer);
        _drawid_vbo = __drawid_buffer;
        glGenBuffers(1, &__cmd_buffer);
        _cmd_vbo = __cmd_buffer;

        {
            glBindBuffer(GL_UNIFORM_BUFFER, _ctx_vbo);
            unsigned flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            const unsigned size = 4 * sizeof(base::ctx_data);

            glBufferStorage(GL_UNIFORM_BUFFER, size, 0, flags);

            _ctx_data_ptr = __ctx_data_ptr = reinterpret_cast<base::ctx_data*>(
                glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, flags));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        {
            glBindBuffer(GL_UNIFORM_BUFFER, _scene_vbo);
            unsigned flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            const unsigned size = 4 * scene::MAX_BLOCK_COUNT * sizeof(base::block_data);

            glBufferStorage(GL_UNIFORM_BUFFER, size, 0, flags);

            _scene_data_ptr = __scene_data_ptr = reinterpret_cast<base::block_data*>(
                glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, flags));
            _scene_data_offset = 0;
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        {
            const int size = 4096;

            unsigned short * const data = new unsigned short[size];
            for (int i = 0; i < size; ++i) data[i] = unsigned short(i);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __elem_buffer);
            glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, size * 2, data, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            delete[] data;
        }
        {
            const int size = scene::MAX_BLOCK_COUNT * sizeof(int) * 4/*int4*/ * 4;
            unsigned flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

            glBindBuffer(GL_ARRAY_BUFFER, __drawid_buffer);
            glBufferStorage(GL_ARRAY_BUFFER, size, 0, flags);
            glEnableVertexAttribArray(13);
            glVertexAttribIPointer(13, 4, GL_INT, 0, (GLvoid*)0);
            glVertexAttribDivisor(13, 1);

            _drawid_data_ptr = __drawid_data_ptr =
                reinterpret_cast<int*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, flags));
            _drawid_data_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
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
        //glGenBuffers(1, tmp);
		//_canvas_vbo = __canvas_buffer;
		_scene_vbo = __scene_buffer;
        _elem_vbo = __elem_buffer;
        _scene_tb = __scene_tb;
        _drawid_vbo = __drawid_buffer;
        _cmd_vbo = __cmd_buffer;

        _scene_data_offset = scene::MAX_BLOCK_COUNT * _ctx_id;
        _scene_data_ptr = __scene_data_ptr + _scene_data_offset;

        _cmd_data_ptr = __cmd_data_ptr + scene::MAX_BLOCK_COUNT * _ctx_id;

        _ctx_vbo = __ctx_buffer;
        _ctx_data_ptr = __ctx_data_ptr + _ctx_id;

        _drawid_data_ptr = __drawid_data_ptr;
        _drawid_data_offset = scene::MAX_BLOCK_COUNT * _ctx_id * 4/*int4*/;
    }
    
    // canvas buffer
    //glGenBuffers(1, &_canvas_vbo);
    //glGenTextures(1, &_canvas_tb);
    //glBindTexture(GL_TEXTURE_BUFFER, _canvas_tb);
    //glTexBuffer(
    //    GL_TEXTURE_BUFFER,
    //    base::get_pfd(base::PF_RGBA32F)->_internal,
    //    _canvas_vbo);
    //glBindTexture(GL_TEXTURE_BUFFER, 0);

    glGenQueries(sizeof(_time_queries) / sizeof(*_time_queries), _time_queries);

    if (_mode == ModePersistent) {
        // canvas
        glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
        glBufferData(
            GL_TEXTURE_BUFFER,
            base::canvas::ELEMENTS_VBO_SIZE,
            0,
            GL_STREAM_DRAW);
    }
	else if(_mode != ModeAMDpinned) {
		// canvas
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		glBufferData(
			GL_TEXTURE_BUFFER,
			base::canvas::ELEMENTS_VBO_SIZE,
			0,
			GL_STREAM_DRAW);
			
		//scene
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
		glBufferData(
			GL_TEXTURE_BUFFER,
			scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
			0,
			GL_STREAM_DRAW);
	}
	else {
		{
			char *ptr = new char[base::canvas::ELEMENTS_VBO_SIZE * sizeof(glm::vec4) + 0x1000];
			char *ptr_aligned = reinterpret_cast<char*>(unsigned(ptr + 0xfff) & (~0xfff));
			_elements_begin = reinterpret_cast<glm::vec4*>(ptr_aligned);

			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, _canvas_vbo);
			glBufferData(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, base::canvas::ELEMENTS_VBO_SIZE, _elements_begin, GL_STREAM_READ);
			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, 0);
		}
		{
			char *ptr = new char[scene::MAX_BLOCK_COUNT * sizeof(glm::mat4) + 0x1000];
			char *ptr_aligned = reinterpret_cast<char*>(unsigned(ptr + 0xfff) & (~0xfff));
			_scene_data_ptr = reinterpret_cast<base::block_data*>(ptr_aligned);

			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, _scene_vbo);
			glBufferData(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, scene::MAX_BLOCK_COUNT * sizeof(glm::mat4), _scene_data_ptr, GL_STREAM_READ);
			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, 0);
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context::frame_context()
	: _scene_data_ptr(0)
	, _scene_data_ptr_size(0)
	, _elements(0)
	, _elements_begin(0)
	, _fence(0)
	, _mode(ModePersistent)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::assign_buffer_range(const int) {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::map_scene()
{
	if(_mode == ModeInvalidateBuffer) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
        _scene_data_ptr = reinterpret_cast<base::block_data*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				scene::MAX_BLOCK_COUNT * sizeof(base::block_data),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	}
	else if(_mode == ModeFlushExplicit) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
        _scene_data_ptr = reinterpret_cast<base::block_data*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
                scene::MAX_BLOCK_COUNT * sizeof(base::block_data),
				GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
	}
	else if(_mode == ModeUnsynchronized) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
        _scene_data_ptr = reinterpret_cast<base::block_data*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
                scene::MAX_BLOCK_COUNT * sizeof(base::block_data),
				GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
	}
    else if (_mode == ModePersistent) {
        // DO NOTHING pointer is persistent
    }
    else if (_mode == ModeWrite) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
        _scene_data_ptr = reinterpret_cast<base::block_data*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
                scene::MAX_BLOCK_COUNT * sizeof(base::block_data),
				GL_MAP_WRITE_BIT));
	}
	else if(_mode == ModeBufferData || _mode == ModeBufferSubData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
		if(!_scene_data_ptr)
            _scene_data_ptr = new base::block_data[scene::MAX_BLOCK_COUNT];
	}
	else if(_mode == ModeAMDpinned) {
	}

	//glBindBuffer(GL_TEXTURE_BUFFER, 0);
	_scene_data_ptr_size = 0;

	assert(_scene_data_ptr != 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::map_canvas()
{
	if(_mode == ModeInvalidateBuffer) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	}
	else if(_mode == ModeFlushExplicit) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
	}
	else if(_mode == ModeUnsynchronized || _mode == ModePersistent) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
	}
	else if(_mode == ModeWrite) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT));
	}
	else if(_mode == ModeBufferData || _mode == ModeBufferSubData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		if(!_elements_begin)
			_elements_begin = new glm::vec4[base::canvas::ELEMENTS_VBO_SIZE];
		_elements = _elements_begin;
	}
	else if(_mode == ModeAMDpinned) {
		_elements = _elements_begin;
	}

	//glBindBuffer(GL_TEXTURE_BUFFER, 0);

	assert(_elements_begin != 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::flush_scene_data()
{
	if(_mode != ModeBufferData && _mode != ModeBufferSubData && _mode != ModeAMDpinned) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
		if(_mode == ModeFlushExplicit && _scene_data_ptr_size > 0) {
			glFlushMappedBufferRange(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size);
		}
		glUnmapBuffer(GL_TEXTURE_BUFFER);
	}
	else if(_mode == ModeBufferData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
		glBufferData(GL_TEXTURE_BUFFER, _scene_data_ptr_size, 0, GL_STREAM_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size, _scene_data_ptr);
	}
	else if(_mode == ModeBufferSubData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size, _scene_data_ptr);
	}
	else if(_mode == ModeAMDpinned) {
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::flush_canvas_data()
{
	const int size = (_elements - _elements_begin) * sizeof(glm::vec4) * 3;

	if(_mode != ModeBufferData && _mode != ModeBufferSubData && _mode != ModeAMDpinned) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		if(_mode == ModeFlushExplicit && _elements - _elements_begin > 0) {
			glFlushMappedBufferRange(GL_TEXTURE_BUFFER, 0, size);
		}
		glUnmapBuffer(GL_TEXTURE_BUFFER);
	}
	else if(_mode == ModeBufferData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STREAM_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size, _elements_begin);
	}
	else if(_mode == ModeBufferSubData) {
		glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size, _elements_begin);
	}
	else if(_mode == ModeAMDpinned) {
	}
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
