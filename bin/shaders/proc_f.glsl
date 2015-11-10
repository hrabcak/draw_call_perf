precision highp float;
precision highp int;

#ifdef USE_TEXTURE
uniform sampler2D grass_tex;
#endif

in Color{
#ifdef USE_TEXTURE
	vec2 uv;
#else
	vec3 color;
#endif
}color_in;

out vec3 _retval;

in vec3 norm;

void main(){
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);
	
	
#ifdef USE_TEXTURE
	vec3 col_grass = vec3(texture(grass_tex, color_in.uv));
	_retval = col_grass *0.15 + LdN * col_grass * sun_color;
#else
	_retval = color_in.color *0.15 + LdN * color_in.color * sun_color;
#endif
}