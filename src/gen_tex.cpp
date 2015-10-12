#include "gen_tex.h"

#include "base/utils.h"

#include <vector>

#define COMP_LATTICE_PROBABILITY		0.25f

void gen_texture(glm::u8vec4 * tex_data, ushort dim, ushort grid_dim, unsigned seed){
	if (grid_dim <= 0){
		return;
	}

	srand(seed);

	int max = (int)(1 / COMP_LATTICE_PROBABILITY);

	int lattice_dim = dim / grid_dim;

	glm::u8vec4 base_color(50, 50, 50, 255);
	glm::u8vec4 complement_color(200, 200, 200, 255);

	std::vector<glm::u8vec4> grid;
	grid.resize(grid_dim*grid_dim, base_color);

	for (int i = 0; i < grid_dim*grid_dim; i++){
		if (util::rndFromInterval(1, max) == 1){
			grid[i] = glm::u8vec4(complement_color.x, complement_color.y, complement_color.z, util::rndFromInterval(100, 255));
		}
	}

	for (ushort y = 0; y < dim; y++){
		for (ushort x = 0; x < dim; x++){
			tex_data[y*dim + x] = grid[(y / lattice_dim)*grid_dim + (x / lattice_dim)];
		}
	}
}

ushort rgb888_to_rgb565(uint8 r, uint8 g, uint8 b) {
	const double scale_5 = 31;//glm::pow(2.0, 5.0) - 1.0;
	const double scale_6 = 63;//glm::pow(2.0, 6.0) - 1.0;
	return (uint8((r / 255.0)*scale_5) << 11) | (uint8((g / 255.0)*scale_6) << 5) | (uint8((b / 255.0)*scale_5));
}

glm::ivec2 gen_dxt1_block(uint8 r, uint8 g, uint8 b, uint8 a) {
	ushort rgb565 = rgb888_to_rgb565(r, g, b);
	return glm::ivec2(rgb565 << 16 | rgb565, 0xaaaaaaaa);
}