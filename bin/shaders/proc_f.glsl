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

uniform vec3 dummy;

void main(){
/*#ifdef USE_TEXTURE
	vec3 col_grass = vec3(texture(grass_tex, color_in.uv));

#ifdef PURE_COLOR
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(dummy, normalize(vec3(-1, 1, -0.5))), 0, 1);	
	_retval = col_grass *0.15 + LdN * col_grass * sun_color;
#else
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);
	_retval = col_grass *0.15 + LdN * col_grass * sun_color;
#endif

#else

#ifdef PURE_COLOR
	_retval = color_in.color; 
#else
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);
	_retval = color_in.color *0.15 + LdN * color_in.color * sun_color;
#endif

#endif*/
}