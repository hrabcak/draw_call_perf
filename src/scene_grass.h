#ifndef __ASYNC_VBO_TRANSFERS_SCENE_GRASS_H__
#define __ASYNC_VBO_TRANSFERS_SCENE_GRASS_H__

#include "scene_i.h"

#include <gl/glew.h>

#include "base/types.h"

#define MAX_GRASS_TILES 1024

namespace base{
	class app;
}

struct grass_data{
	int _blades_per_tuft;
	int _blocks_per_row;
	float _tile_width;
};

class scene_grass :
	public scene_i
{
protected:
	base::app * _app;

	GLuint _prg_grass;
	GLint _prg_grs_ctx;
	GLint _prg_grs_pos;
	GLint _prg_grs_tex;
	GLint _prg_grs_data;
	GLint _prg_grs_hmap;
	
	GLuint _prg_floor;
	GLint _prg_flr_ctx;
	GLint _prg_flr_pos;

	GLuint _prg_tg;
	GLint _prg_tg_tex;
	GLint _prg_tg_pos;

	GLuint _grass_tex;

	grass_data _grs_data;

	glm::vec2 _grass_tiles[MAX_GRASS_TILES];
	GLuint _height_tex[MAX_GRASS_TILES];

	glm::vec3 _cam_pos;
public:
	scene_grass(base::app * app);
	virtual ~scene_grass();

	virtual void init_gpu_stuff(const base::source_location &loc) override;
	virtual void post_gpu_init() override;
	virtual void update(base::frame_context * const ctx) override;
	virtual void gpu_draw(base::frame_context * const ctx) override;
	virtual const char* get_test_name(const int i) const override;

protected:
	void calculate_visible_tiles(int ntiles, float tile_size);
	void create_height_texs();
};

#endif

