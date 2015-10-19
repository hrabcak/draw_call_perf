#ifndef __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__
#define __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__

#include "base/types.h"
#include "base/base.h"

struct voxel_info {
	uint32 vert_idx;
	uint32 mask;
};

void get_face_and_vert_count_for_tess_level(
    uint32 tess_level,
    uint32 & element_count,
    uint32 & vert_count);

void gen_cube_imp(
	ushort tess_level,
	float * pos_data,
	float * norm_uv_data,
	ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
    uint32 & element_count,
    uint32 & vert_count,
	bool use_int,
	bool deform,
	bool multipass);

template < class V >
void gen_cube(
    ushort tess_level,
    V * const vert_data,
    V * const norm_uv_data,
    ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
	uint32 & element_count,
	uint32 & vert_count,
	bool deform,
	bool multipass);

template <>
static void gen_cube<float>(
    ushort tess_level,
    float * const vert_data,
    float * const norm_uv_data,
    ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
	uint32 & element_count,
	uint32 & vert_count,
	bool deform,
	bool multipass)
{
	gen_cube_imp(
        tess_level,
        vert_data,
        norm_uv_data,
        index_array,
		voxel_map,
		voxel_inf,
        element_count,
        vert_count,
        false,
		deform,
		multipass);
}

template<>
static void gen_cube<glm::int2>(
    ushort tess_level,
    glm::int2 * const vert_data,
    glm::int2 * const norm_uv_data,
    ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
	uint32 & element_count,
	uint32 & vert_count, 
	bool deform,
	bool multipass)
{
	gen_cube_imp(
        tess_level,
        reinterpret_cast<float*>(vert_data),
        reinterpret_cast<float*>(norm_uv_data),
        index_array,
		voxel_map,
		voxel_inf,
        element_count,
        vert_count,
		true, 
		deform,
		multipass);
}


class voxel_gen
{
protected:

    static const glm::uint SIZE_BITS = 2;
    static const glm::uint BITS_Y = 2;
    static const glm::uint BITS_Z = 4;
    static const glm::uint CUBE_SIZE = 1 << 2;
    static const glm::uint SIZE_MASK = CUBE_SIZE - 1;
    static const glm::uint NVOXELS = CUBE_SIZE * CUBE_SIZE * CUBE_SIZE;

    ushort _voxels[NVOXELS];
    ushort _indices[24 * NVOXELS];
    ushort _mask_idx[6 * NVOXELS];

    ushort _nvertices;

public:

    voxel_gen()
        : _nvertices(0)
    {
        auto i = _voxels;
        auto e = i + CUBE_SIZE * CUBE_SIZE * CUBE_SIZE;
        do { *i++ = 1; } while (i != e);
    }

    inline uchar idx(const char x, const char y, const char z)
    {
        return x | (y << BITS_Y) | (z << BITS_Z);
    }

    inline bool is_there(const char x, const char y, const char z)
    {
        if (x < 0 || y < 0 || z < 0
            || x > SIZE_MASK || y > SIZE_MASK || z > SIZE_MASK)
            return false;

        return _voxels[idx(x, y, z)] != 0;
    }

    void create_masks()
    {
        ushort i = 0;
        ushort e = NVOXELS;
        do {
            const uchar x = i & SIZE_MASK;
            const uchar y = (i >> BITS_Y) & SIZE_MASK;
            const uchar z = (i >> BITS_Z) & SIZE_MASK;

            if (_voxels[i] == 0)
                continue;
            // 
            uchar mask = 0;
            mask |= uchar(is_there(x - 1, y, z));
            mask |= uchar(is_there(x + 1, y, z)) << 1;
            mask |= uchar(is_there(x, y - 1, z)) << 2;
            mask |= uchar(is_there(x, y + 1, z)) << 3;
            mask |= uchar(is_there(x, y, z - 1)) << 4;
            mask |= uchar(is_there(x, y, z + 1)) << 5;

            _voxels[i] = mask;
        } while (++i != e);
    }

    void triangulate()
    {
        ushort i = 0;
        ushort e = NVOXELS;
        do {
            const uchar x = i & SIZE_MASK;
            const uchar y = (i >> BITS_Y) & SIZE_MASK;
            const uchar z = (i >> BITS_Z) & SIZE_MASK;

            const ushort voxel = _voxels[i];

            if (voxel == 0)
                continue;

            for (uchar i = 0; i < 6; ++i) {
                const uchar mask = 1 << i;

                if (voxel & mask) {
                    // gen neighbor voxel 
                    if ((i & 1) == 0) {
                        
                    }
                    
                }                
            }

        } while (++i != e);
    }
};

#endif