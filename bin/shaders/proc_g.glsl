layout(points) in;
layout(triangle_strip, max_vertices = 7) out;

#define TRICOUNT 20
#define RADIUS 0.02
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

#define BLADE_WIDTH			0.02
#define BLADE_HEIGHT		0.1

// IN
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

in vec3 blade_tangent[];

in float grass_h[];

out vec3 norm;

void main(){
	const float hcf = 1.0 / 3.0;

	const vec3 up = vec3(0.0, 1.0, 0.0);

	vec3 bend = cross(up, blade_tangent[0]);

#ifndef USE_TEXTURE
	color_out.color = color_in[0].color;
#endif

	vec4 base_position = gl_in[0].gl_Position;

	vec3 bx_dis = blade_tangent[0] * BLADE_WIDTH;
	vec4 blade_up_displace = vec4(up * grass_h[0], 0.0) / 3.0;

	for (int i = 0; i < 7; i++){
		float k = (i >> 1) * hcf;

		vec3 bend_displace = bend*(1 - exp2(-1))*k*k*grass_h[0];

		norm = normalize(cross(blade_up_displace.xyz + bend_displace, bx_dis));

		gl_Position = _ctx._mvp * (base_position + vec4(bx_dis * (1.0 - k*k) * ((i & 1) - 0.5) + bend_displace, 0.0));


#ifdef USE_TEXTURE
		color_out.uv = vec2(0.5 + ((i & 1) - 0.5)* (1.0 - k*k), k);
#endif

		EmitVertex();

		if ((i & 1) == 1 && i > 0){
			base_position += blade_up_displace;
		}
	}
}