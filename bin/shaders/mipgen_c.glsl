layout(binding = 0, rgba8) uniform readonly image2DArray src;
layout(binding = 1, rgba8) uniform writeonly image2DArray dst;

layout(local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE, local_size_z = GROUP_DEPTH) in;

void main()
{
    ivec3 pos = ivec3(gl_GlobalInvocationID.xy << 1, gl_GlobalInvocationID.z);
    vec4 c0 = imageLoad(src, pos);
    vec4 c1 = imageLoad(src, pos + ivec3(1,0,0));
    vec4 c2 = imageLoad(src, pos + ivec3(1, 1, 0));
    vec4 c3 = imageLoad(src, pos + ivec3(0, 1, 0));

    imageStore(
        dst,
        ivec3(gl_GlobalInvocationID.xyz),
        (c0 + c1 + c2 + c3) * 0.25);
}
