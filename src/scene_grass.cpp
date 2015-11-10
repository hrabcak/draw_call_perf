#include "scene_grass.h"

#include "base/base.h"
#include "base/frame_context.h"

scene_grass::scene_grass(base::app * app)
	: _app(app)
	
	, _prg_floor(0)
	, _prg_flr_ctx(-1)
	, _prg_flr_pos(-1)

	, _prg_grass(0)
	, _prg_tg(0)

	, _grass_tex(0)
{
	base::stats_data & stats = base::stats();
	stats._ndrawcalls = base::cfg().ngrass_tiles;
	stats._nvertices = base::cfg().ngrass_tiles * base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * base::cfg().vert_per_blade;
	stats._ntriangles = base::cfg().ngrass_tiles * base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * (base::cfg().vert_per_blade - 2);

	_grs_data._blades_per_tuft = base::cfg().blades_per_tuft;
	_grs_data._blocks_per_row = glm::sqrt(base::cfg().tufts_per_tile);
	_grs_data._tile_width = 10.0f;
}

scene_grass::~scene_grass()
{

}

void scene_grass::init_gpu_stuff(const base::source_location &loc) 
{
	char inject_buf[512];

	std::string cfg;

	cfg += "#version 430\n";

	sprintf(&inject_buf[0], "#define BLADESPERTUFT %d \n", _grs_data._blades_per_tuft);

	cfg += inject_buf;

	sprintf(&inject_buf[0], "#define BLOCKSPERROW %d \n", _grs_data._blocks_per_row);

	cfg += inject_buf;

	sprintf(&inject_buf[0], "#define TILEWIDTH %f \n", _grs_data._tile_width);

	cfg += inject_buf;

	if (base::cfg().use_grass_blade_tex) {
		cfg += "#define USE_TEXTURE\n";
	}

	//cfg += "#define WITHOUT_BENDING\n"; // ohybat listy

	sprintf(&inject_buf[0], "#define VERT_PER_BLADE %d\n", base::cfg().vert_per_blade);

	cfg += inject_buf;



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
	
	if (base::cfg().use_grass_blade_tex) {
		_prg_grs_tex = base::get_uniform_location(loc, _prg_grass, "grass_tex");
	}
	_grass_tex = base::create_texture_from_file(loc, "tex/grass_blade_d.tga", false);

	//_grs_data_vbo = 
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

	memset(_grass_tiles, 0, MAX_GRASS_TILES*sizeof(glm::vec2));
	calculate_visible_tiles(base::cfg().ngrass_tiles, _grs_data._tile_width);

	glDisable(GL_CULL_FACE);

	glUseProgram(_prg_floor);	

	

	glBindBufferRange(
		GL_UNIFORM_BUFFER,
		_prg_flr_ctx,
		ctx->_ctx_vbo,
		ctx->_ctx_id * sizeof(base::ctx_data),
		sizeof(base::ctx_data));


	for (int i = 0; i < base::cfg().ngrass_tiles; i++){
		glUniform2f(_prg_flr_pos, _grass_tiles[i].x, _grass_tiles[i].y);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glUseProgram(_prg_grass);

	timer.start();

	if (base::cfg().use_grass_blade_tex) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _grass_tex);
		glUniform1i(_prg_grs_tex, 0);
	}
	
	glBindBufferRange(GL_UNIFORM_BUFFER,
		_prg_grs_ctx,
		ctx->_ctx_vbo,
		ctx->_ctx_id * sizeof(base::ctx_data),
		sizeof(base::ctx_data));

	glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);
	
	for (int i = 0; i < base::cfg().ngrass_tiles; i++){
		glUniform2f(_prg_grs_pos, _grass_tiles[i].x, _grass_tiles[i].y);
		if (base::cfg().use_instancing){
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, (base::cfg().vert_per_blade+2) * _grs_data._blades_per_tuft, _grs_data._blocks_per_row*_grs_data._blocks_per_row); // without GS
		}
		else{
			glDrawArraysInstanced(GL_POINTS, 0, _grs_data._blocks_per_row*_grs_data._blocks_per_row, _grs_data._blades_per_tuft); // with GS
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