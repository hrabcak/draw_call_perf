layout(points) in;
layout(triangle_strip, max_vertices = 60) out;

#define TRICOUNT 20
#define RADIUS 0.02
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

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
	vec3 color;
}color_in [];

out Color{ 
	vec3 color;
}color_out;

void main(){
	color_out.color = color_in[0].color;
	
	const float step = TWO_PI / float(TRICOUNT);

	vec4 base_position = gl_in[0].gl_Position;

	vec4 base_proj = _ctx._mvp * base_position;

	for (int i = 0; i < TRICOUNT; i++){
		gl_Position = base_proj;
		EmitVertex();

		vec4 p = base_position + vec4(cos(i*step)*RADIUS, 0.0, sin(i*step)*RADIUS, 0.0);
		gl_Position = _ctx._mvp * p;
		EmitVertex();

		p = base_position + vec4(cos((i + 1)*step)*RADIUS, 0.0, sin((i + 1)*step)*RADIUS, 0.0);
		gl_Position = _ctx._mvp * p;
		EmitVertex();
	}
}