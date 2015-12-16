#include "scene_buildings.h"
#include "base/base.h"
#include "base/frame_context.h"
#include "base/app.h"

#include <fstream>
#include <vector>

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
	_prg_mvp(-1)
{
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene_buildings::~scene_buildings() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::init_gpu_stuff(const base::source_location &loc){
	assert(_prg == 0);

	_prg = base::create_program(
		base::create_and_compile_shader(
		SRC_LOCATION,
		"",
		"shaders/buildings_v_n.glsl",
		GL_VERTEX_SHADER),
		0,/*base::create_and_compile_shader(
		  SRC_LOCATION, "shaders/buildings_g.glsl", GL_GEOMETRY_SHADER),*/
		  base::create_and_compile_shader(
		  SRC_LOCATION,
		  "",
		  "shaders/buildings_f.glsl",
		  GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_tb_blocks = get_uniform_location(loc, _prg, "tb_blocks");
	_prg_mvp = get_uniform_location(loc, _prg, "mvp");

	load_data();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::post_gpu_init(){

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::update(base::frame_context * const ctx){

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::gpu_draw(base::frame_context * const ctx){
	glEnable(GL_CULL_FACE);
	glUseProgram(_prg);

	// this is only needed on AMD cards due to driver bug wich needs
	// to have attr0 array anabled
	//base::set_attr0_vbo_amd_wa();

	glUniformMatrix4fv(_prg_mvp, 1, GL_FALSE, glm::value_ptr(ctx->_mvp));

	// bind scene texture buffers
	glUniform1i(_prg_tb_blocks, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _blocks_tb);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_vbo);
	glDrawElementsInstanced(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, 0, _block_count);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char* scene_buildings::get_test_name(const int i) const{
	return "";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::load_data()
{ 
	
	// load buildings
	std::ifstream bif("output.osm.bin",std::ios::binary);

	if (!bif.is_open()){
		_app->shutdown();
	}

	std::vector<Building> blds;
	
	uint32 blockCount = 0;

	bif.read((char*)(&blockCount),sizeof(uint32));

	base::stats()._ntriangles = blockCount * 10;

	std::vector<glm::ivec4> blocks;

	blocks.resize(blockCount);

	bif.read((char*)(blocks.begin()._Ptr), sizeof(glm::ivec4)*blockCount);

	const ushort indices[][3] = { { 0, 1, 4 }, { 1, 5, 4 }, { 1, 2, 5 }, { 2, 6, 5 }, { 2, 3, 6 }, { 3, 7, 6 }, { 3, 0, 7 }, { 0, 4, 7 }, { 4, 5, 6 }, { 6, 7, 4 } };
	static_assert(sizeof(indices) < 12 * 3 * 2, "Error!");
	
	_indices_vbo = base::create_buffer<ushort>(sizeof(indices), nullptr, (void *)(&(indices[0][0])));

	_block_count = blockCount;

	_blocks_vbo = base::create_buffer<glm::int4>(blockCount, nullptr, blocks.begin()._Ptr);

	glGenTextures(1,&_blocks_tb);
	glBindTexture(GL_TEXTURE_BUFFER, _blocks_tb);
	glTexBuffer(GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_R32I)->_internal,
		_blocks_vbo);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=