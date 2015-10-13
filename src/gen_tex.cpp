#include "gen_tex.h"

#include "base/utils.h"

#include <vector>

#define COMP_LATTICE_PROBABILITY		0.1f

uint32 u8vec4_to_uint32(const glm::u8vec4 & color){
	return uint32(color.x) | uint32(color.y << 8) | uint32(color.z << 16) | uint32(color.w << 24);
}

void gen_texture(glm::u8vec4 * tex_data, ushort dim, ushort grid_dim, unsigned seed){
	if (grid_dim <= 0){
		return;
	}

	srand(seed);

	int max = (int)(1 / COMP_LATTICE_PROBABILITY);

	int lattice_dim = dim / grid_dim;

	glm::u8vec4 base_color(0,200,0,255);
	glm::u8vec4 complement_color1(100, 100, 100, 0);
	glm::u8vec4 complement_color2(200, 200, 200, 0);

	const uint32 bc = u8vec4_to_uint32(base_color);
	const uint32 cc1 = u8vec4_to_uint32(complement_color1);
	const uint32 cc2 = u8vec4_to_uint32(complement_color2);

	std::vector<uint32> grid;
	grid.resize(grid_dim*grid_dim, bc);

	for (int i = 0; i < grid_dim*grid_dim; i++){
		int rnd = util::rndFromInterval(1, max);
		if (rnd == 1){
			grid[i] = cc1  | (util::rndFromInterval(100, 255)<<24);
		}
		else if (rnd == 2){
			grid[i] = cc2 | (util::rndFromInterval(100, 255) << 24);
		}
	}

	uint32 * row = new uint32[dim];

	for (ushort y = 0; y < dim; y++){
		if (y%lattice_dim){
			for (ushort x = 0; x < dim; x++){
				row[x] = grid[(y / lattice_dim)*grid_dim + (x / lattice_dim)];
			}
		}

		memcpy(tex_data + y*dim, row, dim * 4);
	}

	delete[] row;
}

ushort rgb888_to_rgb565(uint8 r, uint8 g, uint8 b) {
	return ((ushort(r) >> 3) << 11) | (((ushort(g) >> 2)) << 5) | ((ushort(b) >> 3));
}

glm::ivec2 gen_dxt1_block(uint8 r, uint8 g, uint8 b, uint8 a) {
	ushort rgb565 = rgb888_to_rgb565(r, g, b);
	return glm::ivec2(rgb565 << 16 | rgb565, 0xaaaaaaaa);
}