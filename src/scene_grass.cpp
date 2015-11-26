#include "scene_grass.h"

#include "base/base.h"
#include "base/frame_context.h"

#define NIDX_PER_BLADE_TSTRIP 8
#define NIDX_PER_BLADE_T 15


scene_grass::scene_grass(base::app * app)
	: _app(app)

	, _prg_floor(0)
	, _prg_flr_ctx(-1)
	, _prg_flr_pos(-1)
	, _prg_grs_hmap(-1)

	, _prg_grass(0)

	, _prg_tg(0)
	, _prg_tg_tex(-1)
	, _prg_tg_pos(-1)

	, _grass_tex(0)

	, _idx_buf_i(nullptr)
	, _idx_buf_us(nullptr)
	, _idx_buf_h(-1)
{
	base::config & cfg = base::cfg();

	cfg.blades_per_tuft = 16; // pocet listov na jeden interny draw call
	cfg.tufts_per_tile = 4096;	// pocet internych drawcall-ov (prerozdenelenie velkeho tile-u na 64*64 blokov)
	cfg.ngrass_tiles = 16;	// pocet drawcall-ov

	base::stats_data & stats = base::stats();

	stats._nvertices = base::cfg().ngrass_tiles * base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * 7;
	stats._ntriangles = base::cfg().ngrass_tiles * base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * 5;

	_grs_data._blades_per_tuft = base::cfg().blades_per_tuft;
	_grs_data._blocks_per_row = glm::sqrt(base::cfg().tufts_per_tile);
	_grs_data._tile_width = 10.0f;

	if (base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader) {
		if (base::cfg().dc_per_tile == 1){
			stats._ndrawcalls = 1;
			base::cfg().in_vtx_per_dc = 9 * _grs_data._blades_per_tuft
				* _grs_data._blocks_per_row*_grs_data._blocks_per_row;
		}
		else{
			stats._ndrawcalls = base::cfg().dc_per_tile;
			base::cfg().in_vtx_per_dc = (9 * _grs_data._blades_per_tuft
				* _grs_data._blocks_per_row*_grs_data._blocks_per_row) / base::cfg().dc_per_tile;
		}

		if (base::cfg().use_idx_buf) {
			if (base::cfg().use_triangles){
				
				if (base::cfg().vs_variable_blades_per_dc){
					int max_idx = NIDX_PER_BLADE_T* base::cfg().blades_per_dc;

					_idx_buf_i = new int[max_idx];
					
					int curr_buf_pos = 0;

					int curr_v_id = 0;

					for (int b_id = 0; b_id < base::cfg().blades_per_dc; b_id++){
						for (int t_id = 0; t_id < 5; t_id++){
							_idx_buf_i[curr_buf_pos] = curr_v_id + t_id;

							if ((t_id & 1)){
								_idx_buf_i[curr_buf_pos + 1] = (curr_v_id + (t_id + 2));
								_idx_buf_i[curr_buf_pos + 2] = (curr_v_id + (t_id + 1));
							}
							else{
								_idx_buf_i[curr_buf_pos + 1] = (curr_v_id + (t_id + 1));
								_idx_buf_i[curr_buf_pos + 2] = (curr_v_id + (t_id + 2));
							}

							curr_buf_pos += 3;
						}

						curr_v_id += 7;
					}
				}
				else{
					int subtile_w = _grs_data._blocks_per_row / glm::sqrt<int>(base::cfg().dc_per_tile);
					int max_idx = (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_T) / base::cfg().dc_per_tile;

					_idx_buf_i = new int[max_idx];

					int curr_buf_pos = 0;

					for (int y = 0; y < subtile_w; y++){
						for (int x = 0; x < subtile_w; x++){
							for (int b_id = 0; b_id < base::cfg().blades_per_tuft; b_id++){
								for (int t_id = 0; t_id < 5; t_id++){
									int idx_base = (x << 14) | (y << 8) | (b_id << 4);
									_idx_buf_i[curr_buf_pos] = (idx_base | t_id);

									if ((t_id & 1)){
										_idx_buf_i[curr_buf_pos + 1] = (idx_base | (t_id + 2));
										_idx_buf_i[curr_buf_pos + 2] = (idx_base | (t_id + 1));
									}
									else{
										_idx_buf_i[curr_buf_pos + 1] = (idx_base | (t_id + 1));
										_idx_buf_i[curr_buf_pos + 2] = (idx_base | (t_id + 2));
									}

									curr_buf_pos += 3;
								}
							}
						}
					}
				}

			}
			else {
				if (base::cfg().vs_variable_blades_per_dc){
					int max_idx = (base::cfg().blades_per_dc * NIDX_PER_BLADE_TSTRIP);
					_idx_buf_i = new int[max_idx];

					for (int i = 0, curr_idx = 0; i < max_idx; i++){
						_idx_buf_i[i] = ((i & 7) == 7) ? 0xffffffff : curr_idx++;
					}

				}
				else{
					int max_idx = (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_TSTRIP) / base::cfg().dc_per_tile;
					_idx_buf_i = new int[max_idx];

					for (int i = 0, curr_idx = 0; i < max_idx; i++){
						_idx_buf_i[i] = ((i & 7) == 7) ? 0xffffffff : curr_idx++;
					}
				}
			}
		}

	}
	else if (base::cfg().proc_scene_type == base::proc_scn_type::psGeometryShader) {
		if (!base::cfg().use_end_primitive) {
			if (base::cfg().dc_per_tile == 1) {
				stats._ndrawcalls = 1;
				base::cfg().in_vtx_per_dc = _grs_data._blocks_per_row*_grs_data._blocks_per_row * _grs_data._blades_per_tuft;
			}
			else {
				stats._ndrawcalls = base::cfg().dc_per_tile;
				base::cfg().in_vtx_per_dc = ((_grs_data._blocks_per_row*_grs_data._blocks_per_row) / base::cfg().dc_per_tile) * _grs_data._blades_per_tuft;
			}
		}
		else {
			if (base::cfg().dc_per_tile == 1) {
				stats._ndrawcalls = 1;
				base::cfg().in_vtx_per_dc = (base::cfg().blades_per_tuft / base::cfg().blades_per_geom_run) * _grs_data._blocks_per_row*_grs_data._blocks_per_row;
			}
			else {
				stats._ndrawcalls = (base::cfg().blades_per_tuft / base::cfg().blades_per_geom_run);
				base::cfg().in_vtx_per_dc = (_grs_data._blocks_per_row*_grs_data._blocks_per_row);
			}
		}
	}

	stats._ndrawcalls *= base::cfg().ngrass_tiles;
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

	if (base::cfg().use_end_primitive){
		cfg += "#define USE_END_PRIMITIVE\n";

		sprintf(&inject_buf[0], "#define BLADES_PER_GEOM_RUN %d\n", base::cfg().blades_per_geom_run);
		cfg += inject_buf;

		sprintf(&inject_buf[0], "#define VERTS_PER_GEOM_RUN %d\n", 7*base::cfg().blades_per_geom_run);
		cfg += inject_buf;
	}

	if (base::cfg().use_idx_buf){
		cfg += "#define USE_IDX_BUF\n";
		if (base::cfg().use_triangles){
			cfg += "#define USE_TRIANGLES\n";
		}
	}

	if (base::cfg().use_grass_blade_tex) {
		cfg += "#define USE_TEXTURE\n";
	}

	if (base::cfg().pure_color){
		cfg += "#define PURE_COLOR\n";
	}

	if (base::cfg().ip_count != 0){
		sprintf(inject_buf, "#define IP_%dF\n", base::cfg().ip_count);
		cfg += inject_buf;
	}

	if (base::cfg().vs_variable_blades_per_dc){
		cfg += "#define VARIABLE_BLADES_PER_INSTANCE\n";
		sprintf(&inject_buf[0],"#define BLADES_PER_INSTANCE %d\n", base::cfg().blades_per_dc);
		cfg += inject_buf;
	}

	sprintf(&inject_buf[0], "#define VERT_PER_BLADE %d\n", 7);

	cfg += inject_buf;

	if (base::cfg().dc_per_tile == 1) {
		cfg += "#define ONE_BATCH\n";
	}
	else{
		
		sprintf(&inject_buf[0]
			, "#define VERTS_PER_DC %d\n#define DC_COUNT %d\n"
			, (9 * _grs_data._blades_per_tuft
			* _grs_data._blocks_per_row*_grs_data._blocks_per_row) / base::cfg().dc_per_tile
			, base::cfg().dc_per_tile);

		cfg += inject_buf;
	}



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

	base::link_program(loc, _prg_floor);

	_prg_flr_ctx = glGetUniformBlockIndex(_prg_floor, "context");
	_prg_flr_pos = base::get_uniform_location(loc, _prg_floor, "tile_pos");

	if (base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader){
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
	else if (base::cfg().proc_scene_type == base::proc_scn_type::psGeometryShader){
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
	else if (base::cfg().proc_scene_type == base::proc_scn_type::psTessShader){
		_prg_grass = base::create_program(base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_tess_v.glsl",
			GL_VERTEX_SHADER),
			0,
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_f.glsl",
			GL_FRAGMENT_SHADER),
			0,
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_tess_tcs.glsl",
			GL_TESS_CONTROL_SHADER), 
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_tess_tes.glsl",
			GL_TESS_EVALUATION_SHADER));
	}

	base::link_program(loc, _prg_grass);

	_prg_grs_ctx = glGetUniformBlockIndex(_prg_grass, "context");
	_prg_grs_pos = base::get_uniform_location(loc, _prg_grass, "tile_pos");
	_prg_grs_hmap = base::get_uniform_location(loc, _prg_grass, "height_map");

	if (base::cfg().use_grass_blade_tex) {
		_prg_grs_tex = base::get_uniform_location(loc, _prg_grass, "grass_tex");
	}
	_grass_tex = base::create_texture_from_file(loc, "tex/grass_blade_d.tga", false);

	_prg_tg = base::create_program(
		0, 0, 0,
		base::create_and_compile_shader(
		SRC_LOCATION,
		cfg,
		"shaders/heightgen_c.glsl",
		GL_COMPUTE_SHADER));
	base::link_program(loc, _prg_tg);

	_prg_tg_tex = get_uniform_location(loc, _prg_tg, "dst");
	_prg_tg_pos = get_uniform_location(loc, _prg_tg, "pos");

	calculate_visible_tiles(base::cfg().ngrass_tiles, _grs_data._tile_width);
	create_height_texs();

	if (base::cfg().use_idx_buf && base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader && !base::cfg().use_triangles){
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(0xffffffff);
	}

	int idx_buf_size = -1;
	
	if (base::cfg().use_triangles){
		if (base::cfg().vs_variable_blades_per_dc){
 			idx_buf_size = (base::cfg().blades_per_dc * NIDX_PER_BLADE_T);
		}
		else{
			idx_buf_size = (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_T) / base::cfg().dc_per_tile;
		}
	}
	else{
		if (base::cfg().vs_variable_blades_per_dc){
			idx_buf_size = base::cfg().blades_per_dc * NIDX_PER_BLADE_TSTRIP;
		}
		else{
			idx_buf_size = (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_TSTRIP) / base::cfg().dc_per_tile;
		}
	}

	_idx_buf_h = base::create_buffer<int>(
		idx_buf_size,
		nullptr,
		_idx_buf_i);
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
	//glPolygonMode(GL_FRONT, GL_LINE);
	
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

	if (base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader &&
		base::cfg().use_idx_buf) {
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idx_buf_h);
	}

	glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

	for (int i = 0; i < base::cfg().ngrass_tiles; i++){
		glUniform2f(_prg_grs_pos, _grass_tiles[i].x, _grass_tiles[i].y);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _height_tex[i]);
		glUniform1i(_prg_grs_hmap, 1);

		if (base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader) {
			if (!base::cfg().use_idx_buf) {
				if (base::cfg().vs_variable_blades_per_dc){
					glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0,
						9 * base::cfg().blades_per_dc,
						_grs_data._blocks_per_row*_grs_data._blocks_per_row*(_grs_data._blades_per_tuft / base::cfg().blades_per_dc)
						);
				}
				else if (base::cfg().dc_per_tile == 1){
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 9 * _grs_data._blades_per_tuft
						* _grs_data._blocks_per_row*_grs_data._blocks_per_row);
				}
				else{
					glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0
						, (9 * _grs_data._blades_per_tuft
						* _grs_data._blocks_per_row*_grs_data._blocks_per_row) / base::cfg().dc_per_tile
						, base::cfg().dc_per_tile); // without GS
				}
			}
			else{
				if (base::cfg().vs_variable_blades_per_dc){
					if (base::cfg().use_triangles){
						glDrawElementsInstanced(
							GL_TRIANGLES,
							NIDX_PER_BLADE_T * base::cfg().blades_per_dc,
							GL_UNSIGNED_INT,
							0,
							_grs_data._blocks_per_row*_grs_data._blocks_per_row*(_grs_data._blades_per_tuft / base::cfg().blades_per_dc));
					}
					else{
						glDrawElementsInstanced(
							GL_TRIANGLE_STRIP,
							NIDX_PER_BLADE_TSTRIP * base::cfg().blades_per_dc,
							GL_UNSIGNED_INT,
							0,
							_grs_data._blocks_per_row*_grs_data._blocks_per_row*(_grs_data._blades_per_tuft / base::cfg().blades_per_dc));
					}
				}
				if (base::cfg().dc_per_tile == 1){
					if (base::cfg().use_triangles){
						glDrawElements(GL_TRIANGLES
							, (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_T)
							, GL_UNSIGNED_INT,
							0);
					}
					else{
						glDrawElements(GL_TRIANGLE_STRIP
							, (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_TSTRIP)
							, GL_UNSIGNED_INT,
							0);
					}
					
				}
				else{
					if (base::cfg().use_triangles){
						glDrawElementsInstanced(GL_TRIANGLES
							, (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_T) / base::cfg().dc_per_tile
							, GL_UNSIGNED_INT
							, 0
							, base::cfg().dc_per_tile);
					}
					else{
						glDrawElementsInstanced(GL_TRIANGLE_STRIP
							, (base::cfg().tufts_per_tile * base::cfg().blades_per_tuft * NIDX_PER_BLADE_TSTRIP) / base::cfg().dc_per_tile
							, GL_UNSIGNED_INT
							, 0
							, base::cfg().dc_per_tile);
					}
				}

			}
		}
		else if (base::cfg().proc_scene_type == base::proc_scn_type::psGeometryShader) {
			if (!base::cfg().use_end_primitive) {
				if (base::cfg().dc_per_tile == 1){
					glDrawArrays(GL_POINTS, 0
						, _grs_data._blocks_per_row*_grs_data._blocks_per_row * _grs_data._blades_per_tuft); // with GS
				}
				else{
					glDrawArraysInstanced(GL_POINTS, 0
						, ((_grs_data._blocks_per_row*_grs_data._blocks_per_row) / base::cfg().dc_per_tile) * _grs_data._blades_per_tuft
						, base::cfg().dc_per_tile);
				}
			}
			else{
				if (base::cfg().dc_per_tile == 1) {
					glDrawArrays(GL_POINTS, 0
						, (base::cfg().blades_per_tuft / base::cfg().blades_per_geom_run) * _grs_data._blocks_per_row*_grs_data._blocks_per_row);
				} else {
					glDrawArraysInstanced(GL_POINTS, 0
						, (_grs_data._blocks_per_row*_grs_data._blocks_per_row)
						, (base::cfg().blades_per_tuft / base::cfg().blades_per_geom_run));
				}
			}	
		}
		else if (base::cfg().proc_scene_type == base::proc_scn_type::psTessShader) {
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawArraysInstanced(GL_PATCHES, 0, 4 * _grs_data._blades_per_tuft, (_grs_data._blocks_per_row*_grs_data._blocks_per_row));
		}
	}

	glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);

	ctx->_cpu_render_time = timer.elapsed_time();
}

