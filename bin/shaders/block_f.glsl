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

#version 430
precision highp float;
precision highp int;

#extension GL_ARB_bindless_texture : enable

in vec3 color;
in vec2 uv;
in flat uvec2 tex_handle;

#if defined(USE_ARRAY_TEX)
    uniform sampler2DArray mat_tex
#else
    uniform sampler2D mat_tex;
#endif

out vec3 _retval;

void main()
{
    float3 tex_color;

#if defined(USE_ARRAY_TEX)
    tex_color = texture(mat_tex, float3(uv, 0), 0).rgb;
#elif defined(USE_BINDLESS_TEX)
    tex_color = texture(mat_tex, uv, 0).rgb;
#else
    tex_color = texture(sampler2D(tex_handle), uv, 0).rgb;
#endif

    _retval = tex_color;
}
