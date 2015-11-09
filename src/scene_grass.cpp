#include "scene_grass.h"

#include "base/base.h"

scene_grass::scene_grass()
{

}

scene_grass::~scene_grass()
{

}

void scene_grass::init_gpu_stuff(const base::source_location &loc) 
{
	GLuint tile_v = base::create_and_compile_shader(loc, "", "shaders/tile_v.glsl", GL_VERTEX_SHADER);
	GLuint tile_f = base::create_and_compile_shader(loc, "", "shaders/tile_f.glsl", GL_FRAGMENT_SHADER);
	
	GLuint _prg_floor = base::create_program(tile_v,0,tile_f,0);
	base::link_program(loc,_prg_floor);

	/*GLuint proc_v = base::create_and_compile_shader(loc, "", "shaders/proc_v.glsl", GL_VERTEX_SHADER);
	GLuint proc_g = base::create_and_compile_shader(loc, "", "shaders/proc_g.glsl", GL_GEOMETRY_SHADER);
	GLuint proc_f = base::create_and_compile_shader(loc, "", "shaders/proc_f.glsl", GL_FRAGMENT_SHADER);

	_prg_grass = */
}

void scene_grass::post_gpu_init()
{

}

void scene_grass::update(base::frame_context * const ctx)
{

}

void scene_grass::gpu_draw(base::frame_context * const ctx){

}

const char* scene_grass::get_test_name(const int i) const
{
	return "";
}