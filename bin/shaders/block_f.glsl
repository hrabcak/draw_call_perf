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

precision highp float;
precision highp int;

in vec3 color;
in vec2 uv;
in vec3 wpos;

#if defined(USE_NAIVE_TEX)
    uniform sampler2D mat_tex;
#elif defined(USE_ARRAY_TEX)
    uniform sampler2DArray mat_tex;
    in flat float tex_slice;
#elif defined(USE_BINDLESS_TEX)
    #extension GL_ARB_bindless_texture : enable
    in flat uvec2 tex_handle;
#endif

out vec4 _retval;

void main()
{
    vec4 tex_color;

#if defined(USE_NAIVE_TEX)
    tex_color = texture(mat_tex, uv, 0);
#elif defined(USE_ARRAY_TEX)
    tex_color = texture(mat_tex, vec3(uv, tex_slice), 0);
#elif defined(USE_BINDLESS_TEX)
    tex_color = texture(sampler2D(tex_handle), uv, 0);
#else
    tex_color = vec4(vec3(0.5), 0);
#endif

    const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);

    vec3 nor = normalize(cross(dFdx(wpos), dFdy(wpos)));
    float fog = clamp(exp2(-0.01 * (1.0 / gl_FragCoord.w)), 0.0, 1.0);
    float LdN = clamp(dot(nor, normalize(vec3(-1, 1, -0.5))), 0, 1);
    //tex_color.rgb = tex_color.rgb * LdN * sun_color + tex_color.rgb * 0.15;
    tex_color.rgb =
        tex_color.rgb * LdN * sun_color + tex_color.rgb * 0.15
        + tex_color.a * vec3(254, 239,100) * (1.0 / 255.0);

    //if (tex_color.a == 0.0) discard;

    _retval = vec4(tex_color.rgb * fog + vec3(1.0f * (1-fog)),0.9);
}
