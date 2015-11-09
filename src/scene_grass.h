#ifndef __ASYNC_VBO_TRANSFERS_SCENE_GRASS_H__
#define __ASYNC_VBO_TRANSFERS_SCENE_GRASS_H__

#include "scene_i.h"

#include <gl/glew.h>

#include "base/types.h"

class scene_grass :
	public scene_i
{
protected:
	GLuint _prg_grass;
	GLuint _prg_floor;
	GLuint _prg_tg;




public:
	scene_grass();
	virtual ~scene_grass();

	virtual void init_gpu_stuff(const base::source_location &loc) override;
	virtual void post_gpu_init() override;
	virtual void update(base::frame_context * const ctx) override;
	virtual void gpu_draw(base::frame_context * const ctx) override;
	virtual const char* get_test_name(const int i) const override;
};

#endif

