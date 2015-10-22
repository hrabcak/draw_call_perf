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
#include <fstream>

#include <glm/glm.hpp>

using namespace glm;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

renderer::renderer(base::app * const a, const base::source_location &loc)
	: thread()
	, _event()
	, _queue()
	, _mx_queue()
	, _shutdown(false)
	, _app(a)
{
    thread::start(loc);

    if (!_event.wait(1000000))
        throw base::exception(loc.to_str())
        << "Renderer initialization failed!";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

renderer::~renderer() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::run()
{
	base::init_opengl_win();
	base::init_opengl_dbg_win();

	_graphic_card_name = (char*)glGetString(GL_RENDERER);

	// create frame_context pool
	{
		base::mutex_guard g(_mx_queue);
		base::app::get()->create_frame_context_pool();
	}

    _app->gpu_init();

	_event.signal();

	for(;;) {
		base::frame_context *ctx = 0;

		if(!_waiting.empty() && _waiting.front()->check_fence()) {
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

		ctx->put_fence();
		_waiting.push_back(ctx);

		if(_shutdown)
			break;
	}

	//TODO purge frame context pool

	std::cout << "Renderer thread ended...\n";

    _app->shutdown();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void renderer::stop(const base::source_location &loc)
{
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

void renderer::draw_frame(base::frame_context * const ctx)
{
    base::hptimer timer;
   
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.f, 1.f, 1.0f);
    //glClearColor(0.0f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _app->gpu_draw_frame(ctx);

	base::swap_buffers();

    base::stats_data & stats = base::stats();

    __int64 result[2] = { 0, };
    glGetQueryObjecti64v(ctx->_time_queries[0], GL_QUERY_RESULT, result);
    glGetQueryObjecti64v(ctx->_time_queries[1], GL_QUERY_RESULT, result + 1);
    const double coef_n2m = 1.0 / 1000000.0;
    const double gpu_time = double(result[1] - result[0]) * coef_n2m;

    ctx->_stats = stats;
    ctx->_stats._gpu_time = float(gpu_time);
    ctx->_stats._cpu_time = float(ctx->_cpu_render_time);
    ctx->_time = timer.time();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context* renderer::pop_frame_context_from_pool() {
	base::mutex_guard g(_mx_queue);
	return base::app::get()->pop_frame_context_from_pool();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
