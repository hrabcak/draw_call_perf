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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <glm/gtx/verbose_operator.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

const bool uniform_block = false;
const bool use_bindless_tex = true;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::scene()
	: _tms()
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

    , _bench_mode(BenchNaive)
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
	const int type,
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

	bbox[3] = vec4(pos + vec3(0,half_size.y,0),1);
	_bboxes.push_back(bbox);

	_hws.push_back(size * 0.5f);
	_flags.push_back(type & TypeMask);

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
        break;
    case BenchTexBufvsVBO:
        break;
    case BenchProceduralVertices:
        break;
    
    case BenchNaiveTextures:
        cfg += "#define USE_NAIVE_TEX 1\n";
        break;
    case BenchTexArray:
        cfg += "#define USE_ARRAY_TEX 1\n";
        break;
    case BenchBindless:
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

    case BenchNaiveTextures:
    case BenchTexArray:
        _prg_tex = get_uniform_location(loc, _prg, "mat_tex");
        break;
    case BenchBindless:
        _prg_tex = get_uniform_location(loc, _prg, "tb_tex_handles");
        break;
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::init_gpu_stuff(const base::source_location &loc)
{
    load_and_init_shaders(loc);

    const int tess_level = 2;

    get_face_and_vert_count_for_tess_level(tess_level, _nelements, _nvertices);

    std::vector<unsigned short> elements;
    std::vector<float> vertices;
    elements.resize(_nelements * scene::MAX_BLOCK_COUNT);
    vertices.resize(_nvertices * 8 * scene::MAX_BLOCK_COUNT);

	unsigned int dummy; // toto som dolpnil len aby sa to dalo skompilovat

    gen_cube(tess_level, &vertices[0], &elements[0],dummy,dummy);

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

    if (_bench_mode >= BenchNaiveTextures)
        create_textures(loc);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_textures(const base::source_location &)
{
    const int tex_size = 64 * 64;
    glm::u8vec4 * data = 0;

    const GLuint buf = base::create_buffer<u8vec4>(tex_size * 32768, &data);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf);
    
    for (int i = 0; i < 32768; ++i) {
        gen_texture(data, 64, 16, i);

        const GLint tex = create_texture(
            64,
            64,
            base::PF_BGRA8_SRGB,
            (void*)(i * tex_size * 4), 
            buf);

        _texs.push_back(tex);

        const GLuint64 handle = glGetTextureHandleARB(tex);

        _tex_handles.push_back(handle);

        glMakeTextureHandleResidentARB(handle);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    _buffer_tex_handles = base::create_buffer<GLuint64>(32768, 0, &_tex_handles[0]);

    // create texture buffer for vertices
    glGenTextures(1, &_tb_tex_handles);
    glBindTexture(GL_TEXTURE_BUFFER, _tb_tex_handles);
    glTexBuffer(
        GL_TEXTURE_BUFFER,
        base::get_pfd(base::PF_RG32F)->_internal,
        _buffer_tex_handles);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
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
	return p*(1.0f/length(p.swizzle(X,Y,Z)));
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

void scene::gpu_draw(base::frame_context * const ctx)
{
	glUseProgram(_prg);

    //glPolygonMode(GL_FRONT, GL_LINE);

    base::hptimer timer;

    timer.start();

    /*
        Benchmark modes

        1: OpenGL 3.3 glDrawElementsBaseVertex + std texture binding
        2: Instancing for efficiency comparison
        3: Indirect OpenGL 4.3
        4: glDrawElementsBaseInstance + bindless textures    
    */


    // SET FRAME RENDER STATES

    const bool fast_drawcall = true;
    const bool fast_drawcall_old_way = false;
    const bool fast_draw_call_gl33 = true;
    const bool use_instancing = false;

    glVertexAttribI1i(13, 0);
    glBindBufferRange(
        GL_UNIFORM_BUFFER,
        _prg_ctx,
        ctx->_ctx_vbo,
        ctx->_ctx_id * sizeof(base::ctx_data),
        sizeof(base::ctx_data));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_elem);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ctx->_cmd_vbo);

	// bind canvas elements texture buffer
    if (!uniform_block) {
        glUniform1i(_prg_tb_blocks, 0);
        glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_BUFFER, ctx->_scene_tb);

        glUniform1i(_prg_tb_pos, 1);
        glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_BUFFER, _tb_pos);

        glUniform1i(_prg_tex, 2);
        glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_BUFFER, _tb_tex_handles);
    }

    if ((fast_drawcall && fast_drawcall_old_way)
        || (fast_drawcall && fast_draw_call_gl33)
        || (!fast_drawcall && use_instancing)
        || uniform_block) {
        glDisableVertexAttribArray(13);
    }

    glQueryCounter(ctx->_time_queries[0], GL_TIMESTAMP);

    const int count = _nelements;
    const unsigned vbo = ctx->_scene_vbo;
    const uint nblocks = ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1];

    if (nblocks == 0)
        return;
    
    if (!uniform_block) {
        int offset = ctx->_scene_data_offset;

        if (!fast_drawcall) {
            if (use_instancing) {
                glVertexAttribI1i(13, offset);
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
            const int offset_e = offset + ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1];
            while (offset != offset_e) {
                if (fast_drawcall_old_way) {
                    glVertexAttribI1i(13, offset);
                    glDrawElements(
                        GL_TRIANGLES,
                        count,
                        GL_UNSIGNED_SHORT,
                        0);
                }
                else if (fast_draw_call_gl33) {
                    /*if (counter % 190 == 0) {
                        glBindTextures(0, 190, &_texs[counter]);
                    }
                    glUniform1i(_prg_tex, counter % 190);*/
                    //glUniformHandleui64ARB(_prg_tex, _tex_handles[counter]);
                    glDrawElementsBaseVertex(
                        GL_TRIANGLES,
                        count,
                        GL_UNSIGNED_SHORT,
                        0,
                        offset << 12);
                }
                else {
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
    }
    else {
        unsigned offset = (ctx->_scene_data_offset) * sizeof(base::block_data);
        const unsigned offset_e = offset
            + (ctx->_num_visible_blocks[0] + ctx->_num_visible_blocks[1])
            * sizeof(base::block_data);
        while (offset != offset_e) {
            glBindBufferRange(GL_UNIFORM_BUFFER, _prg_tb_blocks, vbo, offset, 64);
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
            offset += sizeof(base::block_data);
        }
    }

    glQueryCounter(ctx->_time_queries[1], GL_TIMESTAMP);

    ctx->_cpu_render_time = timer.elapsed_time();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_test_scene()
{
	const int grid_size = BUILDING_SIDE_SIZE;
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
			}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
