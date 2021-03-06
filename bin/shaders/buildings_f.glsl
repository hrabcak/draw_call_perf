precision highp float;
precision highp int;

in OUT {
	vec3 wpos;
} inp;

out vec4 _retval;

void main()
{
	vec3 norm = normalize(cross(dFdx(inp.wpos), dFdy(inp.wpos)));
	float d = clamp(dot(norm, normalize(vec3(1, 1, 0))), 0, 1);
	_retval = vec4(norm,0.0);
}
