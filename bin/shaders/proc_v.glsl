precision highp float;
precision highp int;

#define PI_HALF 1.57079632679489661923
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

#define BLADESPERTUFT			16
#define BLOCKSPERROW			64
#define TILEWIDTH				10.0

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

out Color{
	vec3 color;
}color_out;

out vec3 blade_tangent;

vec4 random_2d_perm(ivec2 coord)
{
	const ivec4 D = ivec4(1, 17, -43, 71);
	int xp = coord.x;
	int yp = coord.y;

	const ivec4 K = ivec4(2047483673, 1951307951, 1815206923, 1664526161);

	ivec4 p = (K * xp + D) * xp;
	p = (p & 0xffff) + yp;
	p = (K*p + D)*p;
	p = (p & 0xffff) - 0x8000;

	return vec4(p) * float(1.0 / 0x8000);
}

void main()
{
	const float block_width = TILEWIDTH / float(BLOCKSPERROW);
	const float half_block_width = 0.5*block_width;
	int vertex_id = gl_VertexID;
	vec2 block_pos = tile_pos*TILEWIDTH + vec2((vertex_id%BLOCKSPERROW) * block_width, (vertex_id / BLOCKSPERROW) * block_width) + half_block_width;
	vec4 rnd = random_2d_perm(ivec2(block_pos * gl_InstanceID * BLOCKSPERROW));

	vec4 turf_pos = vec4(block_pos.x + rnd.x*half_block_width, 0.0, block_pos.y + rnd.y*half_block_width, 1.0);

	color_out.color = vec3(0.0, 0.29215, 0.0);

	const float tan_angle = (TWO_PI / float(BLADESPERTUFT)) * gl_InstanceID - PI_HALF;

	blade_tangent = vec3(cos(tan_angle), 0.0, sin(tan_angle));

	gl_Position = turf_pos;

}