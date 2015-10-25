layout(binding = 0, rgba8) uniform writeonly image2DArray dst;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

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
    const vec4 c0[] = {
        vec4(0.6462, 0.7648, 0.8421, 1),
        vec4(0.1538, 0.1538, 0.1538, 0) * 0.5,
        vec4(0.3101, 0.2069, 0.1137, 0)
    };

    ivec2 seed = ivec2(gl_GlobalInvocationID.x >> 2, gl_GlobalInvocationID.y >> 1)
        + int(gl_GlobalInvocationID.z * 128);

    vec4 rnd = random_2d_perm(seed);
    int irnd = min(int(abs(rnd.w) * 16), 2);

    float rnd2 = random_1d_perm1(int(gl_GlobalInvocationID.z));

    vec4 color = c0[irnd];
    color.rgb += vec3(rnd2 * 0.1);
    color.r += abs(rnd2 * 0.05);

    imageStore(
        dst,
        ivec3(gl_GlobalInvocationID.xyz),
        color);
}
