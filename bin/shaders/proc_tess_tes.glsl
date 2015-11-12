layout(triangles, equal_spacing, ccw) in;

struct context_data
{
	mat4 _mvp;
	int _mesh_size;
	int _tex_freq;
};

layout(std140) uniform context
{
	context_data _ctx;
};

in vec3 norm_es_in[];
out vec3 norm;

in Color{
#ifdef USE_TEXTURE
	vec2 uv;
#else
	vec3 color;
#endif
}color_in[];

out Color{
#ifdef USE_TEXTURE
	vec2 uv;
#else
	vec3 color;
#endif
}color_out;

in vec3 vert_es_in[];

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main(){
	gl_Position = _ctx._mvp * vec4(interpolate3D(vert_es_in[0], vert_es_in[1], vert_es_in[2]),1.0);
#ifdef USE_TEXTURE
	color_out.uv = interpolate2D(color_in[0].uv, color_in[1].uv, color_in[2].uv);
#else
	color_out.color = color_in[0].color;
#endif	

	norm = interpolate3D(norm_es_in[0], norm_es_in[1], norm_es_in[2]);
}