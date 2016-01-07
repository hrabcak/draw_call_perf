#include "scene_buildings.h"
#include "base/base.h"
#include "base/frame_context.h"
#include "base/app.h"

#include <fstream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct Building{
	uint8 flags;
	glm::vec2 pnts[4];
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene_buildings::scene_buildings()
{
}

scene_buildings::scene_buildings(base::app * app)
	:_app(app),
	_prg(0),
	_prg_tb_blocks(-1),
	_prg_mvp(-1),
	_prg_total_count(-1),

	_indices_vbo(0),
	_indices_tb(0)
{
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene_buildings::~scene_buildings() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::init_gpu_stuff(const base::source_location &loc){
	assert(_prg == 0);

	char inject_buf[512];

	std::string cfg;

	cfg += "#version 430\n";

	sprintf(&inject_buf[0], "#define BLOCKS_PER_IDC %d\n", base::cfg().blocks_per_idc);
	cfg += inject_buf;

	_prg = base::create_program(
		base::create_and_compile_shader(
		SRC_LOCATION,
		cfg,
		"shaders/buildings_v_n.glsl",
		GL_VERTEX_SHADER),
		0,/*base::create_and_compile_shader(
		  SRC_LOCATION, "shaders/buildings_g.glsl", GL_GEOMETRY_SHADER),*/
		  base::create_and_compile_shader(
		  SRC_LOCATION,
		  cfg,
		  "shaders/buildings_f.glsl",
		  GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_tb_blocks = get_uniform_location(loc, _prg, "tb_blocks");
	_prg_tile_offset = get_uniform_location(loc, _prg, "tile_offset");
	_prg_mvp = get_uniform_location(loc, _prg, "mvp");
	_prg_total_count = get_uniform_location(loc, _prg, "total_count");

	load_tile(glm::ivec2(0, 0));
	/*load_tile(glm::ivec2(0, 1));
	load_tile(glm::ivec2(0, 2));
	load_tile(glm::ivec2(0, 3));
	load_tile(glm::ivec2(0, 4));
	load_tile(glm::ivec2(0, 5));
	load_tile(glm::ivec2(1, 0));
	load_tile(glm::ivec2(1, 1));
	load_tile(glm::ivec2(1, 2));
	load_tile(glm::ivec2(1, 3));
	load_tile(glm::ivec2(1, 4));
	load_tile(glm::ivec2(1, 5));
	load_tile(glm::ivec2(2, 0));
	load_tile(glm::ivec2(2, 1));
	load_tile(glm::ivec2(2, 2));
	load_tile(glm::ivec2(2, 3));
	load_tile(glm::ivec2(2, 4));
	load_tile(glm::ivec2(2, 5));
	load_tile(glm::ivec2(3, 0));
	load_tile(glm::ivec2(3, 1));
	load_tile(glm::ivec2(3, 2));
	load_tile(glm::ivec2(3, 3));
	load_tile(glm::ivec2(3, 4));
	load_tile(glm::ivec2(3, 5));
	load_tile(glm::ivec2(4, 0));
	load_tile(glm::ivec2(4, 1));
	load_tile(glm::ivec2(4, 2));
	load_tile(glm::ivec2(4, 3));
	load_tile(glm::ivec2(4, 4));
	load_tile(glm::ivec2(4, 5));
	load_tile(glm::ivec2(5, 0));
	load_tile(glm::ivec2(5, 1));
	load_tile(glm::ivec2(5, 2));
	load_tile(glm::ivec2(5, 3));
	load_tile(glm::ivec2(5, 4));
	load_tile(glm::ivec2(5, 5));*/
	//base::stats()._ntriangles = 25 * _tiles[0]._blocks_count * 10;

	const uint32 indices_base[][3] = { { 0, 1, 4 }, { 1, 5, 4 }, { 1, 2, 5 }, { 2, 6, 5 }, { 2, 3, 6 }, { 3, 7, 6 }, { 3, 0, 7 }, { 0, 4, 7 }, { 4, 5, 6 }, { 6, 7, 4 } };

	std::vector<uint32> indices;
	indices.resize(10 * 3 * base::cfg().blocks_per_idc);

	for (int b = 0; b < base::cfg().blocks_per_idc; b++){
		int offset = b * 8;
		for (int i = 0; i < 30; i++){
			indices[b * 30 + i] = (&indices_base[0][0])[i] + offset;
		}
	}

	_indices_vbo = base::create_buffer<uint32>(30 * base::cfg().blocks_per_idc, 0, indices.begin()._Ptr);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::post_gpu_init(){

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::update(base::frame_context * const ctx){

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::gpu_draw(base::frame_context * const ctx){
	base::hptimer timer;
	timer.start();
	if (base::cfg().no_cull){
		glDisable(GL_CULL_FACE);
	}
	
	glUseProgram(_prg);

	// this is only needed on AMD cards due to driver bug wich needs
	// to have attr0 array anabled
	//base::set_attr0_vbo_amd_wa();

	
	glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

	glUniformMatrix4fv(_prg_mvp, 1, GL_FALSE, glm::value_ptr(ctx->_mvp));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_vbo);
	glUniform1i(_prg_tb_blocks, 0);
	glActiveTexture(GL_TEXTURE0);

	// bind scene texture buffers

	

	for (uint32 i = 0; i < _tiles.size(); i++){
		int x = _tiles[i]._tile_pos.x;
		int y = _tiles[i]._tile_pos.y;
		glUniform2i(_prg_tile_offset, x,y);
		glUniform1ui(_prg_total_count, _tiles[i]._blocks_count);
		glBindTexture(GL_TEXTURE_BUFFER, _tiles[i]._blocks_tb);
		glDrawElementsInstanced(GL_TRIANGLES, 
						10 * 3 * base::cfg().blocks_per_idc, 
						GL_UNSIGNED_INT, 
						0, 
						(_tiles[i]._blocks_count + base::cfg().blocks_per_idc - 1) / base::cfg().blocks_per_idc);
	}

	ctx->_cpu_render_time = timer.elapsed_time();

	glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char* scene_buildings::get_test_name(const int i) const{
	return "";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::load_tile(glm::ivec2 pos)
{ 
	BuildingTileDCData building;
	building._tile_pos = pos;

	std::string file_name = "bld_tiles/buildingTile";
	file_name += '0' + pos.x;
	file_name += '0' + pos.y;
	file_name += ".osm.bin";

	// load buildings
	std::ifstream bif(file_name.c_str(),std::ios::binary);

	if (!bif.is_open()){
		_app->shutdown();
		return;
	}

	std::vector<Building> blds;
	
	uint32 blockCount = 0;

	bif.read((char*)(&blockCount),sizeof(uint32));

	base::stats()._ntriangles += uint64(blockCount) * 10;
	base::stats()._nvertices += uint64(blockCount) * 8;
	base::stats()._ndrawcalls++;

	std::vector<glm::ivec4> blocks;

	blocks.resize(blockCount);

	bif.read((char*)(blocks.begin()._Ptr), sizeof(glm::ivec4)*blockCount);

	building._blocks_count = blockCount;

	building._blocks_vbo = base::create_buffer<glm::int4>(blockCount, 0, blocks.begin()._Ptr);

	glGenTextures(1, &building._blocks_tb);
	glBindTexture(GL_TEXTURE_BUFFER, building._blocks_tb);
	glTexBuffer(GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_RGBA32I)->_internal,
		building._blocks_vbo);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	_tiles.push_back(building);

	base::cfg().buildings_count += blockCount;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=