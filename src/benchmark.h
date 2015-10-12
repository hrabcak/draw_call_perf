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

#ifndef __ASYNC_VBO_TRANSFERS_APP_SCENARIO2_H__
#define __ASYNC_VBO_TRANSFERS_APP_SCENARIO2_H__

#include "base/app.h"
#include "base/thread.h"

#include <memory>
#include <assert.h>

#include <glm/glm.hpp>

class renderer;

class benchmark
	: public base::app
{
protected:
	std::auto_ptr<scene> _scene;
	std::auto_ptr<renderer> _renderer;

public:
	benchmark();
	virtual ~benchmark();

	virtual const char* get_app_name() const override { return "draw call benchmark"; }
    virtual const char* get_wnd_name() const override { return get_app_name(); }
    virtual const char* get_wnd_cls() const override { return "benchmark class"; }

	virtual void start() override;
    virtual void gpu_init() override;

	virtual void stop() override;

	virtual void draw_frame() override;

    virtual void gpu_draw_frame(base::frame_context * const ctx) override;

private:
	benchmark(const benchmark&);
	void operator =(const benchmark&);
};

#endif // __ASYNC_VBO_TRANSFERS_APP_SCENARIO2_H__
