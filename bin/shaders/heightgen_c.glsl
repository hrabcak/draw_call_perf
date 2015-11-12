layout(binding = 0, r32ui) uniform writeonly uimage2D dst;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform ivec2 pos;

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

float random_1d_perm1(int coord)
{
	const int D = 1;
	int xp = coord;

	const int K = 2047483673;

	int p = (K * xp + D) * xp;
	p = (p & 0xffff) - 0x8000;//p - 16807*(p/16807);

	return float(p) * float(1.0 / 0x8000);
}

void main()
{
	ivec2 seed = pos*BLOCKSPERROW + ivec2(gl_GlobalInvocationID.xy);
	vec4 rnd = random_2d_perm(seed);


	imageStore(
		dst,
		ivec2(gl_GlobalInvocationID.xy),
		ivec4(int(((0.35 + rnd.x * 0.25) * 0xffff))));
		//ivec4(0));
}
