precision highp float;
precision highp int;

#ifdef USE_TEXTURE
uniform sampler2D grass_tex;
#endif

in Color{
#ifdef USE_TEXTURE
	 vec2 uv;
#else
	#ifdef IP_1F
		float color;
	#elif defined(IP_2F)
		vec2 color;
	#elif defined(IP_3F)
		vec3 color;
	#elif defined(IP_4F)
		vec4 color;
	#else
		vec3 color;
	#endif
#endif
}color_in;

out vec4 _retval;

in vec3 norm;


void main(){
#ifdef USE_TEXTURE
	vec3 col_grass = vec3(texture(grass_tex,color_in.uv));

#ifdef PURE_COLOR
	_retval = vec4(col_grass,1.0);
#else
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);
	_retval = vec4(col_grass *0.15 + LdN * col_grass * sun_color,1.0);
#endif

#else

#ifdef PURE_COLOR
	#ifdef IP_1F
		_retval = vec4(color_in.color, 0.5, 0.5, 1.0);
	#elif defined(IP_2F)
		_retval = vec4(color_in.color, 0.5, 1.0);
	#elif defined(IP_3F)
		_retval = vec4(color_in.color,1.0);
	#elif defined(IP_4F)
		_retval = color_in.color;
	#else
		_retval = vec4(0,1,0,1);
	#endif
#else
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);
	//_retval = color_in.color *0.15 + LdN * color_in.color * sun_color;

	_retval = vec4(color_in.color * 0.15 + LdN * color_in.color * sun_color,0.0);
#endif

#endif
}