precision highp float;
precision highp int;

#define PI_HALF 1.57079632679489661923
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

uniform vec2 tile_pos;

uniform usampler2D height_map;


out Color{
#ifdef USE_TEXTURE
	vec2 uv;
#else
	#ifdef IP_1F
		float color;
	#elif defined(IP_2F)
		vec2 color;
	#elif defined(IP_3F)
		vec3 color;
	#elif defined(IP_4F)
		vec4 color;
	#else
		vec3 color;
	#endif
#endif
}color_out;

out vec3 norm;

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


#ifdef VARIABLE_BLADES_PER_INSTANCE

void get_blade_data_variable_blades_per_dc(out int blade_vtx_id, out int blade_instance_id, out ivec2 tuft_pos_r){
	const int bpr_log2 = int(log2(BLOCKSPERROW));
	int global_blade_id = -1;

#ifdef USE_IDX_BUF
	blade_vtx_id = gl_VertexID % VERT_PER_BLADE;
	global_blade_id = gl_InstanceID * BLADES_PER_INSTANCE + (gl_VertexID / VERT_PER_BLADE);
#else
	if(BLADES_PER_INSTANCE==1){
		blade_vtx_id = gl_VertexID;
		global_blade_id = gl_InstanceID;

	}else {
		blade_vtx_id = gl_VertexID % (VERT_PER_BLADE + 2);
		blade_vtx_id = (blade_vtx_id == 0) ? 0 : blade_vtx_id - 1;
		blade_vtx_id = (blade_vtx_id == VERT_PER_BLADE) ? VERT_PER_BLADE - 1 : blade_vtx_id;

		global_blade_id = gl_InstanceID * BLADES_PER_INSTANCE + (gl_VertexID / (VERT_PER_BLADE + 2));
	}
#endif

	global_blade_id = ((global_blade_id & 1) << 15 ) | 
		((global_blade_id & 2) << 13 ) |
		((global_blade_id & 4) << 11 ) |
		((global_blade_id & 8) << 9 ) |
		((global_blade_id & 16) << 7 ) |
		((global_blade_id & 32) << 5 ) |
		((global_blade_id & 64) << 3 ) |
		((global_blade_id & 128) << 1 ) |
		((global_blade_id & 0xff00) >> 8 );

	blade_instance_id = global_blade_id & 15;

	tuft_pos_r.x = (global_blade_id >> 4) & 63;
	tuft_pos_r.y = global_blade_id >> 10;

}

#endif