const char* scene_grass::get_test_name(const int i) const
{
	if ( base::cfg().proc_scene_type == base::proc_scn_type::psVertexShader ) {
		return "VS";
	}
	else if ( base::cfg().proc_scene_type == base::proc_scn_type::psGeometryShader ){
		return "GS";
	}
	else{
		return "! ! ! UNDEFINED_TEST ! ! !";
	}
	
}

void scene_grass::calculate_visible_tiles(int ntiles, float tile_size)
{
	int ntil = 0;

	int side = int(glm::sqrt(ntiles));
	side = glm::powerOfTwoAbove(side);
	side = side >> 1;

	glm::vec2 mid_pos = glm::vec2(glm::floor(_cam_pos.x / tile_size), glm::floor(_cam_pos.z / tile_size));

	for (int z = int(mid_pos.y - side); z < int(mid_pos.y) + side; z++){
		for (int x = int(mid_pos.x - side); x < int(mid_pos.x) + side; x++){
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

void scene_grass::create_height_texs(){
	const int width = _grs_data._blocks_per_row;
	const int tex_size = width * width;
	const int ntex = MAX_GRASS_TILES;
	const base::pixelfmt pf = base::PF_R32UI;
	const int tex_size_bytes = tex_size * base::get_pfd(pf)->_size;

	glUseProgram(_prg_tg);
	glUniform1i(_prg_tg_tex, 0);

	for (int i = 0; i < ntex; i++){
		GLuint tex;
		glGenTextures(1, &tex);

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage2D(
			tex,
			1,
			base::get_pfd(pf)->_internal,
			width,
			width);

		glBindTexture(GL_TEXTURE_2D, 0);

		_height_tex[i] = tex;

		glBindImageTexture(0, tex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32UI);
		glUniform2i(_prg_tg_pos, int(_grass_tiles[i].x), int(_grass_tiles[i].y));

		glDispatchCompute(width >> 4, width >> 4, 1);
	}

	glUseProgram(0);

}