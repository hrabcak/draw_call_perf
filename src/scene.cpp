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

#include "scene.h"
#include "base/base.h"
#include "base/frame_context.h"
#include "base/hptimer.h"

#include "cube_gen.h"
#include "gen_tex.h"
#include "benchmark.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

#define SIMPLEX_BIAS_X		4382
#define SIMPLEX_BIAS_Y		14837

#define GRASS_TILE_W			10.0
#define GRASS_TUFTS_PER_TILE	4096
#define GRASS_BLADES_PER_TUFT	16

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::scene(benchmark * const app)
	:_cur_next_block(0,0)
	,_tms()
	, _bboxes()
	, _hws()
	, _flags()

    , _prg(0)
    , _prg_tb_blocks(-1)
    , _prg_ctx(-1)
    , _prg_tb_pos(-1)
	, _prg_tb_nor_uv(-1)
    , _prg_tex(-1)

    , _buffer_elem(0)
    , _buffer_pos(0)
    , _buffer_nor_uv(0)
    , _buffer_tex_handles(0)

    , _tb_pos(0)
    , _tb_tex_handles(0)

    , _texs()
    , _tex_handles()

    , _bench_mode(BenchIndirect)
    , _tex_mode(BenchTexArray)

    , _max_array_layers(1)

    , _app(app)

    , _vertices_base_ptr(0)
    , _norm_uv_base_ptr(0)
    , _elements_base_ptr(0)

    , _use_vbo(false)
    , _one_mesh(false)

    , _mesh_size(2)
    , _tex_freq(1)
{
	_tms.reserve(MAX_BLOCK_COUNT);
	_bboxes.reserve(MAX_BLOCK_COUNT);
	_hws.reserve(MAX_BLOCK_COUNT);
	_flags.reserve(MAX_BLOCK_COUNT);

    _test_names.push_back("Test 0: Naive");
	_test_names.push_back("Test 1: Base instance");
	_test_names.push_back("Test 2: Indirect");
    _test_names.push_back("Test 3: Instancing");

    // set modes from cfg
    if (base::cfg().test != -1) {
        _bench_mode = BenchmarkMode(base::cfg().test);
    }
    _tex_mode = TexturingMode(base::cfg().tex_mode);
    _mesh_size = base::cfg().mesh_size;
    _tex_freq = base::cfg().tex_freq;
    _use_vbo = base::cfg().use_vbo;
    _one_mesh = base::cfg().one_mesh;
}

//X 3056
//Y 9134

//X 4382
//Y 14837

//X 7800
//Y 17720

//X 9310
//Y 14031

