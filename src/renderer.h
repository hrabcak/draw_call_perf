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

#ifndef __ASYNC_VBO_TRANSFERS_RENDERER_H__
#define __ASYNC_VBO_TRANSFERS_RENDERER_H__

#include "base/base.h"
#include "base/thread.h"
#include "base/event.h"
#include "base/mutex.h"

#include <list>

namespace base {
	struct frame_context;
	class app;
}

class renderer
	: public base::thread
{
protected:
	base::thread_event _event;
	std::list<base::frame_context*> _queue;
	std::list<base::frame_context*> _waiting;
	base::mutex _mx_queue;
	bool _shutdown;
	base::app *_app;
	std::string _graphic_card_name;
	std::string _graphic_card_driver;
	std::string _graphic_card_vendor;
public:

    renderer(base::app * const a, const base::source_location &loc);
	virtual ~renderer();

	virtual void run();

	virtual void stop(const base::source_location &loc);

	void draw_frame(base::frame_context * const ctx);

	void push_frame_context(base::frame_context *ctx) {
		base::mutex_guard g(_mx_queue);
		_queue.push_front(ctx);
		_event.signal();
	}

	base::frame_context* pop_frame_context_from_pool();

    const char* get_gpu_str() const { return _graphic_card_name.c_str(); }
	const char* get_gpu_driver_str() const { return _graphic_card_driver.c_str(); }
	const char* get_gpu_vendor_str() const { return _graphic_card_vendor.c_str(); }

    bool is_alive() { return !_shutdown; };
};

#endif // __ASYNC_VBO_TRANSFERS_RENDERER_H__