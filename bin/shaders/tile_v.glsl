#define TILEWIDTH		1.0
#define BLOCKSPERROW	4

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
	const float block_w = TILEWIDTH / BLOCKSPERROW;
	const int vert_per_row = BLOCKSPERROW * 6;
	int row = gl_VertexID / vert_per_row;
	int col = (gl_VertexID % vert_per_row) / 6;
	int vert_ord = (gl_VertexID % vert_per_row) % 6;

	if (vert_ord == 5){
		vert_ord = 1;
	}
	else if (vert_ord == 4){
		vert_ord = 3;
	}
	else if (vert_ord == 3){
		vert_ord = 0;
	}

	vec4 base_vert = vec4(tile_pos.x * TILEWIDTH + col*block_w, -0.001, tile_pos.y * TILEWIDTH + row*block_w, 1.0);

	if (vert_ord == 0){
		gl_Position = _ctx._mvp * base_vert;
	}
	else if (vert_ord == 1){
		gl_Position = _ctx._mvp * (base_vert + vec4(block_w, 0.0, block_w, 0.0));
	}
	else if (vert_ord == 2){
		gl_Position = _ctx._mvp * (base_vert + vec4(block_w, 0.0, 0.0, 0.0));
	}
	else if (vert_ord == 3){
		gl_Position = _ctx._mvp * (base_vert + vec4(0.0, 0.0, block_w, 0.0));
	}

	if (row % 2 == 0){
		if (col % 2 != 0){
			color = vec3(0.0, 0.5, 0.3);
		}
		else{
			color = vec3(0.1, 0.1, 0.1);
		}
	}
	else{
		if (col % 2 == 0){
			color = vec3(0.0, 0.5, 0.3);
		}
		else{
			color = vec3(0.1, 0.1, 0.1);
		}
	}
}