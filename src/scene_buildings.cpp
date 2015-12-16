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
	_prg_tb_vertices(-1),
	_prg_tb_flags(-1),
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
		"shaders/buildings_v.glsl",
		GL_VERTEX_SHADER),
		0,/*base::create_and_compile_shader(
		  SRC_LOCATION, "shaders/buildings_g.glsl", GL_GEOMETRY_SHADER),*/
		  base::create_and_compile_shader(
		  SRC_LOCATION,
		  "",
		  "shaders/buildings_f.glsl",
		  GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_tb_vertices = get_uniform_location(loc, _prg, "tb_vertices");
	_prg_tb_flags = get_uniform_location(loc, _prg, "tb_flags");
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
	glClearColor(0.3f, 0.3f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glUseProgram(_prg);

	// this is only needed on AMD cards due to driver bug wich needs
	// to have attr0 array anabled
	//base::set_attr0_vbo_amd_wa();

	glUniformMatrix4fv(_prg_mvp, 1, GL_FALSE, glm::value_ptr(ctx->_mvp));

	// bind scene texture buffers
	glUniform1i(_prg_tb_vertices, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _vertices_tb);

	glUniform1i(_prg_tb_flags, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, _flags_tb);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_vbo);
	glDrawElementsInstanced(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, 0, _block_count);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char* scene_buildings::get_test_name(const int i) const{
	return "";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::create_buffers(uint32 blockCount)
{
	GLuint tmp[3];
	glGenBuffers(3, tmp);

	_vertices_vbo = tmp[0];
	_flags_vbo = tmp[1];
	_indices_vbo = tmp[2];

	glGenTextures(2, tmp);
	_vertices_tb = tmp[0];
	_flags_tb = tmp[1];

	// create scene vertices buffer

	glBindBuffer(GL_TEXTURE_BUFFER, _vertices_vbo);

	glBufferData(
		GL_TEXTURE_BUFFER,
		blockCount * 4 * sizeof(glm::vec2),
		0,
		GL_STATIC_DRAW);

	_vertices_data_ptr = reinterpret_cast<glm::vec2*>(
		glMapBufferRange(
		GL_TEXTURE_BUFFER,
		0,
		blockCount * 4 * sizeof(glm::vec2),
		GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));

	glBindTexture(GL_TEXTURE_BUFFER, _vertices_tb);

	glTexBuffer(
		GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_RG32F)->_internal,
		_vertices_vbo);

	// create scene flags buffer

	glBindBuffer(GL_TEXTURE_BUFFER, _flags_vbo);

	glBufferData(
		GL_TEXTURE_BUFFER,
		blockCount * sizeof(uint32),
		0,
		GL_STATIC_DRAW);


	_flags_data_ptr = reinterpret_cast<uint32 *>(
		glMapBufferRange(
		GL_TEXTURE_BUFFER,
		0,
		blockCount * sizeof(uint32),
		GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));

	glBindTexture(GL_TEXTURE_BUFFER, _flags_tb);

	glTexBuffer(
		GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_R32UI)->_internal,
		_flags_vbo);

	// create scene index buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_vbo);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		12 * 3 * sizeof(ushort),
		0,
		GL_STATIC_DRAW);

	_indices_data_ptr = reinterpret_cast<ushort *>(
		glMapBufferRange(
		GL_ELEMENT_ARRAY_BUFFER,
		0,
		12 * 3 * sizeof(ushort),
		GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));

	// cleaning

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::unmap_buffers() {

	// unmap scene vertices

	glBindBuffer(GL_TEXTURE_BUFFER, _vertices_vbo);
	glUnmapBuffer(GL_TEXTURE_BUFFER);

	// unmap scene flags

	glBindBuffer(GL_TEXTURE_BUFFER, _flags_vbo);
	glUnmapBuffer(GL_TEXTURE_BUFFER);

	// unmap scene indices

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_vbo);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// cleaning

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene_buildings::load_data()
{
	// load buildings
	std::ifstream bif("buildingTile46906.osm.bin",std::ios::binary);

	if (!bif.is_open()){
		_app->shutdown();
	}

	std::vector<Building> blds;
	
	uint32 blockCount = 0;

	bif.read((char*)(&blockCount),sizeof(uint32));

	create_buffers(blockCount);

	glm::vec2 *vertices_ptr = _vertices_data_ptr;
	uint32 *flags_ptr = _flags_data_ptr;

	for (int q = 0; q < blockCount; q++) {
		Building b;
		uint8 flags = 0;
		float x = 0.0f;
		float y = 0.0f;

		bif.read((char*)(&flags),sizeof(uint8));

		*flags_ptr++ = 2.9f * 16.0f * (float)flags;

		b.flags = flags;

		for (int v = 0; v < 4; v++) {
			bif.read((char*)(&x),sizeof(float));
			bif.read((char*)(&y), sizeof(float));
			*vertices_ptr++ = glm::vec2(y,x);
			b.pnts[v] = glm::vec2(y, x);
		}

		blds.push_back(b);
	}

	const ushort indices[][3] = { { 0, 1, 4 }, { 1, 5, 4 }, { 1, 2, 5 }, { 2, 6, 5 }, { 2, 3, 6 }, { 3, 7, 6 }, { 3, 0, 7 }, { 0, 4, 7 }, { 4, 5, 6 }, { 6, 7, 4 } };
	static_assert(sizeof(indices) < 12 * 3 * 2, "Error!");
	memcpy(_indices_data_ptr, indices, sizeof(indices));

	_block_count = blockCount;
	unmap_buffers();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=