//X 12951
//Y 5408
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::~scene() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::add_block(
	const glm::vec3 &pos,
	const glm::vec3 &size,
	const float heading)
{
	const vec3 half_size = size * 0.5f;

	mat4 tm = rotate(mat4(1), heading, vec3(0,0,1));
	mat4 bbox = tm;

	tm[0].x = half_size.x;
	tm[1].y = half_size.y;
	tm[2].z = half_size.z;
	tm[3] = vec4(pos,1);
	_tms.push_back(tm);

	bbox[3] = vec4(pos + vec3(0, half_size.y, 0), 1);
	_bboxes.push_back(bbox);

	_hws.push_back(size * 0.5f);
	_flags.push_back(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::load_and_init_shaders(const base::source_location &loc)
{
	assert(_prg == 0);

    std::string cfg;

    cfg += "#version 430\n";

   /* if (!_use_vbo)
           cfg += "#define USE_TB_FOR_VERTEX_DATA 1\n";

    switch (_bench_mode) {
    case BenchNaive:
        cfg += "#define USE_NAIVE_DRAW 1\n";
        break;
    case BenchBaseVertex:
        cfg += "#define USE_BASE_VERTEX_DRAW 1\n";
        break;
    case BenchInstancing:
        cfg += "#define USE_INSTANCED_DRAW 1\n";
        break;
    case BenchIndirect:
        cfg += "#define USE_INDIRECT_DRAW 1\n";
        break;
    case BenchBaseInstance:
        cfg += "#define USE_BASE_INSTANCE 1\n";
        break;
    }
    
    switch (_tex_mode) {
    case BenchTexNaive:
        cfg += "#define USE_NAIVE_TEX 1\n";
        break;
    case BenchTexArray:
        cfg += "#define USE_ARRAY_TEX 1\n";
        break;
    case BenchTexBindless:
        cfg += "#define USE_BINDLESS_TEX 1\n";
        break;
    }*/
	
	if (base::cfg().use_instancing){
		// without GS

		_prg = base::create_program(
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_inst_v.glsl",
			GL_VERTEX_SHADER),
			/*base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_g.glsl",
			GL_GEOMETRY_SHADER)*/ 0,
			base::create_and_compile_shader(
			SRC_LOCATION,
			cfg,
			"shaders/proc_f.glsl",
			GL_FRAGMENT_SHADER));
		base::link_program(loc, _prg);

	}
	else{
		// with GS

		_prg = base::create_program(
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
		base::link_program(loc, _prg);
	}
	
    // GET UNIFORM STUFF

	_prg_ctx = glGetUniformBlockIndex(_prg, "context");

	_prg2 = base::create_program(
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

	base::link_program(loc,_prg2);

	_prg2_ctx = glGetUniformBlockIndex(_prg2, "context");

    /*if (!_use_vbo) {
        _prg_tb_pos = get_uniform_location(loc, _prg, "tb_pos");
        if (_tex_mode != BenchTexNone)
            _prg_tb_nor_uv = get_uniform_location(loc, _prg, "tb_nor_uv");
    }
    _prg_ctx = glGetUniformBlockIndex(_prg, "context");

    switch (_bench_mode) {
    case BenchNaive:
        break;
    case BenchBaseVertex:
        break;
    case BenchInstancing:
        break;
    case BenchIndirect:
        break;
    case BenchBaseInstance:
        break;
    }

    switch (_tex_mode) {
    case BenchTexNaive:
    case BenchTexArray:
        _prg_tex = get_uniform_location(loc, _prg, "mat_tex");
        break;
    case BenchTexBindless:
        _prg_tex = get_uniform_location(loc, _prg, "tb_tex_handles");
        break;
    }*/
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::init_gpu_stuff(const base::source_location &loc)
{
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &_max_array_layers);

    load_and_init_shaders(loc);

    const int * vtx_tbl = scene::get_vtx_tbl();
	const int * ele_tbl = scene::get_ele_tbl();

    uint nvertices = vtx_tbl[_mesh_size] * 2;
    uint nelements = ele_tbl[_mesh_size] * 2;

    std::vector<ushort> elements;
    std::vector<int2> vertices;
    std::vector<int2> norm_uv;

    elements.resize(nelements * MAX_BLOCK_COUNT);
    vertices.resize(nvertices * MAX_BLOCK_COUNT);
    norm_uv.resize(nvertices * MAX_BLOCK_COUNT);

    ushort * elements_ptr = elements.begin()._Ptr;
    int2 * vertices_ptr = vertices.begin()._Ptr;
    int2 * norm_uv_ptr = norm_uv.begin()._Ptr;

    int i = 0;
    int e = MAX_BLOCK_COUNT;
    do {
        if (_bench_mode != BenchInstancing || i == 0) {
            gen_cube<int2>(
                _mesh_size,
                vertices_ptr,
                norm_uv_ptr,
                elements_ptr,
                nullptr,
                nullptr,
                nelements,
                nvertices,
                !base::cfg().dont_rnd_cubes,  // argument true if deform cube
                true); // argument true if multipass

            _dc_data.push_back(dc_data(
                nelements,
                uint(elements_ptr - elements.begin()._Ptr),
                nvertices,
                uint(vertices_ptr - vertices.begin()._Ptr)));
            elements_ptr += nelements;
            vertices_ptr += nvertices;
            norm_uv_ptr += nvertices;
        }
        else {
            memcpy(elements_ptr, elements.begin()._Ptr, _dc_data[0]._nelements * sizeof(*elements_ptr));
            memcpy(vertices_ptr, vertices.begin()._Ptr, _dc_data[0]._nvertices * sizeof(*vertices_ptr));
            memcpy(norm_uv_ptr, norm_uv.begin()._Ptr, _dc_data[0]._nvertices * sizeof(*norm_uv_ptr));

            _dc_data.push_back(_dc_data[0]);

            elements_ptr += _dc_data[0]._nelements;
            vertices_ptr += _dc_data[0]._nvertices;
            norm_uv_ptr += _dc_data[0]._nvertices;
        }
    } while (++i != e);

    nelements = uint(elements_ptr - elements.begin()._Ptr);
    nvertices = uint(vertices_ptr - vertices.begin()._Ptr);

    _buffer_elem = base::create_buffer<ushort>(nelements, 0, elements.begin()._Ptr);
    _buffer_pos = base::create_buffer<glm::ivec2>(nvertices, 0, vertices.begin()._Ptr);
    _buffer_nor_uv = base::create_buffer<glm::ivec2>(nvertices, 0, norm_uv.begin()._Ptr);

    // create VBO for vertices positions
    if (_use_vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, _buffer_pos);
        glVertexAttribIPointer(0, 2, GL_INT, 0, (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffer_nor_uv);
        glVertexAttribIPointer(1, 2, GL_INT, 0, (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    // create texture buffer for vertices
    glGenTextures(1, &_tb_pos);
    glBindTexture(GL_TEXTURE_BUFFER, _tb_pos);
    glTexBuffer(
        GL_TEXTURE_BUFFER,
        base::get_pfd(base::PF_RG32I)->_internal,
        _buffer_pos);

	glGenTextures(1, &_tb_nor_uv);
	glBindTexture(GL_TEXTURE_BUFFER, _tb_nor_uv);
	glTexBuffer(
        GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_RG32I)->_internal,
		_buffer_nor_uv);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

    if (_tex_mode != BenchTexNone) {
        create_textures(loc);
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::post_gpu_init()
{
    create_test_scene();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_textures(const base::source_location &)
{
    const int width = base::cfg().tex_size;
    const int tex_size = width * width;
    const int ntex = MAX_BLOCK_COUNT;
    const base::pixelfmt pf = base::PF_BGRA8_SRGB;
    const int tex_size_bytes = tex_size * base::get_pfd(pf)->_size;
 
    std::vector<uchar4> data;
    data.resize(tex_size);
    std::vector<glm::uint32> tmp;
    tmp.resize(tex_size);

    if (_tex_mode != BenchTexArray) {
        for (int i = 0; i < ntex; ++i) {
            gen_texture(data.begin()._Ptr, width, width >> 2, i, &tmp[0]);
            const GLint tex = create_texture_storage(
                width,
                width,
                pf,
                data.begin()._Ptr,
                0,
                true);

            _texs.push_back(tex);
        }
    }
    else {
        const base::pfd * const pfd = base::get_pfd(pf);
        for (int i = 0; i < ntex; i += _max_array_layers) {
            const GLint tex = create_texture_array(
                width,
                width,
                _max_array_layers,
                pf,
                0);

            for (int j = 0; j < _max_array_layers; ++j) {
                gen_texture(&data[0], width, width >> 2, i+j, &tmp[0]);
                glTextureSubImage3DEXT(
                    tex,
                    GL_TEXTURE_2D_ARRAY,
                    0,
                    0,
                    0,
                    j,
                    width,
                    width,
                    1,
                    pfd->_format,
                    pfd->_type,
                    &data[0]);
            }

            _texs.push_back(tex);

            glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    
    if (_tex_mode == BenchTexBindless) {
        for (int i = 0; i < ntex; ++i) {
            const GLuint64 handle = glGetTextureHandleARB(_texs[i]);
            glMakeTextureHandleResidentARB(handle);
            _tex_handles.push_back(handle);
        }

        _buffer_tex_handles = base::create_buffer<GLuint64>(ntex, 0, &_tex_handles[0]);

        // create texture buffer for texture handles
        glGenTextures(1, &_tb_tex_handles);
        glBindTexture(GL_TEXTURE_BUFFER, _tb_tex_handles);
        glTexBuffer(
            GL_TEXTURE_BUFFER,
            base::get_pfd(base::PF_RG32I)->_internal,
            _buffer_tex_handles);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::update(base::frame_context * const ctx)
{
    frustum_check(ctx);
    upload_blocks_to_gpu(SRC_LOCATION, ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

inline glm::vec4 normalize_plane(const glm::vec4 &p)
{
	return p*(1.0f/length(vec3(p)));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_frustum_planes(
	vec4 *planes,
	const glm::mat4 &mvp)
{
    // Left plane
    planes[0] = normalize_plane(vec4(
		mvp[0].w+mvp[0].x,
		mvp[1].w+mvp[1].x,
		mvp[2].w+mvp[2].x,
		mvp[3].w+mvp[3].x));
 
    // Right plane
    planes[1]=normalize_plane(vec4(
		mvp[0].w-mvp[0].x,
		mvp[1].w-mvp[1].x,
		mvp[2].w-mvp[2].x,
		mvp[3].w-mvp[3].x));
 
    // Top plane
    planes[2]=normalize_plane(vec4(
		mvp[0].w-mvp[0].y,
		mvp[1].w-mvp[1].y,
		mvp[2].w-mvp[2].y,
		mvp[3].w-mvp[3].y));
 
    // Bottom plane
    planes[3]=normalize_plane(vec4(
		mvp[0].w+mvp[0].y,
		mvp[1].w+mvp[1].y,
		mvp[2].w+mvp[2].y,
		mvp[3].w+mvp[3].y));
 
    // Far plane
    planes[4]=normalize_plane(vec4(
		mvp[0].w-mvp[0].z,
		mvp[1].w-mvp[1].z,
		mvp[2].w-mvp[2].z,
		mvp[3].w-mvp[3].z));

	// Near plane
    planes[5]=normalize_plane(vec4(
		mvp[0].z,mvp[1].z,mvp[2].z,mvp[3].z));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int scene::frustum_check(base::frame_context * const ctx)
{
	// prepare frustum planes
	const int num_planes=6;
	glm::vec4 planes[num_planes];
	create_frustum_planes(planes,ctx->_mvp);

	glm::vec4 * const planes_end = planes + num_planes;

	const mat4 *bbox = &_bboxes[0];
	const vec3 *hw = &_hws[0];
	unsigned int *flags = &_flags[0];
	const unsigned int *e = &_flags[0] + _flags.size();

    const bool dont_check = true;

	ctx->_num_visible_blocks[0] = 0;

	for( ; flags != e; ++hw, ++bbox, ++flags) {
		const vec4 *plane = planes;
		if(!dont_check) {
			for(; plane != planes_end; ++plane) {
				// transform plane's normal to bbox space
				const glm::vec3 npv(
					dot((*bbox)[0], *plane),
					dot((*bbox)[1], *plane),
					dot((*bbox)[2], *plane));

				// and do standard p/n vertex aabb check
				const float mp = dot((*bbox)[3], *plane);
				const float np = dot(*hw, abs(npv));

				if(mp + np < 0.0f) break;
			}

			*flags = *flags & (~Visible);
		}

		if(plane == planes_end || dont_check) {
			*flags |= Visible;
			++ctx->_num_visible_blocks[0];
		}
	}

	vec4 a = ctx->_imvp * vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vec4 b = ctx->_imvp * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	a = vec4(a.x / a.w, a.y / a.w, a.z / a.w, 1.0);
	b = vec4(b.x / b.w, b.y / b.w, b.z / b.w, 1.0);

	_lt_ray = glm::normalize(vec3(b - a));

	a = ctx->_imvp * vec4(1.0f, 1.0f, -1.0f, 1.0f);
	b = ctx->_imvp * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	a = vec4(a.x / a.w, a.y / a.w, a.z / a.w, 1.0);
	b = vec4(b.x / b.w, b.y / b.w, b.z / b.w, 1.0);

	_rt_ray = glm::normalize(vec3(b - a));

	a = ctx->_imvp * vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	b = ctx->_imvp * vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	a = vec4(a.x / a.w, a.y / a.w, a.z / a.w, 1.0);
	b = vec4(b.x / b.w, b.y / b.w, b.z / b.w, 1.0);

	_lb_ray = glm::normalize(vec3(b - a));
	
	a = ctx->_imvp * vec4(1.0f, -1.0f, -1.0f, 1.0f);
	b = ctx->_imvp * vec4(1.0f, -1.0f, 1.0f, 1.0f);
	a = vec4(a.x / a.w, a.y / a.w, a.z / a.w, 1.0);
	b = vec4(b.x / b.w, b.y / b.w, b.z / b.w, 1.0);

	_rb_ray = glm::normalize(vec3(b - a));
	
	_cam_pos = vec3(ctx->_view[3]);
	_cam_view = vec3(ctx->_view[2]);

	

	return ctx->_num_visible_blocks[0];
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::upload_blocks_to_gpu(
	const base::source_location &loc,
	base::frame_context *ctx)
{
    static int counter = 0;

	ctx->_scene_data_ptr_size = 0;
	ctx->_scene_data_ptr_size = ctx->_num_visible_blocks[0];
	ctx->_scene_data_ptr_size *= sizeof(base::block_data);

	if(ctx->_scene_data_ptr_size > 0) {
		base::block_data *ptr[NumTypes];
	
		ptr[0] = ctx->_scene_data_ptr;

		if(ptr[0] == 0)
			throw base::exception(loc.to_str())
				<< "Cannot map visible blocks VBO to CPU memory!";

		for(int i = 1; i < NumTypes; ++i)
			ptr[i] = ptr[i - 1] + ctx->_num_visible_blocks[i - 1];

		const mat4 *tm = &_tms[0];
		const mat4 *e = &_tms[0] + _tms.size();
		unsigned int *flags = &_flags[0];
        base::cmd * cmd = ctx->_cmd_data_ptr;

        base::dc_gpu_data * dc_gpu = ctx->_drawid_data_ptr + ctx->_drawid_data_offset;
        dc_data * dc = &_dc_data[0];

		// transform visible blocks to screen and add to VBO 
        for (; tm != e; ++tm, ++flags, ++dc, ++dc_gpu, ++cmd) {
            const char type = 0;
            const uint offset = uint(ptr[type] - ctx->_scene_data_ptr);

            ptr[type]->_tm = *tm;

            if (!_one_mesh) {
                new (dc_gpu)base::dc_gpu_data(
                    ctx->_scene_data_offset + offset,
                    dc->_first_vertex,
                    offset);
                new (cmd)base::cmd(
                    dc->_nelements,
                    1,
                    dc->_first_index,
                    _use_vbo ? dc->_first_vertex : 0,
                    ctx->_scene_data_offset + offset);
            }
            else {
                new (dc_gpu)base::dc_gpu_data(
                    ctx->_scene_data_offset + offset,
                    _dc_data[0]._first_vertex,
                    offset);
                new (cmd)base::cmd(
                    _dc_data[0]._nelements,
                    1,
                    _dc_data[0]._first_index,
                    _use_vbo ? _dc_data[0]._first_vertex : 0,
                    ctx->_scene_data_offset + offset);
            }

            ptr[type]++;
        }
	}

    ctx->_ctx_data_ptr->_mvp = ctx->_mvp;
    if (_bench_mode == BenchInstancing) {
        ctx->_ctx_data_ptr->_mesh_size = _dc_data[0]._nvertices;
    }
    else {
        ctx->_ctx_data_ptr->_mesh_size = _one_mesh
            ? 0
            : _dc_data[0]._nvertices;
    }

    ctx->_ctx_data_ptr->_tex_freq = _tex_freq;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::bind_texture(int counter)
{
    //if ((counter & 0x7) != 0)
    //    return;

    //glUniformHandleui64ARB(_prg_tex, _tex_handles[counter]);
    
    if (_tex_mode == BenchTexNaive) {
        if ((_tex_freq != -1 && (counter & ((1 << _tex_freq) - 1)) == 0)
            || (_tex_freq == -1 && counter == 0))
            glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_2D, _texs[counter]);
    }
    else if (_tex_mode == BenchTexArray && (counter & 0x7ff) == 0) {
        glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_2D_ARRAY, _texs[counter >> 11]);
    }
    
    //const int cm = counter % 190;
    //if (cm == 0)
    //    glBindTextures(2, 190, &_texs[counter]);
    //glUniform1i(_prg_tex, cm + 2);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::gpu_draw(base::frame_context * const ctx)
{
	/*glUseProgram(_prg);

    //glColorMask(false, false, false, false);
    //glPolygonMode(GL_FRONT, GL_LINE);

    base::hptimer timer;

    timer.start();

    // SET FRAME RENDER STATES

    bool fast_drawcall = false;
    bool fast_drawcall_old_way = false;
    bool fast_draw_call_gl33 = false;
    bool use_instancing = false;

    glVertexAttribI4i(13, 0, 0, 0, 0);
    glBindBufferRange(
        GL_UNIFORM_BUFFER,
        _prg_ctx,
        ctx->_ctx_vbo,
        ctx->_ctx_id * sizeof(base::ctx_data),
        sizeof(base::ctx_data));

    // bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_elem);

	// bind texture buffer with block data
    glUniform1i(_prg_tb_blocks, 0);
    glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_BUFFER, ctx->_scene_tb);

    // bind texture buffer with vertex data
    glUniform1i(_prg_tb_pos, 1);
    glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_BUFFER, _tb_pos);

    if (_tex_mode != BenchTexNone) {
        glUniform1i(_prg_tb_nor_uv, 2);
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_BUFFER, _tb_nor_uv);
    }

    switch (_bench_mode) {
    case BenchNaive:
        fast_drawcall = true;
        fast_drawcall_old_way = true;
        glVertexAttribI1i(13, ctx->_scene_data_offset);
        break;
    case BenchBaseVertex:
        fast_drawcall = true;
        fast_draw_call_gl33 = true;
        glVertexAttribI1i(13, ctx->_scene_data_offset);
        break;
    case BenchInstancing:
        use_instancing = true;
        glVertexAttribI4i(13, ctx->_scene_data_offset, 0, 0, 0);
        break;
    case BenchIndirect:
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ctx->_cmd_vbo);
        glEnableVertexAttribArray(13);
        break;
    case BenchBaseInstance:
        fast_drawcall = true;
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ctx->_cmd_vbo);
        glEnableVertexAttribArray(13);
        break;
    }

    bool use_tex = false;

    switch (_tex_mode) {
    case BenchTexNaive:
        glUniform1i(_prg_tex, 3);
        use_tex = true;
        break;
    case BenchTexArray:
        glUniform1i(_prg_tex, 3);
        glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_2D_ARRAY, _texs[0]);
        use_tex = true;
        break;
    case BenchTexBindless:
        glUniform1i(_prg_tex, 3);
        glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_BUFFER, _tb_tex_handles);
        break;
    }

    if (_use_vbo) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }


    glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

    const uint nblocks = ctx->_num_visible_blocks[0];

    if (nblocks == 0)
        return;
    
    if (!fast_drawcall) {
        if (use_instancing) {
            if (_tex_mode == BenchTexBindless || _tex_mode == BenchTexNone) {
                glDrawElementsInstanced(
                    GL_TRIANGLES,
                    _dc_data[0]._nelements,
                    GL_UNSIGNED_SHORT,
                    0,
                    nblocks);
            }
            else if (_tex_mode == BenchTexArray) {
                for (uint i = 0; i < nblocks; i += _max_array_layers) {
                    bind_texture(i);
                    glVertexAttribI3i(
                        13,
                        ctx->_scene_data_offset + i,
                        _dc_data[i]._first_vertex,
                        i);
                    glDrawElementsInstanced(
                        GL_TRIANGLES,
                        _dc_data[_one_mesh ? 0 : i]._nelements,
                        GL_UNSIGNED_SHORT,
                        (void*)(_dc_data[_one_mesh ? 0 : i]._first_index * sizeof(*_elements_base_ptr)),
                        _max_array_layers);
                }
            }
        }
        else {
            if (_tex_mode == BenchTexBindless || _tex_mode == BenchTexNone) {
                glMultiDrawElementsIndirect(
                    GL_TRIANGLES,
                    GL_UNSIGNED_SHORT,
                    (void*)((ctx->_scene_data_offset) * sizeof(base::cmd)),
                    nblocks,
                    0);
            }
            else if (_tex_mode == BenchTexArray) {
                for (uint i = 0; i < nblocks; i += _max_array_layers) {
                    bind_texture(i);
                    glMultiDrawElementsIndirect(
                        GL_TRIANGLES,
                        GL_UNSIGNED_SHORT,
                        (void*)((ctx->_scene_data_offset + i) * sizeof(base::cmd)),
                        _max_array_layers,
                        0);
                }
            }
        }
    }
    else {
        int counter = 0;
        int offset = 0;
        const int offset_e = ctx->_num_visible_blocks[0];
        dc_data * dc = _dc_data.begin()._Ptr;
        while (offset != offset_e) {
            if (fast_drawcall_old_way) {
                if (use_tex)
                    bind_texture(counter);
                glVertexAttribI3i(
                    13,
                    ctx->_scene_data_offset + offset,
                    dc->_first_vertex,
                    offset);
                glDrawElements(
                    GL_TRIANGLES,
                    dc->_nelements,
                    GL_UNSIGNED_SHORT,
                    (void*)(dc->_first_index * sizeof(*_elements_base_ptr)));
            }
            else if (fast_draw_call_gl33) {
                if (use_tex)
                    bind_texture(counter);
                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    dc->_nelements,
                    GL_UNSIGNED_SHORT,
                    (void*)(dc->_first_index * sizeof(*_elements_base_ptr)),
                    offset << 12);
            }
            else {
                if (use_tex)
                    bind_texture(counter);
                if (!_use_vbo) {
                    glDrawElementsInstancedBaseInstance(
                        GL_TRIANGLES,
                        dc->_nelements,
                        GL_UNSIGNED_SHORT,
                        (void*)(dc->_first_index * sizeof(*_elements_base_ptr)),
                        1,
                        offset);
                }
                else {
                    glDrawElementsInstancedBaseVertexBaseInstance(
                        GL_TRIANGLES,
                        dc->_nelements,
                        GL_UNSIGNED_SHORT,
                        (void*)(dc->_first_index * sizeof(*_elements_base_ptr)),
                        1,
                        dc->_first_vertex,
                        offset);
                }
            }
            offset++;
            counter++;
            dc++;
        }
    }

    glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);

    ctx->_cpu_render_time = timer.elapsed_time();*/

	memset(_grass_tiles, 0, MAX_GRASS_TILES*sizeof(vec2));
	calculate_visible_tiles(16, GRASS_TILE_W);

	base::hptimer timer;

	timer.start();

	glDisable(GL_CULL_FACE);

	glUseProgram(_prg2);

	GLint pos_uloc = get_uniform_location(SRC_LOCATION, _prg2, "tile_pos");

	glBindBufferRange(
		GL_UNIFORM_BUFFER,
		_prg2_ctx,
		ctx->_ctx_vbo,
		ctx->_ctx_id * sizeof(base::ctx_data),
		sizeof(base::ctx_data));

	for (int i = 0; i < 16; i++){
		glUniform2f(pos_uloc, _grass_tiles[i].x, _grass_tiles[i].y);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glUseProgram(_prg);
	
	pos_uloc = get_uniform_location(SRC_LOCATION, _prg2, "tile_pos");
	
	

	glBindBufferRange(
	GL_UNIFORM_BUFFER,
	_prg_ctx,
	ctx->_ctx_vbo,
	ctx->_ctx_id * sizeof(base::ctx_data),
	sizeof(base::ctx_data));

	glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

	for (int i = 0; i < 16; i++){
		glUniform2f(pos_uloc, _grass_tiles[i].x, _grass_tiles[i].y);
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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_test_scene()
{
	_cur_block = get_perspective_block_bound(1,32.0f);
	for (int i = 0; i < 116; ++i){
	    add_test_block(true);
	 }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::add_test_block(bool add_peaks)
{
	const int grid_size = 16;
	const int grid_size2 = grid_size * grid_size;
	int max_height = 5;

	const glm::vec3 box_size(2.0f, 2.0f, 2.0f);
	std::vector<int> height_map;
	height_map.resize(grid_size2, 0);

	const float world_row_len = box_size.z * grid_size;
    
    const float grid_size_r = 1.f / float(grid_size);
    base::stats_data & stats = base::stats();

	ushort cur_z = (_cur_block & 0xffff0000) >> 16;
	ushort cur_bound = get_perspective_block_bound(cur_z + 1, world_row_len);
	short cur_x = (_cur_block & 0xffff);

    // bottom layer
	for (int y = 0; y < grid_size; y++) {
		for (int x = 0; x < grid_size; x++) {
            // create instances
			const float s0b = glm::simplex(glm::vec2(x + cur_x*grid_size + SIMPLEX_BIAS_X, y + cur_z*grid_size + SIMPLEX_BIAS_Y) * grid_size_r);
			const float s1b = glm::simplex(glm::vec2(x + cur_x*grid_size + SIMPLEX_BIAS_X, y + cur_z*grid_size + SIMPLEX_BIAS_Y) * 2.f * grid_size_r);
			float sb = (((((s0b + s1b) + 2.f) / 4.f)));
			
			float peak_b = glm::clamp(glm::pow(s0b, 10.0f),0.1f,1.0f)*15.0f;

			const int height_t =  int(sb * max_height);

			const float s0t = glm::simplex(glm::vec2(x + cur_x*grid_size + 64 + SIMPLEX_BIAS_X, y + cur_z*grid_size + 64 + SIMPLEX_BIAS_Y) * grid_size_r);
			const float s1t = glm::simplex(glm::vec2(x + cur_x*grid_size + 64 + SIMPLEX_BIAS_X, y + cur_z*grid_size + 64 + SIMPLEX_BIAS_Y) * 2.f * grid_size_r);
			float st = (((((s0t + s1t) + 2.f) / 4.f)));

			float peak_t = glm::clamp(glm::pow(s0t, 10.0f), 0.1f, 1.0f)*15.0f;

			const int height_b =  int(st * max_height);

			

            const float xpos = (((cur_x - 1) * grid_size) + x) * 2.f;
			const float ypos_b = height_b * 2.0f;
			const float ypos_t = height_t * 2.0f;
			const float zpos = ((cur_z * grid_size) + y) * 2.f;


            int idx = int(_tms.size());
			int idx_e = idx;

			add_block(
				glm::vec3(xpos, ypos_b, zpos),
				box_size,
				0);

			idx_e++;

			add_block(
				glm::vec3(xpos, 32.f - ypos_t, zpos),
				box_size,
				0);
			idx_e++;
			
			if (add_peaks && peak_b > 1.0f){
				for (float peak_ypos = ypos_b + 2.0f; peak_ypos < peak_t*2.0f; peak_ypos += box_size.y){
					add_block(
						glm::vec3(xpos, peak_ypos, zpos),
						box_size,
						0);
					idx_e++;
				}
			}

			if (add_peaks &&  peak_t > 1.0f){
				for (float peak_ypos = ypos_t + 2.0f; peak_ypos < peak_t*2.0f; peak_ypos += box_size.y){
					add_block(
						glm::vec3(xpos,32 - peak_ypos, zpos),
						box_size,
						0);
					idx_e++;
				}
			}

            
            do {
                stats._ndrawcalls += 1;
                stats._ntriangles += _dc_data[idx]._nelements / 3;
                stats._nvertices += _dc_data[idx]._nvertices;
            } while (++idx != idx_e);
        }
	}

	// set next block coords
	if (cur_x - 1 <= -cur_bound){
		cur_z += 1;
		cur_x = get_perspective_block_bound(cur_z + 1,world_row_len);
		_cur_block = (cur_z << 16) | ushort(cur_x);
	}
	else{
		_cur_block = (cur_z << 16) | ushort(cur_x - 1);
	}

};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int scene::get_perspective_block_bound(int row, float world_row_len){
	float fovx = glm::atan(glm::tan(_app->get_fovy()/2.0f)*(_app->get_aspect()));
	return int(glm::ceil((glm::tan(fovx)*row*world_row_len) / int(world_row_len)));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::calculate_visible_tiles(int ntiles, float tile_size) {
	/*float fovx = atan(tan(glm::radians(_app->get_fovy()) * 0.5f) * _app->get_aspect());
	vec3 view_proj = -vec3(_cam_view.x,0.0f,_cam_view.z);
	vec3 cam_proj = vec3(_cam_pos.x,0.0f,_cam_pos.z);

	view_proj = glm::normalize(view_proj);

	vec3 left = glm::rotateY(view_proj, fovx);
	
	vec3 right = glm::rotateY(view_proj, -fovx);

	vec3 far_l = cam_proj + 20 * left;
	vec3 far_r = cam_proj + 20 * right;

	//left = vec3(-left.z, 0.0f, left.x);
	//right = vec3(-right.z, 0.0f, right.x);

	

	vec2 min = vec2(glm::min(far_r.x, glm::min(cam_proj.x, far_l.x)), glm::min(far_r.z, glm::min(cam_proj.z, far_l.z)));
	vec2 max = vec2(glm::max(far_r.x, glm::max(cam_proj.x, far_l.x)), glm::max(far_r.z, glm::max(cam_proj.z, far_l.z)));

	int t_count = 0;

	vec3 res = cam_proj;

	for (int z = 0; z  < 256; z += 1){
		res += left;
		_grass_tiles[z] = vec2(glm::floor(res.x), glm::floor(res.z));
	}*/
	/*
	float d = -_cam_pos.y / _lb_ray.y;

	vec3 lb = _cam_pos + d*_lb_ray;

	d = -_cam_pos.y / _rb_ray.y;

	vec3 rb = _cam_pos + d*_rb_ray;

	vec3 rb_ray_p(_rb_ray.x, 0.0f, _rb_ray.z);
	vec3 lb_ray_p(_lb_ray.x, 0.0f, _lb_ray.z);

	rb_ray_p = glm::normalize(rb_ray_p);
	lb_ray_p = glm::normalize(lb_ray_p);
	
	vec3 lt = lb + lb_ray_p * 15;
	vec3 rt = rb + rb_ray_p * 15;

	vec3 min = glm::min(rb,glm::min(rt, glm::min(lt, lb)));
	vec3 max = glm::max(rb, glm::max(rt, glm::max(lt, lb)));

	int ntil = 0;

	
	
	for (int z = glm::floor(min.z / tile_size); z < glm::ceil(max.z / tile_size); z++){
		for (int x = glm::floor(min.x / tile_size); x < glm::ceil(max.x / tile_size); x++){
			_grass_tiles[ntil] = vec2(float(x),float(z));
			ntil++;
			if (ntil >= ntiles){
				break;
			}
		}

		if (ntil >= ntiles){
			break;
		}
	}*/

	int ntil = 0;

	int side = int(glm::sqrt(ntiles)) >> 1;

	vec2 mid_pos = vec2(glm::floor(_cam_pos.x / tile_size), glm::floor(_cam_pos.z / tile_size));

	for (int z = mid_pos.y - side; z < mid_pos.y + side; z++){
		for (int x = mid_pos.x - side; x < mid_pos.x + side; x++){
			_grass_tiles[ntil] = vec2(float(x), float(z));
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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=