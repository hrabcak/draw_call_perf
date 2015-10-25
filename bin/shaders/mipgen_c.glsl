layout(binding = 0, rgba8) uniform readwrite image2DArray tex;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main()
{
    ivec3 pos = ivec3(gl_GlobalInvocationID.xyz) << 1;
    vec4 c0 = imageLoad(tex, pos);
    vec4 c1 = imageLoad(tex, pos + ivec3(1,0,0));
    vec4 c2 = imageLoad(tex, pos + ivec3(1, 1, 0));
    vec4 c3 = imageLoad(tex, pos + ivec3(0, 1, 0));

    imageStore(
        tex,
        ivec3(gl_GlobalInvocationID.xyz),
        (c0 + c1 + c2 + c3) * 0.25);
}
