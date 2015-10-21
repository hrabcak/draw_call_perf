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

#include "benchmark.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "base/canvas.h"
#include "scene.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

benchmark::benchmark()
    : app()
    , _scene(new scene(this))
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

benchmark::~benchmark() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::start()
{
    // start renderer thread
    _renderer.reset(new renderer(this, SRC_LOCATION));
   
    app::start();

    _scene->post_gpu_init();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::gpu_init()
{
    app::gpu_init();

    _scene->init_gpu_stuff(SRC_LOCATION);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::stop()
{
	_renderer->stop(SRC_LOCATION);
	app::stop();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::draw_frame()
{
	base::frame_context *ctx = 0;
	
	// wait for renderer
	while((ctx = _renderer->pop_frame_context_from_pool()) == 0 && !_shutdown)
        base::sleep_ms(0);

    if (_shutdown)
        return;
	
	assert(ctx != 0);

	app::begin_frame();

	app::create_perspective_matrix(ctx);
	app::update_camera(ctx);

    _scene->update(ctx);

    //
    _canvas->fill_rect(
        ctx,
        glm::vec2(0),
        glm::vec2(500, 100),
        glm::vec4(0.0, 0.0, 0.0, 0.6));

    char tmp[512];
    sprintf(
        tmp,
        "vel(%.2f,%.2f)",
        _velocity.x,
        _velocity.z);
    
    _canvas->draw_text(
        ctx,
        glm::vec2(3),
        tmp,
        glm::vec4(1, 1, 1, 1),
        _fnt_mono.get());

	_renderer->push_frame_context(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::gpu_draw_frame(base::frame_context * const ctx)
{
    glDisable(GL_BLEND);
    _scene->gpu_draw(ctx);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    base::canvas::render(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::key(const int key, const bool down)
{
	app::key(key, down);

	if (key == 'N' && down){
		_scene->add_test_block();
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char* benchmark::get_test_name() const
{
    return base::cfg().test != -1
        ? _scene->get_test_name(base::cfg().test)
        : "Interactive";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
