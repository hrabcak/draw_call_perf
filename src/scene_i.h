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

#pragma once
#ifndef __DRAW_CALL_PERF_SCENE_I_H__
#define __DRAW_CALL_PERF_SCENE_I_H__

namespace base {
    class source_location;
    struct frame_context;
}

class scene_i
{
public:
    virtual void init_gpu_stuff(const base::source_location &loc) = 0;
    virtual void post_gpu_init() = 0;
    virtual void update(base::frame_context * const ctx) = 0;
    virtual void gpu_draw(base::frame_context * const ctx) = 0;
    virtual const char* get_test_name(const int i) const = 0;
};

#endif // __DRAW_CALL_PERF_SCENE_I_H__