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
		static const int vtx_tbl[] = { 24, 54, 96, 150, 216, 294 }; 
		return &vtx_tbl[0]; 
	}
	static const int * get_ele_tbl() { 
		static const int ele_tbl[] = { 26, 144, 324, 576, 900, 1296 }; 
		return &ele_tbl[0]; 
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
    int _mesh_size;
    int _tex_freq;

    benchmark * const _app;

};

#endif // __ASYNC_VBO_TRANSFERS_SCENE_H__
