#ifndef __SCENE_BUILDINGS_H__
#define __SCENE_BUILDINGS_H__

#include "base/types.h"
#include "scene_i.h"

#include <gl/glew.h>

#include <vector>

namespace base {
	struct frame_context;
	class source_location;
	class app;
}

struct BuildingTileDCData{
	glm::ivec2 _tile_pos;
	GLuint _blocks_vbo;
	GLuint _blocks_tb;
	uint32 _blocks_count;
};

class scene_buildings: public scene_i
{
public:
	scene_buildings();
	scene_buildings(base::app * app);
	~scene_buildings();

	virtual void init_gpu_stuff(const base::source_location &loc) override;
	virtual void post_gpu_init() override;
	virtual void update(base::frame_context * const ctx) override;
	virtual void gpu_draw(base::frame_context * const ctx) override;
	virtual const char* get_test_name(const int i) const override;

	void load_tile(glm::ivec2 pos);

private:
	std::vector<BuildingTileDCData> _tiles;
	unsigned _indices_vbo;
	unsigned _indices_tb;
protected:
	base::app * _app;

	GLuint _prg;
	GLint _prg_tb_blocks;
	GLint _prg_tile_offset;
	GLint _prg_mvp;
};

#endif // __SCENE_BUILDINGS_H__
