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

uniform vec2 tile_pos;

out vec3 color;

void main(){
	int vert_ord = gl_VertexID;

	if (vert_ord == 5){
		vert_ord = 1;
	}
	else if (vert_ord == 4){
		vert_ord = 3;
	}
	else if (vert_ord == 3){
		vert_ord = 0;
	}

	vec4 base_vert = vec4(tile_pos.x * TILEWIDTH, -0.001, tile_pos.y * TILEWIDTH, 1.0);

	if (vert_ord == 0){
		gl_Position = _ctx._mvp * base_vert;
	}
	else if (vert_ord == 1){
		gl_Position = _ctx._mvp * (base_vert + vec4(TILEWIDTH, 0.0, TILEWIDTH, 0.0));
	}
	else if (vert_ord == 2){
		gl_Position = _ctx._mvp * (base_vert + vec4(TILEWIDTH, 0.0, 0.0, 0.0));
	}
	else if (vert_ord == 3){
		gl_Position = _ctx._mvp * (base_vert + vec4(0.0, 0.0, TILEWIDTH, 0.0));
	}

	if (int(tile_pos.x + tile_pos.y) % 2 == 0){
		color = vec3(0.5, 0.25, 0.0);
	}
	else{
		color = vec3(0.317647, 0.156862, 0.0);
	}
}