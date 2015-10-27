layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

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

	vec4 base_position = gl_in[0].gl_Position;
	gl_Position = _ctx._mvp * base_position;
	EmitVertex();
	
	base_position.z += 0.01;
	gl_Position = _ctx._mvp * base_position;
	EmitVertex();
	
	base_position.x += 0.01;
	base_position.z -= 0.01;
	gl_Position = _ctx._mvp * base_position;
	EmitVertex();
}