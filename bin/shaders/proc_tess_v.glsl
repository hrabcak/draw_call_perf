precision highp float;
precision highp int;

#define PI_HALF 1.57079632679489661923
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692
#define SQRT_2 1.414213562

#define BLADE_WIDTH			0.03
#define BLADE_HEIGHT		0.1

uniform vec2 tile_pos;

uniform usampler2D height_map;

out Color{
#ifdef USE_TEXTURE
	vec2 uv;
#else
	vec3 color;
#endif
}color_out;

out vec3 norm_cs_in;
out vec3 vert_cs_in;


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

void main(){
	const vec3 up = vec3(0.0, 1.0, 0.0);
	const float block_width = TILEWIDTH / float(BLOCKSPERROW);
	const int bpr_log2 = int(log2(BLOCKSPERROW));
	const float half_block_width = 0.5*block_width;

	const int instance_id = gl_VertexID / 4;
	int vert_id = gl_VertexID % 4;
	vert_id = (vert_id == 3) ? 2 : vert_id;
	const int block_pos_part = gl_InstanceID;

	ivec2 block_pos_r = ivec2(block_pos_part & (BLOCKSPERROW - 1), (block_pos_part >> bpr_log2));

	uvec4 height = texelFetch(height_map, block_pos_r, 0);

	const float grass_h = float(height.x) / float(0xffff);

	vec2 block_pos = tile_pos*TILEWIDTH + vec2(block_pos_r.x * block_width, block_pos_r.y * block_width) + half_block_width;
	vec4 rnd = random_2d_perm(ivec2(block_pos * instance_id * BLOCKSPERROW));

	vec4 turf_pos = vec4(block_pos.x + rnd.x*half_block_width, 0.0, block_pos.y + rnd.y*half_block_width, 1.0);

	const float tan_angle = 0;//(TWO_PI / float(BLADESPERTUFT)) * instance_id - PI_HALF;

	vec3 blade_tangent = vec3(cos(tan_angle), 0.0, sin(tan_angle));

	vec3 bx_dis = blade_tangent * BLADE_WIDTH;

	vec4 blade_up_displace = vec4(up * grass_h, 0.0);

	float k = float(vert_id >> 1);

	norm_cs_in = normalize(cross(blade_up_displace.xyz, bx_dis));

	vert_cs_in = ((turf_pos + vec4(bx_dis  * ((vert_id & 1) - 0.5) + vec3(0, 0, 0), 0.0) + (blade_up_displace * k))).xyz;


#ifdef USE_TEXTURE
	color_out.uv = vec2(0.5 + (0.5*((vert_id & 1) - 0.5)* (1.0 - k)), k);
#else
	color_out.color = vec3(0.0, 0.29215, 0.0);
#endif
}