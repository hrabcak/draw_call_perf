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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::scene(benchmark * const app)
	:_cur_next_block(0,0)
	,_tms()
	, _bboxes()
	, _hws()
	, _flags()
	, _blocks()

    , _prg(0)
    , _prg_tb_blocks(-1)
    , _prg_ctx(-1)
    , _prg_tb_pos(-1)
    , _prg_tex(-1)

    , _buffer_elem(0)
    , _buffer_pos(0)
    , _buffer_nor_uv(0)
    , _buffer_tex_handles(0)

    , _nelements(0)
    , _nvertices(0)

    , _tb_pos(0)
    , _tb_tex_handles(0)

    , _texs()
    , _tex_handles()

    , _bench_mode(BenchIndirect)
    , _tex_mode(BenchTexArray)

    , _max_array_layers(1)

    , _app(app)
{
	_tms.reserve(MAX_BLOCK_COUNT);
	_bboxes.reserve(MAX_BLOCK_COUNT);
	_hws.reserve(MAX_BLOCK_COUNT);
	_flags.reserve(MAX_BLOCK_COUNT);

    create_test_scene();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::~scene() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::block* scene::add_block(
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

	_blocks.push_back(
		block(
			(_tms.end()-1)._Ptr,
			(_flags.end()-1)._Ptr,
			(_hws.end()-1)._Ptr));
	
	return (_blocks.end()-1)._Ptr;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::load_and_init_shaders(const base::source_location &loc)
{
	assert(_prg == 0);

    std::string cfg;

    cfg += "#version 430\n"
           "#define USE_TB_FOR_VERTEX_DATA 1\n";

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
    case BenchTexBufvsVBO:
        break;
    case BenchProceduralVertices:
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
    }

	_prg = base::create_program(
		base::create_and_compile_shader(
			SRC_LOCATION,
            cfg,
            "shaders/block_v.glsl",
            GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(
			SRC_LOCATION,
            cfg,
            "shaders/block_f.glsl",
            GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

    // GET UNIFORM STUFF

    _prg_tb_blocks = get_uniform_location(loc, _prg, "tb_blocks");
    _prg_tb_pos = get_uniform_location(loc, _prg, "tb_pos");
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
    case BenchTexBufvsVBO:
        break;
    case BenchProceduralVertices:
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
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::init_gpu_stuff(const base::source_location &loc)
{
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &_max_array_layers);

    load_and_init_shaders(loc);

    const int tess_level = 2;

    get_face_and_vert_count_for_tess_level(tess_level, _nelements, _nvertices);

    std::vector<unsigned short> elements;
    std::vector<float> vertices;
    elements.resize(_nelements * scene::MAX_BLOCK_COUNT);
    vertices.resize(_nvertices * 8 * scene::MAX_BLOCK_COUNT);

	unsigned int dummy; // toto som dolpnil len aby sa to dalo skompilovat

    gen_cube<float>(tess_level, &vertices[0], &elements[0],dummy,dummy);

    const double scale = (glm::pow(2.0, 20.0) - 1.0) / 1.0;
    glm::ivec2 * const ptr = reinterpret_cast<glm::ivec2*>(&vertices[0]);
    for (unsigned i = 0; i < _nvertices; ++i) {
        const int idx = i * 8;
        ptr[i] = base::pack_to_pos3x21b(dvec3(vertices[idx], vertices[idx + 1], vertices[idx + 2]), scale);
    }

    //TODO create second array with normal + uv

    // duplicate 
    for (int i = 1; i < scene::MAX_BLOCK_COUNT; ++i) {
        memcpy(&elements[i * _nelements], &elements[0], _nelements * sizeof(short));
        memcpy(&vertices[i * 2 * _nvertices], &vertices[0], _nvertices * sizeof(glm::ivec2));
    }

    _buffer_elem = base::create_buffer<unsigned short>(_nelements * scene::MAX_BLOCK_COUNT, 0, &elements[0]);
    _buffer_pos = base::create_buffer<glm::ivec2>(_nvertices * scene::MAX_BLOCK_COUNT, 0, &vertices[0]);
    //_buffer_nor_uv = base::create_buffer<glm::ivec4>(_nvertices * scene::MAX_BLOCK_COUNT, 0, &vertices[0]);

    // create texture buffer for vertices
    glGenTextures(1, &_tb_pos);
    glBindTexture(GL_TEXTURE_BUFFER, _tb_pos);
    glTexBuffer(
        GL_TEXTURE_BUFFER,
        base::get_pfd(base::PF_RG32F)->_internal,
        _buffer_pos);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    if (_tex_mode != BenchTexNone)
        create_textures(loc);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_textures(const base::source_location &)
{
    const int width = 64;
    const int tex_size = 64 * 64;
    glm::u8vec4 * data = 0;
    const int ntex = 32768;
    const base::pixelfmt pf = base::PF_BGRA8_SRGB;
    const int tex_size_bytes = tex_size * base::get_pfd(pf)->_size;

    const GLuint buf = base::create_buffer<u8vec4>(tex_size * ntex, &data);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf);
    
    for (int i = 0; i < ntex; ++i) {
        gen_texture(data + i * tex_size, width, 16, i);
    }

    if (_tex_mode != BenchTexArray) {
        for (int i = 0; i < ntex; ++i) {
            const GLint tex = create_texture(
                width,
                width,
                pf,
                (void*)(i * tex_size_bytes),
                buf);

            _texs.push_back(tex);
        }

        for (int i = 0; i < ntex; ++i) {
            const GLuint64 handle = glGetTextureHandleARB(_texs[i]);
            _tex_handles.push_back(handle);
            glMakeTextureHandleResidentARB(handle);
        }
    }
    else {
        for (int i = 0; i < ntex; i += _max_array_layers) {
            const GLint tex = create_texture_array(
                width,
                width,
                _max_array_layers,
                pf,
                (void*)(i * tex_size_bytes),
                buf);
            _texs.push_back(tex);

            glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    if (_tex_mode == BenchTexBindless) {
        _buffer_tex_handles = base::create_buffer<GLuint64>(ntex, 0, &_tex_handles[0]);

        // create texture buffer for vertices
        glGenTextures(1, &_tb_tex_handles);
        glBindTexture(GL_TEXTURE_BUFFER, _tb_tex_handles);
        glTexBuffer(
            GL_TEXTURE_BUFFER,
            base::get_pfd(base::PF_RG32F)->_internal,
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

	glm::vec4 * const planes_end = planes +num_planes;

	const mat4 *bbox = &_bboxes[0];
	const vec3 *hw = &_hws[0];
	unsigned int *flags = &_flags[0];
	const unsigned int *e = &_flags[0] + _flags.size();

    const bool dont_check = true;

	memset(ctx->_num_visible_blocks, 0, sizeof(ctx->_num_visible_blocks));

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
			++ctx->_num_visible_blocks[*flags & TypeMask];
		}
	}

	return ctx->_num_visible_blocks[0];
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::upload_blocks_to_gpu(
	const base::source_location &loc,
	base::frame_context *ctx)
{
    static int counter = 0;

	ctx->_scene_data_ptr_size = 0;
	for(int i = 0; i < NumTypes; ++i)
		ctx->_scene_data_ptr_size += ctx->_num_visible_blocks[i];
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

        int * const drawid = ctx->_drawid_data_ptr + ctx->_drawid_data_offset;

		// transform visible blocks to screen and add to VBO 
        for (; tm != e; ++tm, ++flags) {
            const char type = char(*flags & TypeMask);
            const uint offset = ptr[type] - ctx->_scene_data_ptr;

            ptr[type]->_tm = *tm;

            drawid[offset * 4] = ctx->_scene_data_offset + offset;
            drawid[offset * 4 + 1] = _nvertices * offset;
            drawid[offset * 4 + 2] = offset;

            new (cmd + offset) base::cmd(
                _nelements,
                1,
                _nelements * offset,
                0,
                ctx->_scene_data_offset + offset);

            ptr[type]++;
        }
	}

    ctx->_ctx_data_ptr->_mvp = ctx->_mvp;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::bind_texture(int counter)
{
    //if ((counter & 0x7) != 0)
    //    return;

    //glUniformHandleui64ARB(_prg_tex, _tex_handles[counter]);
    
    if (_tex_mode == BenchTexNaive) {
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, _texs[counter]);
    }
    else if (_tex_mode == BenchTexArray && (counter & 0x7ff) == 0) {
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, _texs[counter >> 11]);
    }
    
    //const int cm = counter % 190;
    //if (cm == 0)
    //    glBindTextures(2, 190, &_texs[counter]);
    //glUniform1i(_prg_tex, cm + 2);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::gpu_draw(base::frame_context * const ctx)
{
	glUseProgram(_prg);

    //glPolygonMode(GL_FRONT, GL_LINE);

    base::hptimer timer;

    timer.start();

    // SET FRAME RENDER STATES

    bool fast_drawcall = false;
    bool fast_drawcall_old_way = false;
    bool fast_draw_call_gl33 = false;
    bool use_instancing = false;

    glVertexAttribI4i(13, 0, 0, 0, 0);
    glVertexAttribI1i(14, 0);
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
        glVertexAttribI1i(13, ctx->_scene_data_offset);
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
    case BenchTexBufvsVBO:
        break;
    case BenchProceduralVertices:
        break;
    }

    bool use_tex = false;

    switch (_tex_mode) {
    case BenchTexNaive:
        glUniform1i(_prg_tex, 2);
        use_tex = true;
        break;
    case BenchTexArray:
        glUniform1i(_prg_tex, 2);
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D_ARRAY, _texs[0]);
        use_tex = true;
        break;
    case BenchTexBindless:
        glUniform1i(_prg_tex, 2);
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_BUFFER, _tb_tex_handles);
        break;
    }

    glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

    const int count = _nelements;
    const uint nblocks = ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1];

    if (nblocks == 0)
        return;
    
    if (!fast_drawcall) {
        if (use_instancing) {
            glDrawElementsInstanced(
                GL_TRIANGLES,
                count,
                GL_UNSIGNED_SHORT,
                0,
                ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1]);
        }
        else {
            glMultiDrawElementsIndirect(
                GL_TRIANGLES,
                GL_UNSIGNED_SHORT,
                (void*)((ctx->_scene_data_offset) * sizeof(base::cmd)),
                ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1],
                0);
        }
    }
    else {
        int counter = 0;
        int offset = 0;
        const int offset_e = ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1];
        while (offset != offset_e) {
            if (fast_drawcall_old_way) {
                if (use_tex)
                    bind_texture(counter);
                glVertexAttribI1i(14, offset);
                glDrawElements(
                    GL_TRIANGLES,
                    count,
                    GL_UNSIGNED_SHORT,
                    0);
            }
            else if (fast_draw_call_gl33) {
                if (use_tex)
                    bind_texture(counter);
                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    count,
                    GL_UNSIGNED_SHORT,
                    0,
                    offset << 12);
            }
            else {
                if (use_tex)
                    bind_texture(counter);
                glDrawElementsInstancedBaseInstance(
                    GL_TRIANGLES,
                    count,
                    GL_UNSIGNED_SHORT,
                    0,
                    1,
                    offset);
            }
            offset++;
            counter++;
        }
    }

    glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);

    ctx->_cpu_render_time = timer.elapsed_time();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_test_scene()
{
    for (int i = 0; i < 32; ++i)
	    add_test_block();

	/*const int grid_size = BUILDING_SIDE_SIZE;
	const glm::vec3 pillar_size(0.25f, 2.6f, 0.25f);
	const glm::vec3 box_size(3.0f, 0.2f, 4.0f);

	// create floor's pillars
	for(int z = 0; z < grid_size; ++z)
		for(int y = 0; y < (grid_size >> 1); ++y)
			for(int x = 0; x < grid_size; ++x) {
				if(z < grid_size-1) 
					add_block(
						0, glm::vec3(x * 3, 0.2 + z * 2.8, y * 4), pillar_size, 0);
				add_block(
					1, glm::vec3(x * 3, z * 2.8, y * 4), box_size, 0);
			}*/
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_test_scene(unsigned short obj_count)
{
	
	
	/*
	std::vector<bool> height_map;
	voxel_plane.resize(grid_size*grid_size, true);

	unsigned short obj_to_place = obj_count >> 1;
	short level = 0;
	int num_plane_falses = 0;
	// lower half
	while (obj_to_place && (num_plane_falses < grid_size * grid_size) && (level < grid_size/2))
	{
		for (int z = 0; z < grid_size; z++) {
			for (int x = 0; x < grid_size; x++) {
				if (voxel_plane[z*grid_size + x]  && base::rndNomalized() <= prob) {
					obj_to_place--;
					add_block(base::rndFromInterval(0, 1), glm::vec3(x, level, z), box_size, 0);
					if (obj_to_place == 0) {
						x = grid_size;
						z = grid_size;
					}
				}
				else {
					num_plane_falses += (voxel_plane[z*grid_size + x]) ? 1 : 0;
					voxel_plane[z*grid_size + x] = false;
				}
			}
		}
		level++;
	}
	level = -1;
	while (obj_to_place) {
		for (int z = 0; z < grid_size; z++) {
			for (int x = 0; x < grid_size; x++) {
					obj_to_place--;
					add_block(base::rndFromInterval(0, 1), glm::vec3(x, level, z), box_size, 0);
					if (obj_to_place == 0) {
						z = grid_size;
						x = grid_size;
					}
			}
		}
		level--;
	}

	//upper half
	voxel_plane.resize(0, true);
	voxel_plane.resize(grid_size2, true);
	obj_to_place = obj_count >> 1;
	level = grid_size;
	num_plane_falses = 0;
	
	while (obj_to_place && (num_plane_falses < grid_size * grid_size) && (level > grid_size / 2))
	{
		for (int z = 0; z < grid_size; z++) {
			for (int x = 0; x < grid_size; x++) {
				if (voxel_plane[z*grid_size + x] && base::rndNomalized() <= prob) {
					obj_to_place--;
					add_block(base::rndFromInterval(0,1), glm::vec3(x, level, z), box_size, 0);
					if (obj_to_place == 0) {
						x = grid_size;
						z = grid_size;
					}
				}
				else {
					num_plane_falses += (voxel_plane[z*grid_size + x]) ? 1 : 0;
					voxel_plane[z*grid_size + x] = false;
				}
			}
		}
		level--;
	}
	level = grid_size;
	while (obj_to_place) {
		for (int z = 0; z < grid_size; z++) {
			for (int x = 0; x < grid_size; x++) {
				obj_to_place--;
				add_block(base::rndFromInterval(0, 1), glm::vec3(x, level, z), box_size, 0);
				if (obj_to_place == 0) {
					z = grid_size;
					x = grid_size;
				}
			}
		}
		level++;
	}
	*/
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::add_test_block()
{
	const int grid_size = 16;
	const int grid_size2 = grid_size * grid_size;
	int max_height = 5;

	const glm::vec3 box_size(2.0f, 2.0f, 2.0f);
	std::vector<int> height_map;
	height_map.resize(grid_size2, 0);
    
    const float grid_size_r = 1.f / float(grid_size);

    // bottom layer
	for (int y = 0; y < grid_size; y++) {
		for (int x = 0; x < grid_size; x++) {
            const float s0 = glm::simplex(glm::vec2(x, y) * grid_size_r);
            const float s1 = glm::simplex(glm::vec2(x, y) * 2.f * grid_size_r);
			const int height = int(((((s0 + .5f * s1) + 1.5f) / 3.f) * max_height));

			add_block(
                glm::vec3(
                    (_cur_next_block.x * grid_size) + x * 2.f,
                    height * 2.f,
                    (_cur_next_block.y * grid_size) + y * 2.f),
                box_size,
                0);
			}
		}

    // top layer ?
	for (int y = 0; y < grid_size; y++) {
		for (int x = 0; x < grid_size; x++) {
            const glm::vec2 pos0 = glm::vec2(x, y) * grid_size_r;
            const glm::vec2 pos1 = glm::vec2(x, y) * 2.f * grid_size_r;
			int height = (int)((((glm::simplex(pos0) + .5f * glm::simplex(pos1)) + 1.5f) / 3.f) * max_height);
			add_block(
                glm::vec3(
                    (_cur_next_block.x*grid_size) + x * 2.f,
                    32.f - height * 2.f,
                    (_cur_next_block.y*grid_size) + y * 2.f),
                box_size,
                0);
		}
	}
	

	// set next block coords
	if (_cur_next_block.x == 0){
		_cur_next_block.x = -2;
	}
	else if (_cur_next_block.x == -2){
		_cur_next_block.x = 2;
	}
	else if (_cur_next_block.x == 2){
		_cur_next_block.x = 0;
		_cur_next_block.y -= 2;
	}

	get_perspective_block_bound(32);

};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int scene::get_perspective_block_bound(int depth){
	float fovx = glm::atan(glm::tan(_app->get_fovy())*_app->get_aspect());
	return 0;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
