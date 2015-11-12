layout(vertices = 4) out;

in vec3 norm_cs_in[];
out vec3 norm_es_in[];

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
}color_out[];

in vec3 vert_cs_in[];
out vec3 vert_es_in[];


void main(){
	norm_es_in[gl_InvocationID] = norm_cs_in[gl_InvocationID];
#ifdef USE_TEXTURE
	color_out[gl_InvocationID].uv = color_in[gl_InvocationID].uv;
#else
	color_out[gl_InvocationID].color = color_in[gl_InvocationID].color;
#endif
	
	vert_es_in[gl_InvocationID] = vert_cs_in[gl_InvocationID];

	gl_TessLevelOuter[0] = 1;
	gl_TessLevelOuter[1] = 1;
	gl_TessLevelOuter[2] = 1;
	gl_TessLevelOuter[3] = 1;

	gl_TessLevelInner[0] = 0;
	gl_TessLevelInner[1] = 0;
	
}