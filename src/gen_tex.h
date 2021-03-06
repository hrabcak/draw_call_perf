#ifndef __ASYNC_VBO_TRANSFERS_GEN_TEX_H__
#define __ASYNC_VBO_TRANSFERS_GEN_TEX_H__
#include "base/types.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

void gen_texture(glm::u8vec4 * tex_data, ushort dim, ushort grid_dim, unsigned seed, uint32 * const tmp);
glm::ivec2 gen_dxt1_block(ushort r, ushort g, ushort b, ushort a);

#endif