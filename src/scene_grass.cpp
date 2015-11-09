#include "scene_grass.h"

#include "base/base.h"
#include "base/frame_context.h"

#define GRASS_TILE_W			10.0
#define GRASS_TUFTS_PER_TILE	4096
#define GRASS_BLADES_PER_TUFT	16

scene_grass::scene_grass(base::app * app)
	: _app(app)
	
	, _prg_floor(0)
	, _prg_flr_ctx(-1)
	, _prg_flr_pos(-1)

	, _prg_grass(0)
	, _prg_tg(0)
{

}

scene_grass::~scene_grass()
{

}

void scene_grass::init_gpu_stuff(const base::source_location &loc) 
{
	std::string cfg;

	cfg += "#version 430\n";


	_prg_floor = base::create_program(
		base::create_and_compile_shader(
		SRC_LOCATION,
		cfg,
		"shaders/tile_v.glsl",
		GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(
		SRC_LOCATION,
		cfg,
		"shaders/tile_f.glsl",
		GL_FRAGMENT_SHADER));

	base::link_program(loc,_prg_floor);

	_prg_flr_ctx = glGetUniformBlockIndex(_prg_floor, "context");
	_prg_flr_pos = base::get_uniform_location(loc, _prg_floor, "tile_pos");

	if (base::cfg().use_instancing){
		_prg_grass = base::create_program(
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_inst_v.glsl",
			GL_VERTEX_SHADER),
			0,
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_f.glsl",
			GL_FRAGMENT_SHADER));
	}
	else{
		_prg_grass = base::create_program(
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_v.glsl",
			GL_VERTEX_SHADER),
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_g.glsl",
			GL_GEOMETRY_SHADER),
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_f.glsl",
			GL_FRAGMENT_SHADER));
	}

	base::link_program(loc,_prg_grass);

	_prg_grs_ctx = glGetUniformBlockIndex(_prg_grass, "context");
	_prg_grs_pos = base::get_uniform_location(loc, _prg_grass, "tile_pos");
	
}

void scene_grass::post_gpu_init()
{

}

void scene_grass::update(base::frame_context * const ctx)
{
	_cam_pos = glm::vec3(ctx->_view[3]);
	ctx->_ctx_data_ptr->_mvp = ctx->_mvp;
}

void scene_grass::gpu_draw(base::frame_context * const ctx){
	base::hptimer timer;
	timer.start();

	memset(_grass_tiles, 0, MAX_GRASS_TILES*sizeof(glm::vec2));
	calculate_visible_tiles(16, GRASS_TILE_W);

	glDisable(GL_CULL_FACE);

	glUseProgram(_prg_floor);	

	glBindBufferRange(
		GL_UNIFORM_BUFFER,
		_prg_flr_ctx,
		ctx->_ctx_vbo,
		ctx->_ctx_id * sizeof(base::ctx_data),
		sizeof(base::ctx_data));


	for (int i = 0; i < 16; i++){
		glUniform2f(_prg_flr_pos, _grass_tiles[i].x, _grass_tiles[i].y);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glUseProgram(_prg_grass);

	glBindBufferRange(GL_UNIFORM_BUFFER,
		_prg_grs_ctx,
		ctx->_ctx_vbo,
		ctx->_ctx_id * sizeof(base::ctx_data),
		sizeof(base::ctx_data));

	glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);
	
	for (int i = 0; i < 16; i++){
		glUniform2f(_prg_grs_pos, _grass_tiles[i].x, _grass_tiles[i].y);
		if (base::cfg().use_instancing){
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 9 * GRASS_BLADES_PER_TUFT, GRASS_TUFTS_PER_TILE); // without GS
		}
		else{
			glDrawArraysInstanced(GL_POINTS, 0, GRASS_TUFTS_PER_TILE, GRASS_BLADES_PER_TUFT); // with GS
		}
	}
	
	glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);

	ctx->_cpu_render_time = timer.elapsed_time();
}

const char* scene_grass::get_test_name(const int i) const
{
	return "";
}

void scene_grass::calculate_visible_tiles(int ntiles, float tile_size)
{
	int ntil = 0;

	int side = int(glm::sqrt(ntiles)) >> 1;

	glm::vec2 mid_pos = glm::vec2(glm::floor(_cam_pos.x / tile_size), glm::floor(_cam_pos.z / tile_size));

	for (int z = mid_pos.y - side; z < int(mid_pos.y) + side; z++){
		for (int x = mid_pos.x - side; x < int(mid_pos.x) + side; x++){
			_grass_tiles[ntil] = glm::vec2(float(x), float(z));
			ntil++;
			if (ntil >= ntiles){
				break;
			}
		}

		if (ntil >= ntiles){
			break;
		}
	}
}