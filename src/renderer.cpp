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

#include "renderer.h"
#include "scene.h"

#include "base/base.h"
#include "base/canvas.h"
#include "base/frame_context.h"
#include "base/app.h"
#include "base/hptimer.h"

#include <iostream>

#include <glm/glm.hpp>

using namespace glm;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

renderer::renderer(base::app *a, const bool create_shared_context)
	: thread()
	, _event()
	, _queue()
	, _mx_queue()
	, _shutdown(false)
	, _app(a)
	, _create_shared_context(create_shared_context)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

renderer::~renderer() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::start(const base::source_location &loc)
{
	thread::start(loc);

	if(!_event.wait(1000000))
		throw base::exception(loc.to_str())
			<< "Renderer initialization failed!";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::run()
{
	base::init_opengl_win(_create_shared_context);
	base::init_opengl_dbg_win();

	// create frame_context pool
	{
		base::mutex_guard g(_mx_queue);

		base::app::get()->create_frame_context_pool(!_create_shared_context);
	}

	base::canvas::load_and_init_shaders(SRC_LOCATION);
	scene::load_and_init_shaders(SRC_LOCATION);

	_app->renderer_side_start();

	_event.signal();


	// TEST
	base::pixelfmt fmt = base::PF_BGRA8;
	GLuint buf;
	int texsize = 1024;
	int bufsize = texsize * texsize * base::get_pfd(fmt)->_size;
	int counter = 0;

	int prg =base::create_program(
		base::create_and_compile_shader(SRC_LOCATION,"shaders/vs.glsl",GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(SRC_LOCATION,"shaders/fs.glsl",GL_FRAGMENT_SHADER));
	
	base::link_program(SRC_LOCATION, prg);

	int prg_tex = get_uniform_location(SRC_LOCATION, prg, "tex");

	glGenBuffers(1, &buf);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, bufsize, 0, GL_STREAM_DRAW);

	GLuint tex = base::create_texture(texsize,texsize,fmt,0);

	for(;;) {
		base::frame_context *ctx = 0;

		if(!_waiting.empty() && _waiting.front()->check_fence()) {
			if(!_create_shared_context) {
				_waiting.front()->map_scene();
				//_waiting.front()->map_canvas();
			}

			base::mutex_guard g(_mx_queue);
			base::app::get()->push_frame_context_to_pool(_waiting.front());
			
			_waiting.pop_front();
		}		

		{
			base::mutex_guard g(_mx_queue);
			if(!_queue.empty()) {
				ctx = _queue.back();
				_queue.pop_back();
			}
		}

		if(ctx==0) {
			if(_shutdown) break;
			continue;
		}

		draw_frame(ctx);

		// TEST
		/*{
			glUseProgram(prg);
			glUniform1i(prg_tex, 1);
	        glActiveTexture(GL_TEXTURE0);
		    glBindTexture(GL_TEXTURE_2D, tex);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}*/

		ctx->put_fence();
		_waiting.push_back(ctx);

		if(!_create_shared_context && counter++ > 3) {
			//base::set_shared_rc();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf);
			void * data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
			//void * data = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, bufsize, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			//memset(data, 0xab, bufsize);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			glBindTexture(GL_TEXTURE_2D, tex);
			glTexSubImage2D(
				GL_TEXTURE_2D, 0, 0, 0, texsize, texsize, 
				base::get_pfd(fmt)->_format, base::get_pfd(fmt)->_type, 0);

			counter = 0;
			//base::set_main_rc();
		}

		if(_shutdown)
			break;
	}

	//TODO purge frame context pool

	std::cout << "Renderer thread ended...\n";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::stop(const base::source_location &loc)
{
	assert(_shutdown==false);

	std::cout << "Renderer is going down...\n";
	
	_shutdown = true;
	_event.signal();

	if(!wait_for_end(2000)) {
		std::cout << "Terminating renderer thread!\n";
		terminate();
	}

	thread::stop(loc);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::draw_frame(base::frame_context *ctx)
{
    base::hptimer timer;
    
    static double start_time = timer.elapsed_time();

    double current_time = timer.elapsed_time();
    if (current_time - start_time > 1000.0) {
        __int64 result[3] = { 0, };
        glGetQueryObjecti64v(ctx->_time_queries[0], GL_QUERY_RESULT, result);
        glGetQueryObjecti64v(ctx->_time_queries[1], GL_QUERY_RESULT, result + 1);
        const double coef_n2m = 1.0 / 1000000.0;
        const double time = double(result[1] - result[0]) * coef_n2m;
        printf("cpu render time: %.2f gpu render time: %.2f\n", ctx->_cpu_render_time, time);
        start_time = current_time;
    }


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// unmap buffers in SC2
	if(!_create_shared_context) {
		ctx->flush_scene_data();
		//ctx->flush_canvas_data();
	}

	scene::render_blocks(ctx);
	//base::canvas::render(ctx);

	if(_create_shared_context) {
		//_app->capture_screen();
	}

	base::swap_buffers();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context* renderer::pop_frame_context_from_pool() {
	base::mutex_guard g(_mx_queue);
	return base::app::get()->pop_frame_context_from_pool();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
