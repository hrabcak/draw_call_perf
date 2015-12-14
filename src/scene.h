/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __ASYNC_VBO_TRANSFERS_SCENE_H__
#define __ASYNC_VBO_TRANSFERS_SCENE_H__

#include <vector>

#include <gl/glew.h>

#include "base/types.h"

#include "scene_i.h"

namespace base {
	struct frame_context;
	class source_location;
}

class benchmark;

class scene
    : public scene_i
{
public:
	static const int * get_vtx_tbl() { 
		static const int vtx_tbl[] = { 24, 32, 40, 42, 48, 56, 54, 
										64, 62, 72, 66, 72, 76, 82, 
										80, 92, 90, 94, 100, 102, 104,
										112, 114, 112, 118, 120, 122, 126,
										132, 130, 136, 142, 144, 152, 154,
										158, 160, 162, 166, 172, 170, 180,
										182, 184, 192, 190, 192, 202, 208,
										210, 214, 216, 224, 232, 236, 238,
										240, 256, 262, 264, 266, 288, 292,
										294, 320, 322, 352, 384 };

		return &vtx_tbl[0]; 
	}
	static const int * get_ele_tbl() { 
		static const int ele_tbl[] = { 36, 60, 84, 96, 108, 132, 144, 
										156, 168, 180, 192, 204, 228, 240,
										252, 276, 288, 312, 324, 336, 348,
										372, 384, 396, 408, 420, 432, 456,
										468, 480, 492, 528, 540, 564, 576,
										600, 612, 624, 648, 660, 672, 708,
										720, 732, 756, 768, 780, 816, 852,
										864, 888, 900, 924, 972, 996, 1008,
										1020, 1092, 1128, 1140, 1152, 1260, 1284,
										1296, 1428, 1440, 1596, 1764 };
		return &ele_tbl[0]; 
	}
	static const int * get_sz_tbl(){
		static const int sz_tbl[] = { 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 1, 0, 0, 3,
			0, 0, 4, 1, 1, 1, 0, 0, 5, 0, 1, 3, 0, 0, 6,
			1, 1, 2, 0, 1, 4, 0, 2, 3, 0, 1, 5, 1, 2, 2,
			0, 1, 6, 0, 3, 3, 1, 2, 3, 0, 2, 5, 1, 1, 5,
			0, 3, 4, 0, 2, 6, 1, 1, 6, 2, 2, 3, 0, 3, 5,
			0, 4, 4, 1, 2, 5, 1, 3, 4, 0, 3, 6, 2, 3, 3,
			0, 4, 5, 1, 3, 5, 1, 4, 4, 0, 4, 6, 0, 5, 5,
			1, 3, 6, 2, 2, 6, 1, 4, 5, 2, 3, 5, 0, 5, 6,
			3, 3, 4, 1, 4, 6, 1, 5, 5, 2, 3, 6, 0, 6, 6,
			3, 3, 5, 3, 4, 4, 1, 5, 6, 2, 4, 6, 2, 5, 5,
			3, 4, 5, 4, 4, 4, 1, 6, 6, 2, 5, 6, 3, 4, 6,
			3, 5, 5, 4, 4, 5, 2, 6, 6, 3, 5, 6, 4, 4, 6,
			4, 5, 5, 3, 6, 6, 4, 5, 6, 5, 5, 5, 4, 6, 6,
			5, 5, 6, 5, 6, 6, 6, 6, 6 };

		return &sz_tbl[0];
	}

	static const int MAX_BLOCK_COUNT = 0x10000;
	std::vector<std::string> _test_names;

	scene(benchmark * const app);
	~scene();

    void init_gpu_stuff(const base::source_location &loc) override;
    void post_gpu_init() override;
    void update(base::frame_context * const ctx) override;
    void gpu_draw(base::frame_context * const ctx) override;

    const char* get_test_name(const int i) const override { return _test_names[i].c_str(); }

public:

	void add_test_block(bool add_peaks);

protected:

    // heading is in degrees
    void add_block(
        const glm::vec3 &pos,
        const glm::vec3 &size,
        const float heading);

    void create_textures(const base::source_location &loc);
    void create_test_scene();
	
	int get_perspective_block_bound(int row, float scale);
	
    int frustum_check(base::frame_context *fc);
    void create_frustum_planes(glm::vec4 *planes, const glm::mat4 &mvp);
    void load_and_init_shaders(const base::source_location &loc);
    void upload_blocks_to_gpu(
        const base::source_location &loc,
        base::frame_context *ctx);
    void bind_texture(int counter);

protected:
	glm::ivec2 _cur_next_block;
	uint32 _cur_block;
	
	static const int NumTypes = 4;

	enum Flags {
		TypeMask	= NumTypes-1,
		Visible		= 0x04,
	};

	std::vector<glm::mat4> _tms;			//< transformation matrixes for blocks
	std::vector<glm::mat4> _bboxes;			//< inverse transposed TM matrices
	std::vector<glm::vec3> _hws;			//< half width for every block
	std::vector<unsigned int> _flags;		//<

    GLuint _prg;
    GLint _prg_tb_blocks;
    GLint _prg_ctx;
    GLint _prg_tb_pos;
	GLint _prg_tb_nor_uv;
    GLint _prg_tex;

    GLuint _prg_tg;
    GLint _prg_tg_tex;

    GLuint _prg_mip;
    GLuint _prg_mip2;

    GLuint _tb_pos;
    GLuint _tb_nor_uv;
    GLuint _tb_tex_handles;

    GLuint _buffer_elem;
    GLuint _buffer_pos;
    GLuint _buffer_nor_uv;
    GLuint _buffer_tex_handles;

    std::vector<GLuint> _texs;
    std::vector<GLuint64> _tex_handles;

    GLint _max_array_layers;

    glm::int2 * _vertices_base_ptr;
    glm::int2 * _norm_uv_base_ptr;
    ushort * _elements_base_ptr;

    struct dc_data
    {
        const glm::uint _nelements;
        const glm::uint _first_index;
        const glm::uint _nvertices;
        const glm::uint _first_vertex;

        dc_data(
            const glm::uint nelements,
            const glm::uint first_index,
            const glm::uint nvertices,
            const glm::uint first_vertex)
            : _nelements(nelements)
            , _first_index(first_index)
            , _nvertices(nvertices)
            , _first_vertex(first_vertex)
            {}
    };

    std::vector<dc_data> _dc_data;

    enum BenchmarkMode
    {
        // no textures (depth prepass, shadow mapping)
        BenchNaive,
        BenchBaseInstance,
        BenchIndirect,
        BenchInstancing,
        BenchBaseVertex,
    };

    enum TexturingMode
    {
        BenchTexNone,
        BenchTexNaive,     // glBindTexture
        BenchTexArray,
        BenchTexBindless,
    };

    bool _use_vbo;
    bool _one_mesh;

    BenchmarkMode _bench_mode;
    TexturingMode _tex_mode;
	int _mesh_size_opt;
    int _mesh_size_x;
	int _mesh_size_y;
	int _mesh_size_z;
	int _tex_freq;

    benchmark * const _app;

};

#endif // __ASYNC_VBO_TRANSFERS_SCENE_H__
