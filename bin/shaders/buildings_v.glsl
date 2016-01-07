precision highp float;
precision highp int;

uniform samplerBuffer tb_vertices;
uniform usamplerBuffer tb_flags;
uniform mat4 mvp;

out OUT {
	vec3 wpos;
} o;

void main()
{
	int index = (gl_InstanceID << 2) + (gl_VertexID & 3);
	vec2 xy = texelFetch(tb_vertices, index).xy;
	uint flags = texelFetch(tb_flags, gl_InstanceID).x;
	o.wpos = vec3(xy.x, float((gl_VertexID > 3) ? flags >> 4u: 0u) , -xy.y);
	vec4 opos = mvp * vec4(o.wpos, 1);

	opos.z = log2(max(1e-6, 1.0 + opos.w)) / log(1000000 + 1.0) - 1.0;
	opos.z *= opos.w;
	gl_Position = opos;
}