void main(){
	const float hcf = 1.0 / (VERT_PER_BLADE >> 1);
	const vec3 up = vec3(0.0, 1.0, 0.0);
	const float block_width = TILEWIDTH / float(BLOCKSPERROW);
	const int bpr_log2 = int(log2(BLOCKSPERROW));
	const float half_block_width = 0.5*block_width;

#ifdef USE_IDX_BUF
	const int verts_per_block = VERT_PER_BLADE*BLADESPERTUFT;
#else
	const int verts_per_block = (VERT_PER_BLADE + 2)*BLADESPERTUFT;
#endif
	
	const int block_part = gl_VertexID % verts_per_block;
	const int instance_part = (gl_VertexID - block_part) / verts_per_block;


#ifdef VARIABLE_BLADES_PER_INSTANCE 
	int vertex_id = -1;
	int instance_id = -1;
	ivec2 block_pos_r;
	
	get_blade_data_variable_blades_per_dc(vertex_id,instance_id,block_pos_r);

#elif defined(ONE_BATCH)
#ifndef USE_TRIANGLES
#ifdef USE_IDX_BUF
	int vertex_id = block_part % (VERT_PER_BLADE );
	int instance_id = block_part / VERT_PER_BLADE;
#else
	int vertex_id = block_part % (VERT_PER_BLADE + 2);
	vertex_id = (vertex_id == 0) ? 0 : vertex_id - 1;
	vertex_id = (vertex_id == VERT_PER_BLADE) ? VERT_PER_BLADE - 1 : vertex_id;
	int instance_id = block_part / (VERT_PER_BLADE + 2);
#endif
	ivec2 block_pos_r = ivec2(instance_part & (BLOCKSPERROW - 1), (instance_part >> bpr_log2));
#else
	int vertex_id = gl_VertexID & 7;
	int instance_id = (gl_VertexID >> 4) & 15;
	ivec2 block_pos_r = ivec2((gl_VertexID >> 14) & 0x3f, (gl_VertexID >> 8) & 0x3f);
#endif
#else


	const int subdc_per_row = int(sqrt(DC_COUNT));
	const int subdc_per_row_log2 = int(log2(subdc_per_row));

	const int blocks_per_dc = BLOCKSPERROW / subdc_per_row;
	const int blocks_per_dc_log2 = int(log2(blocks_per_dc));

	ivec2 subdc_origin = ivec2((gl_InstanceID & (subdc_per_row - 1)), (gl_InstanceID >> subdc_per_row_log2)) * blocks_per_dc;
#ifndef USE_TRIANGLES 	
#ifdef USE_IDX_BUF
	int vertex_id = block_part % (VERT_PER_BLADE );
	int instance_id = block_part / VERT_PER_BLADE;
#else
	int vertex_id = block_part % (VERT_PER_BLADE + 2);
	vertex_id = (vertex_id == 0) ? 0 : vertex_id - 1;
	vertex_id = (vertex_id == VERT_PER_BLADE) ? VERT_PER_BLADE-1 : vertex_id;
	int instance_id = block_part / (VERT_PER_BLADE + 2);
#endif
	ivec2 block_pos_r = subdc_origin + ivec2(instance_part & (blocks_per_dc - 1), (instance_part >> blocks_per_dc_log2));
#else
	int vertex_id = gl_VertexID & 7;
	int instance_id = (gl_VertexID >> 4) & 15;
	ivec2 block_pos_r = subdc_origin + ivec2((gl_VertexID >> 14) & 0x3f, (gl_VertexID >> 8) & 0x3f);
#endif	

#endif
	uvec4 height = texelFetch(height_map, block_pos_r,0);

	const float grass_h = float(height.x) / float(0xffff);

	vec2 block_pos = tile_pos*TILEWIDTH + vec2(block_pos_r.x * block_width, block_pos_r.y * block_width) + half_block_width;
	vec4 rnd = random_2d_perm(ivec2(block_pos * instance_id * BLOCKSPERROW));

	vec4 blade_pos = vec4(block_pos.x + rnd.x*half_block_width, 0.0, block_pos.y + rnd.y*half_block_width, 1.0);

	const float tan_angle = (TWO_PI / float(BLADESPERTUFT)) * instance_id - PI_HALF;

	vec3 blade_tangent = vec3(cos(tan_angle), 0.0, sin(tan_angle));

	vec3 bx_dis = blade_tangent * BLADE_WIDTH;

	vec4 blade_up_displace = vec4(up * grass_h, 0.0);

	float k = (vertex_id >> 1) * hcf;

#ifdef WITHOUT_BENDING
	vec3 bend = cross(up, blade_tangent);

	vec3 bend_displace = bend*(1 - exp2(-1))*k*k*BLADE_HEIGHT;

	norm = normalize(cross(blade_up_displace.xyz + bend_displace, bx_dis));

	gl_Position = _ctx._mvp * (turf_pos + vec4(bx_dis * (1.0 - k*k) * ((vertex_id & 1) - 0.5) + vec3(0,0,0), 0.0) + (blade_up_displace * k));
#else
	vec3 bend = cross(up, blade_tangent);

	vec3 bend_displace = bend*(1 - exp2(-1))*k*k*grass_h;

	norm = normalize(cross(blade_up_displace.xyz * hcf + bend_displace, bx_dis));

	gl_Position = _ctx._mvp * (blade_pos + vec4(bx_dis * (1.0 - k*k) * ((vertex_id & 1) - 0.5) + bend_displace, 0.0) + (blade_up_displace * k));

#endif
	
#ifdef USE_TEXTURE
	color_out.uv = vec2(0.5 + (0.5*((vertex_id & 1) - 0.5)* (1.0 - k*k)), k);
#else
	rnd = random_2d_perm(ivec2(gl_VertexID))*0.5 + 0.5;
	#ifdef IP_1F
		color_out.color = rnd.x;
	#elif defined(IP_2F)
		color_out.color = rnd.xy;
	#elif defined(IP_3F)
		color_out.color = rnd.xyz;
	#elif defined(IP_4F)
		color_out.color = rnd;
	#else
		color_out.color = vec3(0.0, 0.29215, 0.0);
	#endif
#endif
}