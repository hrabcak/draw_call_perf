#ifndef __SCENE_BUILDINGS_H__
#define __SCENE_BUILDINGS_H__

#include "base/types.h"
#include "scene_i.h"

#include <gl/glew.h>

namespace base {
	struct frame_context;
	class source_location;
	class app;
}

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

	void load_data();

private:
	unsigned _blocks_vbo;
	unsigned _blocks_tb;
	glm::ivec4* _blocks_data_ptr;

	unsigned _indices_vbo;
	unsigned _indices_tb;
	ushort* _indices_data_ptr;

	uint32 _block_count;

	void create_buffers(uint32 blockCount);
	void unmap_buffers();

protected:
	base::app * _app;

	GLuint _prg;
	GLint _prg_tb_blocks;
	GLint _prg_mvp;
};

#endif // __SCENE_BUILDINGS_H__
