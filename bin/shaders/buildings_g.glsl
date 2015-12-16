#version 330
precision highp float;
precision highp int;

////////////////////////////////////////////////////////////////////////////////
in OUT {
	vec3 wpos;
	vec4 opos;
} inp[3];

out OUT2{
	vec3 wpos;
} o;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main()
{
	if ((inp[2].opos.x - inp[0].opos.x) * (inp[0].opos.y - inp[1].opos.y) >= (inp[2].opos.y - inp[0].opos.y) * (inp[0].opos.x - inp[1].opos.x)) {

		for (int i = 0; i < 3; i++) {
			o.wpos = inp[i].wpos;
			gl_Position = inp[i].opos;
			EmitVertex();
		}
	}
}
