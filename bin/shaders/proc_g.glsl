layout(points) in;
#ifdef USE_END_PRIMITIVE

layout(triangle_strip, max_vertices = VERTS_PER_GEOM_RUN ) out;

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

uniform vec2 tile_pos;

#else
layout(triangle_strip, max_vertices = 7) out;
#endif

#define PI_HALF 1.57079632679489661923
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
}color_in[];

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

in vec3 blade_tangent[];

in float grass_h[];

out vec3 norm;

uniform sampler2D height_map;

void main(){
	const float hcf = 1.0 / 3.0;

	const vec3 up = vec3(0.0, 1.0, 0.0);

	const vec4 blade_up_displace = vec4(up * grass_h[0], 0.0) / 3.0;

#ifndef USE_TEXTURE
	color_out.color = color_in[0].color;
#endif
#ifdef USE_END_PRIMITIVE
	const float block_width = TILEWIDTH / float(BLOCKSPERROW);
	const float half_block_width = 0.5*block_width;

	const float bpt_inverse = 1.0/BLADESPERTUFT;

	vec4 block_pos_r = gl_in[0].gl_Position;

	for(int blade_id = int(block_pos_r.z * BLADES_PER_GEOM_RUN); blade_id < int((block_pos_r.z + 1) * BLADES_PER_GEOM_RUN); blade_id++){
	//for(int blade_id = 0; blade_id <  8; blade_id++){	
		vec2 block_pos = tile_pos*TILEWIDTH + vec2(block_pos_r.x * block_width, block_pos_r.y * block_width) + half_block_width;

		vec4 rnd = random_2d_perm(ivec2(block_pos * blade_id * BLOCKSPERROW));

		vec4 base_position = vec4(block_pos.x + rnd.x*half_block_width, 0.0, block_pos.y+ rnd.y*half_block_width, 1.0);

		vec3 bx_dis = vec3( cos(TWO_PI*bpt_inverse*blade_id - PI_HALF), 0.0 , sin(TWO_PI*bpt_inverse*blade_id- PI_HALF)) * BLADE_WIDTH;

		vec3 bend = normalize(cross(up, bx_dis));

		for (int i = 0; i < 7; i++){
#if defined(IP_1F) || defined(IP_2F) || defined(IP_3F) || defined(IP_4F) 
			color_out.color = color_in[0].color * (i/7.0);
#endif

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

		EndPrimitive();
	}

#else
	vec4 base_position = gl_in[0].gl_Position;

	vec3 bx_dis = blade_tangent[0] * BLADE_WIDTH;
	
	vec3 bend = normalize(cross(up, blade_tangent[0]));

	for (int i = 0; i < 7; i++){
#if defined(IP_1F) || defined(IP_2F) || defined(IP_3F) || defined(IP_4F) 
		color_out.color = color_in[0].color * (i/7.0);
#endif
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

#endif
}