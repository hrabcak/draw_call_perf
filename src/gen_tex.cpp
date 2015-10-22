#include "gen_tex.h"

#include "base/base.h"

#include <vector>

#define COMP_LATTICE_PROBABILITY		0.1f


base::rnd_int _gen_text_rnd_generator(9894);

uint32 u8vec4_to_uint32(const glm::u8vec4 & color){
	return uint32(color.x) | uint32(color.y << 8) | uint32(color.z << 16) | uint32(color.w << 24);
}

void gen_texture(
    glm::u8vec4 * const  tex_data,
    const ushort dim,
    const ushort grid_dim,
    const unsigned seed,
    uint32 * const grid)
{
	if (grid_dim <= 0){
		return;
	}

	int max = (int)(1 / COMP_LATTICE_PROBABILITY);

	int lattice_dim = dim / grid_dim;
	
	int base_col_rand = base::rndFromInterval(_gen_text_rnd_generator, -50, 50);
	
    glm::u8vec4 base_color(
        86 + base_col_rand,
        116 + base_col_rand,
        142 + base_col_rand + (base_col_rand & 0xf), 0);
	glm::u8vec4 complement_color1(100, 100, 100, 0);
	glm::u8vec4 complement_color2(234, 223, 205, 255);

	const uint32 bc = u8vec4_to_uint32(base_color);
	const uint32 cc1 = u8vec4_to_uint32(complement_color1);
    const uint32 cc2 = u8vec4_to_uint32(complement_color2);

    {
        uint32 * i = grid;
        uint32 * const e = i + grid_dim * grid_dim;
        do { *i++ = bc; } while (i != e);
    }

	for (int i = 0; i < grid_dim*grid_dim; i++){
		int rnd = base::rndFromInterval(_gen_text_rnd_generator, 1, max);
		if (rnd == 1){
			grid[i] = cc1;
		}
		else if (rnd == 3){
			grid[i] = cc2;
		}
	}

	uint32 * row = grid + grid_dim * grid_dim;

	for (ushort y = 0; y < dim; y += 4){
		for (ushort x = 0; x < dim; x++){
			row[x] = grid[(y / lattice_dim)*grid_dim + (x / lattice_dim)];
		}

		memcpy(tex_data + y*dim, row, dim * 4);
        memcpy(tex_data + (y+1)*dim, row, dim * 4);
        memcpy(tex_data + (y+2)*dim, row, dim * 4);
        memcpy(tex_data + (y+3)*dim, row, dim * 4);
    }
}

ushort rgb888_to_rgb565(uint8 r, uint8 g, uint8 b) {
	return ((ushort(r) >> 3) << 11) | (((ushort(g) >> 2)) << 5) | ((ushort(b) >> 3));
}

glm::ivec2 gen_dxt1_block(uint8 r, uint8 g, uint8 b, uint8 a) {
	ushort rgb565 = rgb888_to_rgb565(r, g, b);
	return glm::ivec2(rgb565 << 16 | rgb565, 0xaaaaaaaa);
}