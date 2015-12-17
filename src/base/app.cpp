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

#include "app.h"

#include "base.h"
#include "font.h"
#include "canvas.h"

#include "frame_context.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

base::app* base::app::_app = 0;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::app::app()
	: _velocity_boost(false)
    , _position(0.0, 10.3, 0.0)
	, _frame_number(0)
    , _fovy(45.f)
    , _aspect(float(get_wnd_width()) / float(get_wnd_height()))
	, _benchmark_mode(cfg().test != -1)

    , _shutdown(false)
{ 
	assert(_app == 0);
    _app = this; 

	// init mouse stuff
	base::set_mouse_pos(glm::ivec2(200));
	_last_mouse_pos = base::get_mouse_pos();
	_mouse_pos = glm::ivec2(45.8000183, 23.6000042);

	if (base::cfg().procedural_scene){
		_position = glm::vec3(0.0, 1.8, 0.0);
	}

	_position = glm::vec3(-1335.44604, 261.017517, 1306.41760);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::app::~app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::start()
{
	_canvas.reset(new base::canvas());
	_canvas->set_pos(10, 400);
	_canvas->set_size(1280, 260);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::gpu_init()
{
    canvas::load_and_init_shaders(SRC_LOCATION);

    _fnt_mono.reset(new base::font());
	_fnt_mono->load(SRC_LOCATION,"fonts/lucida_console_13.fnt");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::begin_frame()
{
	if(_active && !_benchmark_mode) {
		_mouse_pos += glm::vec2(base::get_mouse_pos() - _last_mouse_pos) * 0.2f;
		base::set_mouse_pos(glm::ivec2(200));
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::create_perspective_matrix(frame_context * const fc)
{
	fc->_mprj = glm::perspective(
		_fovy,
		_aspect,
		0.01f,
		500.0f);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::update_camera(frame_context * const fc)
{	
	fc->_view = glm::rotate(
		glm::rotate(
			glm::mat4(1),
			glm::radians(-_mouse_pos.x),
			glm::vec3(0,1,0)),
			glm::radians (- _mouse_pos.y), glm::vec3(1, 0, 0));

	_position += glm::mat3(fc->_view) * _velocity * (_velocity_boost ? 50.0f : 1.0f);

	fc->_view[3] = glm::vec4(_position, 1);
	fc->_mvp = fc->_mprj * glm::inverse(fc->_view);
	fc->_imvp = glm::inverse(fc->_mvp);

    fc->_fovy = _fovy;
    fc->_aspect = _aspect;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::stop()
{
	_canvas.reset(0);
	_fnt_mono.reset(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::mouse_move(const int, const int) {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::key(const int key,const bool down)
{
	const float speed = 0.05f;

	if(key == 0x57 || key == 0x26) _velocity.z += down ? -speed : speed;
	if(key == 0x53 || key == 0x28) _velocity.z += down ? speed : -speed;

	if(key == 0x41 || key == 0x25) _velocity.x += down ? -speed : speed;
	if(key == 0x44 || key == 0x27) _velocity.x += down ? speed : -speed;

	if(key == 0x52) _velocity.y += down ? speed : -speed;
	if(key == 0x46) _velocity.y += down ? -speed : speed;

	if(key == 0x10) _velocity_boost = down;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::activated(const bool state)
{
	_active=state;
	if(state)
		base::set_mouse_pos(glm::ivec2(200));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::create_frame_context_pool()
{
	for(int i = 0; i < base::frame_context::POOL_SIZE; ++i) {
		base::frame_context *ctx = new base::frame_context();
		ctx->create_buffers();
		_pool.push_front(ctx);
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
