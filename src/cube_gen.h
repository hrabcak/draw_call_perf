#ifndef __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__
#define __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__

#include "base/types.h"
#include "base/base.h"

typedef void *(*cpy_fun)(void * data_ptr, const gen::vert & vert_data);

void * cpy_float(void * data_ptr, const gen::vert & vert_data);
void * cpy_int(void * data_ptr, const gen::vert & vert_data);


void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count);


void gen_cube_imp(ushort tess_level, float * vert_data, ushort * index_array, uint32 & element_count, uint32 & vert_count, cpy_fun trait);

template < class V >
void gen_cube(ushort tess_level, float * vert_data, ushort * index_array, uint32 & element_count, uint32 & vert_count);

template <>
static void gen_cube<float>(ushort tess_level, float * vert_data, ushort * index_array, uint32 & element_count, uint32 & vert_count){
	gen_cube_imp(tess_level, vert_data, index_array, element_count, vert_count, cpy_float);
}

template <>
static void gen_cube<int>(ushort tess_level, float * vert_data, ushort * index_array, uint32 & element_count, uint32 & vert_count){
	gen_cube_imp(tess_level, vert_data, index_array, element_count, vert_count, &cpy_int);
}
#endif