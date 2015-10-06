#version 330
precision highp float;
precision highp int;

layout(location=0) out vec3 _retval;

uniform sampler2D tex;

void main()
{
	ivec2 uv=ivec2(gl_FragCoord);
    _retval=texelFetch(tex,uv,0).xyz;
}
