precision highp float;
precision highp int;

in Color{
	vec3 color;
}color_in;

out vec3 _retval;

in vec3 norm;

void main(){
	const vec3 sun_color = vec3(1.0f, 0.9725f, 0.9490f);
	float LdN = clamp(dot(norm, normalize(vec3(-1, 1, -0.5))), 0, 1);

	_retval = color_in.color * 0.15 + LdN * color_in.color * sun_color;
}