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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _app->gpu_draw_frame(ctx);

	base::swap_buffers();

    static double start_time = timer.elapsed_time();
    static float frame = 0;
    static int test_cycles = 0;
    static float test_time = 0;

    double current_time = timer.elapsed_time();
    const float t = float(current_time - start_time);

    static base::stats_data test_stats;

    base::stats_data & stats = base::stats();
    test_stats += stats;

    if (t > 1000.f) {
		__int64 result[3] = { 0, };
		glGetQueryObjecti64v(ctx->_time_queries[0], GL_QUERY_RESULT, result);
		glGetQueryObjecti64v(ctx->_time_queries[1], GL_QUERY_RESULT, result + 1);
		const double coef_n2m = 1.0 / 1000000.0;
		const double time = double(result[1] - result[0]) * coef_n2m;
		const float fps = frame / (t * 0.001f);
		
		float dc_per_sec = stats._ndrawcalls * fps * (1.f / (1024.f));
		float tri_per_sec = float(stats._ntriangles) * fps * (1.f / (1024.f * 1024.f));

		printf("fps: %.0f cpu: %.2f gpu: %.2f dc: %u dc/s: %.0fk t/s: %.0fM t: %uk vtx: %uk buf: %uM\n",
			fps,
			ctx->_cpu_render_time,
			time,
			stats._ndrawcalls,
			dc_per_sec,
			tri_per_sec,
			stats._ntriangles >> 10,
			stats._nvertices >> 10,
			stats._buffer_mem >> 20);

		//_stat_data_buf._fps += fps;
        _stat_data_buf._nframes += int(frame);
		_stat_data_buf._cpu_render_time += ctx->_cpu_render_time;
		_stat_data_buf._gpu_render_time += time;
		//_stat_data_buf._dc_per_sec += dc_per_sec;
		//_stat_data_buf._tri_per_sec += tri_per_sec;
		//_stat_data_buf._count++;

		start_time = current_time;
        frame = 0;
        test_cycles++;
        test_time += t;

        if (base::cfg().test != -1 &&  test_cycles == 4) {
            _stat_data_buf.mean(test_time);

            write_test_data_csv(
                "result.csv",
                test_stats,
                _stat_data_buf,
                test_time,
                test_cycles - 1);

            _shutdown = true;
        }
    }

    frame += 1.f;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context* renderer::pop_frame_context_from_pool() {
	base::mutex_guard g(_mx_queue);
	return base::app::get()->pop_frame_context_from_pool();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool renderer::write_test_data_csv(
    const char * file_name,
    const base::stats_data & stats,
    const base::stats_data2 & stats_buf,
    const float time,
    const int num_test_cycles)
{
	FILE * pFile;
	pFile = fopen(file_name,"r+");

	if (pFile == NULL){
		pFile = fopen(file_name,"w");
		if (pFile == NULL){
			return false;
		}

		fputs(
            "test_name,"
            "gpu_gl_name,"
		    "use_vbo,"
			"tex_freq,"
			"mesh_size,"
			"one_mesh,"
			"fps,"
            "cpu_render_time (ms),"
            "gpu_render_time (ms),"
			"dc,"
            "dc_per_sec (Kc/s),"
            "tri_per_sec (Mtri/s),"
            "ntri (Mtri),"
            "nvert (Mvtx),"
            "buf_mem (MB),"
            "tex_mem (MB)",
            pFile);
	}
	else{
		fseek(pFile, 0, SEEK_END);
	}

	fprintf(
        pFile,
        "\n%s,%s,%s,%i,%i,%s,%u,%f,%f,%u,%u,%u,%u,%u,%u,%u",
		_app->get_test_name(),
		_graphic_card_name.c_str(),
		base::cfg().use_vbo ? "true" : "false",
        base::cfg().tex_freq,
        base::cfg().mesh_size,
        base::cfg().one_mesh ? "true" : "false",
        stats_buf._nframes,
		stats_buf._cpu_render_time,
		stats_buf._gpu_render_time,
        stats._ndrawcalls / stats_buf._nframes,
		stats._ndrawcalls / glm::uint64(time),//stats_buf._dc_per_sec,
        stats._ntriangles / glm::uint64(time * 1000),//stats_buf._tri_per_sec,
		(stats._ntriangles / stats_buf._nframes) / 1000,
        (stats._nvertices / stats_buf._nframes) / 1000,
		stats._buffer_mem >> 20,
        stats._texture_mem >> 20);

	fclose(pFile);

	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
