precision highp float;
precision highp int;

#define PI_HALF 1.57079632679489661923
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

uniform vec2 tile_pos;

uniform usampler2D height_map;

out Color{
	#ifdef USE_TEXTURE
		vec2 uv;
	#else
		vec3 color;
	#endif
}color_out;

out vec3 blade_tangent;

out float grass_h;

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
	const int inst_part = gl_VertexID % BLADESPERTUFT;
	const int block_part = (gl_VertexID - inst_part) / BLADESPERTUFT;

#ifdef ONE_BATCH
	const int bpr_log2 = int(log2(BLOCKSPERROW));
	ivec2 block_pos_r = ivec2(block_part & (BLOCKSPERROW -1), block_part >> bpr_log2);
#else
	const int subdc_per_row = int(sqrt(DC_COUNT));
	const int subdc_per_row_log2 = int(log2(subdc_per_row));

	const int blocks_per_dc = BLOCKSPERROW / subdc_per_row;
	const int blocks_per_dc_log2 = int(log2(blocks_per_dc));

	ivec2 subdc_origin = ivec2((gl_InstanceID & (subdc_per_row - 1)), (gl_InstanceID >> subdc_per_row_log2)) * blocks_per_dc;

	ivec2 block_pos_r = subdc_origin + ivec2(block_part & (blocks_per_dc - 1), (block_part >> blocks_per_dc_log2));
#endif

	vec2 block_pos = tile_pos*TILEWIDTH + vec2(block_pos_r.x * block_width, block_pos_r.y * block_width) + half_block_width;
	vec4 rnd = random_2d_perm(ivec2(block_pos * inst_part * BLOCKSPERROW));

	vec4 turf_pos = vec4(block_pos.x + rnd.x*half_block_width, 0.0, block_pos.y+ rnd.y*half_block_width, 1.0);

#ifndef USE_TEXTURE
	color_out.color = vec3(0.0, 0.29215, 0.0);
#endif

	const float tan_angle = (TWO_PI / float(BLADESPERTUFT)) * inst_part - PI_HALF;

	blade_tangent = vec3(cos(tan_angle), 0.0, sin(tan_angle));

	uvec4 height = texelFetch(height_map, block_pos_r, 0);

	grass_h = float(height.x) / float(0xffff);

	gl_Position = turf_pos;
}