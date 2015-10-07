#ifndef __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__
#define __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__

#include "base/types.h"

void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count);
void gen_cube(uint32 tess_level, float * vert_data, ushort * index_array);

#endif