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

#include "benchmark.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "base/canvas.h"
#include "scene.h"
#include "scene_grass.h"
#include "scene_buildings.h"
#include "renderer.h"
#include "cube_gen.h"

#include <glm/gtc/matrix_transform.hpp>

#include <sstream>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

benchmark::benchmark()
    : app()
{
    _stats_str.resize(4096);
    memset(&_stats_str[0], 0, _stats_str.size());

	if (base::cfg().sceneType == base::config::stGrass){
		_scene = std::auto_ptr<scene_i>(new scene_grass(this));
	}
	else if (base::cfg().sceneType == base::config::stCubes){
		_scene = std::auto_ptr<scene_i>(new scene(this));
	}
	else if (base::cfg().sceneType == base::config::stBuildings){
		_scene = std::auto_ptr<scene_i>(new scene_buildings(this));
	}
	
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

benchmark::~benchmark() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::start()
{
    char tmp[2048];

    sprintf(
        tmp,
        "%s - Generating scene with %u unique meshes and textures",
        get_wnd_name(),
        scene::MAX_BLOCK_COUNT);

    base::set_win_title(tmp);

    // start renderer thread
    _renderer.reset(new renderer(this, SRC_LOCATION));
    
    if (!_renderer->is_alive()) {
        shutdown();
        return;
    }
   
    app::start();

    _scene->post_gpu_init();

    base::set_win_title(get_wnd_name());
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::gpu_init()
{
    app::gpu_init();

    _scene->init_gpu_stuff(SRC_LOCATION);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::stop()
{
	_renderer->stop(SRC_LOCATION);
	app::stop();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::draw_frame()
{
	base::frame_context *ctx = 0;
	
	// wait for renderer
	while((ctx = _renderer->pop_frame_context_from_pool()) == 0 && !_shutdown)
        base::sleep_ms(0);

    if (_shutdown)
        return;
	
	assert(ctx != 0);

	app::begin_frame();

	app::create_perspective_matrix(ctx);
	app::update_camera(ctx);

    _scene->update(ctx);

    // prepare stats
    static base::stats_data stats;
    static int nframes = 0;
    static __int64 start_time = 0;
	static int nframes_total = 0;
	static float dtime_total = 0.0f;
	static int test_cycles = 0;

	char mesh_size_str[32];

    if (start_time == 0 && ctx->_time != 0)
        start_time = ctx->_time;

    if (start_time != 0) {
        nframes++;
        stats += ctx->_stats;
        stats._cpu_time += ctx->_stats._cpu_time;
        stats._gpu_time += ctx->_stats._gpu_time;

        const __int64 freq = base::hptimer().freq();
        const __int64 dt = ((ctx->_time - start_time) * 1000000) / freq;
        
        if (dt > 1000000) {
            const float dtf = float(dt) * 0.000001f;
            const float fps = float(nframes) / dtf;
            const float r_nframes = 1.0f / float(nframes);

			
			static uint32 nvert ;
			static uint32 nelem;
			get_face_and_vert_count(base::cfg().mesh_size, nelem, nvert);
			get_mesh_size_str(mesh_size_str,
				nvert,
				nelem);

			if (base::cfg().sceneType == base::config::stGrass)
			{
				sprintf(
					&_stats_str[0],
					"MVtx/s:   %.0f\n"
					"MVtx:     %.3f\n"
					"MTris/s:  %.0f\n"
					"MTris:    %.3f\n"
					"KDraw/s:  %.0f\n"
					"KDraw:    %.3f\n"
					"gpu:      %.3f ms\n"
					"cpu:      %.3f ms\n"
					"fps:      %.0f\n\n"
					"GPU driver: %s\n"
					"GPU: %s\n\n"
					"%s",

					float(stats._nvertices) * 0.000001 / dtf,
					float(stats._nvertices) * 0.000001 / float(nframes),
					float(stats._ntriangles) * 0.000001 / dtf,
					float(stats._ntriangles) * 0.000001 / float(nframes),
					float(stats._ndrawcalls) * 0.001 / dtf,
					float(stats._ndrawcalls) * 0.001 / nframes,
					stats._gpu_time * r_nframes,
					stats._cpu_time * r_nframes,
					fps,
					_renderer->get_gpu_driver_str(),
					_renderer->get_gpu_str(),
					get_test_name());
			}
			else if (base::cfg().sceneType == base::config::stCubes)
			{
				sprintf(
					&_stats_str[0],
					"tex:      %.0f MB\n"
					"buf:      %.0f MB\n"
					"MVtx/s:   %.0f\n"
					"MVtx:     %.3f\n"
					"MTris/s:  %.0f\n"
					"MTris:    %.3f\n"
					"KDraw/s:  %.0f\n"
					"KDraw:    %.3f\n"
					"gpu:      %.3f ms\n"
					"cpu:      %.3f ms\n"
					"fps:      %.0f\n\n"
					"one mesh: %s\n"
					"vertex data: %s\n"
					"average mesh size: %s\n"
					"textures: %ux%u BGRA8\n"
					"tex freq: %u\n"
					"tex mode: %s\n"
					"GPU driver: %s\n"
					"GPU: %s\n\n"
					"%s",

					float(stats._texture_mem) / float(1024 * 1024),
					float(stats._buffer_mem) / float(1024 * 1024),
					float(stats._nvertices) * 0.000001 / dtf,
					float(stats._nvertices) * 0.000001 / float(nframes),
					float(stats._ntriangles) * 0.000001 / dtf,
					float(stats._ntriangles) * 0.000001 / float(nframes),
					float(stats._ndrawcalls) * 0.001 / dtf,
					float(stats._ndrawcalls) * 0.001 / nframes,
					stats._gpu_time * r_nframes,
					stats._cpu_time * r_nframes,
					fps,
					base::cfg().one_mesh ? "true" : "false",
					base::cfg().use_vbo ? "VERTEX BUFFER" : "TEXTURE BUFFER",
					mesh_size_str,
					base::cfg().tex_size, base::cfg().tex_size,
					base::cfg().tex_freq,
					get_texturing_mode_str(base::cfg().tex_mode),
					_renderer->get_gpu_driver_str(),
					_renderer->get_gpu_str(),
					get_test_name());
			}else if (base::cfg().sceneType == base::config::stBuildings)
			{
				sprintf(
					&_stats_str[0],
					"NBuildings:%u\n"
					"Block size:%d\n"
					"MVtx/s:   %.0f\n"
					"MVtx:     %.3f\n"
					"MTris/s:  %.0f\n"
					"MTris:    %.3f\n"
					"KDraw/s:  %.0f\n"
					"KDraw:    %.3f\n"
					"gpu:      %.3f ms\n"
					"cpu:      %.3f ms\n"
					"fps:      %.0f\n\n"
					"GPU driver: %s\n"
					"GPU: %s\n\n"
					"%s",
					base::cfg().buildings_count,
					base::cfg().blocks_per_idc,
					float(stats._nvertices) * 0.000001 / dtf,
					float(stats._nvertices) * 0.000001 / float(nframes),
					float(stats._ntriangles) * 0.000001 / dtf,
					float(stats._ntriangles) * 0.000001 / float(nframes),
					float(stats._ndrawcalls) * 0.001 / dtf,
					float(stats._ndrawcalls) * 0.001 / nframes,
					stats._gpu_time * r_nframes,
					stats._cpu_time * r_nframes,
					fps,
					_renderer->get_gpu_driver_str(),
					_renderer->get_gpu_str(),
					get_test_name());
			}

			if (test_cycles >= 1){
				_test_stats += stats;
				_test_stats._cpu_time += stats._cpu_time;
				_test_stats._gpu_time += stats._gpu_time;
				nframes_total += nframes;
				dtime_total += dtf;
			}

			if (base::cfg().test != -1 && test_cycles == 3){
				if (base::cfg().sceneType == base::config::stGrass) {
					grass_write_test_data_csv(GRASS_TEST_FILE_NAME, _test_stats, dtime_total, nframes_total);
				}
				else if (base::cfg().sceneType == base::config::stCubes){
					write_test_data_csv(CUBES_TEST_FILE_NAME, _test_stats, dtime_total, nframes_total);
				}
				else if (base::cfg().sceneType == base::config::stBuildings){
					buildings_write_test_data_csv(BUILDINGS_TEST_FILE_NAME, _test_stats, dtime_total, nframes_total);
				}
				_shutdown = true;
			}

			start_time = ctx->_time;
			nframes = 0;
            stats = base::stats_data();
			test_cycles++;
        }
        
        //
        _canvas->fill_rect(
            ctx,
            glm::vec2(0),
            glm::vec2(256, 260),
            glm::vec4(0.0, 0.0, 0.0, 0.8));

        _canvas->draw_text(
            ctx,
            glm::vec2(3),
            &_stats_str[0],
            glm::vec4(1, 1, 1, 1),
            _fnt_mono.get());
    }

	_renderer->push_frame_context(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::gpu_draw_frame(base::frame_context * const ctx)
{
    glDisable(GL_BLEND);
    _scene->gpu_draw(ctx);



    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    base::canvas::render(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::key(const int key, const bool down)
{
	app::key(key, down);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char* benchmark::get_test_name() const
{
    return base::cfg().test != -1
        ? _scene->get_test_name(base::cfg().test)
        : "Interactive";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool benchmark::write_test_data_csv(
	const char * file_name,
	const base::stats_data & stats,
	const float time,
	const int nframes,
	bool is_dummy)
{
	FILE * pFile;
	pFile = fopen(file_name, "r+");

	if (pFile == NULL){
		pFile = fopen(file_name, "w");
		if (pFile == NULL){
			return false;
		}

		fputs(
			"test_name,"
			"gpu_gl_name,"
			"use_vbo,"
			"tex_freq,"
			"tex_mode,"
			"mesh_size,"
			"one_mesh,"
			"frames,"
			"render_time,"
			"cpu_render_time (ms),"
			"gpu_render_time (ms),"
			"dc,"
			"ntri (Mtri),"
			"nverts (Mvtx),"
			"buf_mem (MB),"
			"tex_mem (MB),"
			"gpu_vendor,"
			"gpu_drv_ver,"
			"gpu_vendor_id,"
			"gpu_device_id,"
			"gpu_rev_id",
			pFile);
	}
	else{
		fseek(pFile, 0, SEEK_END);
	}

	if (!is_dummy){
	std::ostringstream oss;
	std::string vendor_id("");
	std::string device_id("");
	std::string rev_id("");

	oss << std::hex << _renderer->get_vendor_id();
	vendor_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_device_id();
	device_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_rev_id();
	rev_id = oss.str();
	oss.str("");

	fprintf(
		pFile,
		"\n%s,%s,%s,%i,%u,%i,%s,%u,%f,%f,%f,%u,%llu,%llu,%u,%u,%s,%s,%s,%s,%s",
		this->get_test_name(),
		_renderer->get_gpu_str(),
		base::cfg().use_vbo ? "true" : "false",
		base::cfg().tex_freq,
		base::cfg().tex_mode,
		base::cfg().mesh_size*16,
		base::cfg().one_mesh ? "true" : "false",
		nframes,
		time,
		stats._cpu_time,
		stats._gpu_time,
		stats._ndrawcalls,
		stats._ntriangles,
		stats._nvertices,
		stats._buffer_mem >> 20,
		stats._texture_mem >> 20,
		_renderer->get_gpu_vendor_str(),
		_renderer->get_gpu_driver_str(),
		vendor_id.c_str(),
		device_id.c_str(),
		rev_id.c_str());
	}
	else{
		fprintf(pFile, "\n,,,,,,,,,,,,,,,,,,,,");
	}
	fclose(pFile);

	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool benchmark::grass_write_test_data_csv(
	const char * file_name,
	const base::stats_data & stats,
	const float time,
	const int nframes,
	bool is_dummy)
{
	FILE * pFile;
	pFile = fopen(file_name, "r+");

	if (pFile == NULL){
		pFile = fopen(file_name, "w");
		if (pFile == NULL){
			return false;
		}

		fputs(
			"grass_method,"
			"gpu_gl_name,"
			"frames,"
			"render_time,"
			"cpu_render_time (ms),"
			"gpu_render_time (ms),"
			"dc,"
			"ntri,"
			"use_texture,"
			"pure_color,"
			"use_idx_buf,"
			"tris_per_frame,"
			"geom_blade_out,"
			"in_vtx_per_dc,"
			"use_triangles,"
			"gpu_vendor,"
			"gpu_drv_ver,"
			"gpu_vendor_id,"
			"gpu_device_id,"
			"gpu_rev_id"
			,pFile);
	}
	else{
		fseek(pFile, 0, SEEK_END);
	}

	if (!is_dummy){
	std::ostringstream oss;
	std::string vendor_id("");
	std::string device_id("");
	std::string rev_id("");

	oss << std::hex << _renderer->get_vendor_id();
	vendor_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_device_id();
	device_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_rev_id();
	rev_id = oss.str();
	oss.str("");

	fprintf(
		pFile
		, "\n%s,%s,%u,%f,%f,%f,%llu,%llu,%s,%s,%s,%u,%u,%u,%s,%s,%s,%s,%s,%s"
		, this->get_test_name()
		, _renderer->get_gpu_str()
		, nframes
		, time
		, stats._cpu_time
		, stats._gpu_time
		, stats._ndrawcalls
		, stats._ntriangles
		, base::cfg().use_grass_blade_tex ? "true" : "false"
		, base::cfg().pure_color ? "true" : "false"
		, base::cfg().use_idx_buf ? "true" : "false"
		, base::cfg().ngrass_tiles*base::cfg().blades_per_tuft*base::cfg().tufts_per_tile * 5
		, (base::cfg().proc_scene_type == base::proc_scn_type::psGeometryShader) ? base::cfg().blades_per_geom_run : 0
		, base::cfg().in_vtx_per_dc
		, base::cfg().use_triangles ? "true" : "false"
		, _renderer->get_gpu_vendor_str()
		, _renderer->get_gpu_driver_str()
		, vendor_id.c_str()
		, device_id.c_str()
		, rev_id.c_str()
		);
	}
	else{
		fprintf(pFile, "\n,,,,,,,,,,,,,,,,,,,");
	}
	
	fclose(pFile);

	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool benchmark::buildings_write_test_data_csv(
	const char * file_name,
	const base::stats_data & stats,
	const float time,
	const int nframes,
	bool is_dummy)
{
	FILE * pFile;
	pFile = fopen(file_name, "r+");

	if (pFile == NULL){
		pFile = fopen(file_name, "w");
		if (pFile == NULL){
			return false;
		}

		fputs(
			"gpu_gl_name,"
			"gpu_driver,"
			"frames,"
			"render_time,"
			"cpu_render_time (ms),"
			"gpu_render_time (ms),"
			"dc,"
			"ntri,"
			"blocks_per_tile,"
			"gpu_vendor,"
			"gpu_vendor_id,"
			"gpu_device_id,"
			"gpu_rev_id"
			, pFile);
	}
	else{
		fseek(pFile, 0, SEEK_END);
	}

	if (!is_dummy){
	std::ostringstream oss;
	std::string vendor_id("");
	std::string device_id("");
	std::string rev_id("");

	oss << std::hex << _renderer->get_vendor_id();
	vendor_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_device_id();
	device_id = oss.str();
	oss.str("");

	oss << std::hex << _renderer->get_rev_id();
	rev_id = oss.str();
	oss.str("");

	fprintf(
		pFile
		, "\n%s,%s,%u,%f,%f,%f,%llu,%llu,%d,%s,%s,%s,%s"
		, _renderer->get_gpu_str()
		, _renderer->get_gpu_driver_str()
		, nframes
		, time
		, stats._cpu_time
		, stats._gpu_time
		, stats._ndrawcalls
		, stats._ntriangles
		, base::cfg().blocks_per_idc
		, _renderer->get_gpu_vendor_str()
		, vendor_id.c_str()
		, device_id.c_str()
		, rev_id.c_str()
		);
	}
	else{
		fprintf(pFile, "\n,,,,,,,,,,,,");
	}
	
	fclose(pFile);

	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char * benchmark::get_texturing_mode_str(int mode){
	switch (mode){
	case 0: return "No texture";
		break;
	case 1:	return "Naive";
		break;
	case 2: return "Array";
		break;
	case 3: return "Bindless";
		break;
	default: return "Not valid!";
		break;
	}
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void benchmark::get_mesh_size_str(char * out_str,ushort nvert, ushort nelem){
    sprintf(out_str, "%uF/%uV", nelem / 3, nvert);
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=