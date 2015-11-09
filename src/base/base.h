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

#ifndef __ASYNC_VBO_TRANSFERS_BASE_H__
#define __ASYNC_VBO_TRANSFERS_BASE_H__

#include "pixelfmt.h"
#include "hptimer.h"
#include "types.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>

#include <gl/glew.h>

#define TMP_STR_BUF_SIZE 64

namespace base {

class app;

struct stats_data
{
    uint64 _buffer_mem;
    uint64 _texture_mem;
    uint64 _ntriangles;
    uint64 _nvertices;
    uint64 _ndrawcalls;
    float _cpu_time;
    float _gpu_time;

    stats_data()
        : _buffer_mem(0)
        , _texture_mem(0)
        , _ntriangles(0)
        , _nvertices(0)
        , _ndrawcalls(0)
        , _cpu_time(0)
        , _gpu_time(0)
    {}

    void operator += (const stats_data &s)
    {
        _buffer_mem = s._buffer_mem;
        _texture_mem = s._texture_mem;
        _ntriangles += s._ntriangles;
        _nvertices += s._nvertices;
        _ndrawcalls += s._ndrawcalls;
    }
};

struct stats_data2{
    int _nframes;
    float _fps;
	double _cpu_render_time;
	double _gpu_render_time;
	//float _dc_per_sec;
	//float _tri_per_sec;
	//ushort _count;

	stats_data2()
        : _nframes(0)
		, _fps(0.0f)
		, _cpu_render_time(0.0f)
		, _gpu_render_time(0.0f)
		//_dc_per_sec(0.0f),
		//_tri_per_sec(0.0f),
		//_count(0)
	{}

    /// @time in seconds
	void mean(const float time)
    {
        _fps = float(_nframes) / time;
		//_fps /= _count;
		_cpu_render_time /= time;
		_gpu_render_time /= time;
		//_dc_per_sec /= _count;
		//_tri_per_sec /= _count;
		//_count = 1;
	}
};

inline stats_data& stats() { static stats_data s; return s; }

///
class source_location
{
public:

	source_location(
		const char *function_name,
		const char *file_name,
		const int line_number)
		: _function_name(function_name)
		, _file_name(file_name)
		, _line_number(line_number)
	{}

	std::string to_str() const {
		char line[TMP_STR_BUF_SIZE];
		return std::string(_function_name)+"@"+_file_name+'('+_itoa(_line_number,line,10)+") ";
	}

	const char* function_name() const { return _function_name; }
	const char* file_name() const { return _file_name; }
	int line_number() const { return _line_number; }

private:
	const char* const _function_name;
	const char* const _file_name;
	const int _line_number;

	source_location(const source_location&);
	void operator = (const source_location&);
};

///
class exception
{
public:
    exception(const char* text) : _text(text) {}
    exception(const std::string &text) : _text(text) {}
    exception() : _text("unknown exception") {}
	
	~exception() { _text+="\n"; }

    exception& operator << (const char *czstr) { _text+=czstr; return *this; }
    exception& operator << (const std::string& str) { _text+=str; return *this; }
    exception& operator << (char c) { _text+=c; return *this; }

    exception& operator << (const int i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_itoa(i,tmp,10); return *this; }
    exception& operator << (const unsigned int i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_ultoa(i,tmp,10); return *this; }
    exception& operator << (const unsigned long i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_ultoa(i,tmp,10); return *this; }
    exception& operator << (const float f) { char tmp[TMP_STR_BUF_SIZE]; sprintf(tmp,"%f",f); _text+=tmp; return *this; }

    const std::string& text() const { return _text; }

protected:
    std::string _text;
};

/// pixel format descriptor
struct pfd {
	int _internal;			//< OpenGL internal format (GL_RGBA8)
	int _format;			//< OpenGL format (GL_RGBA)
	int _type;				//< OpenGL component type (GL_UNSIGNED_BYTE)
	unsigned int _size;		//< byte size of one pixel (4)
	int _components;		//< number of components (4)
	bool _rt;				//< true if it can be used as render target
	int _sRGB;				//< sRGB pixel format alternative
	const char *_txt;		//< readable description
	bool _compressed;		//< true if it is compressed format
};

/// 
extern const pfd ___pfds[];

/// return pointer to pixel format descriptor
inline const pfd* get_pfd(const pixelfmt pf) { assert(pf>PF_UNKNOWN && pf<PF_LAST); return ___pfds+pf; }

/// run app and init OpenGL stuff
void run_app_win(app * const a, const bool initgl = true);

/// read whole file into given std::vector
void read_file(
	const base::source_location &loc,
	const char* filename,
	std::vector<unsigned char> &data);

///
GLuint create_and_compile_shader(
	const base::source_location &loc,
    const std::string &cfg,
	const std::string &filename,
	const GLuint type);

///
GLuint create_program(
	const GLuint vs,
	const GLuint gs,
	const GLuint fs,
    const GLuint cs = 0);

///
void link_program(
	const base::source_location &loc,
	const GLuint prg);

/// simple wrapper for glGetUniformLocation with error reporting
GLint get_uniform_location(
	const base::source_location &loc,
	const GLuint prg,
	const char *name);

///
GLuint create_texture(
	const int width, 
	const int height,
	const base::pixelfmt pf,
	const void * const data,
    const unsigned buffer = 0);

///
GLuint create_texture_storage(
    const int width,
    const int height,
    const base::pixelfmt pf,
    const void * const data,
    const unsigned buffer = 0,
    const bool generate_mips = false);

///
GLuint create_texture_array(
    const int width,
    const int height,
    const int nslices,
    const base::pixelfmt pf,
    const unsigned buffer);

/// The only supported format is TGA 32bit RGBA8 uncompressed
GLuint create_texture_from_file(
	const base::source_location &loc,
	const char *filename,
    bool srgb);

void set_win_title(const char* const str);

/// AMD attr0 workaround stuff
void set_attr0_vbo_amd_wa();
void clear_attr0_vbo_amd_wa();

///
glm::ivec2 get_mouse_pos();
void set_mouse_pos(const glm::ivec2 &pos);

///
void init_opengl_win();

void init_opengl_dbg_win();

void* get_window_handle();

void* get_window_hdc();

void* get_shared_context();

void set_main_rc();

void swap_buffers();

void sleep_ms(const int time);

struct config {
	config()
		: use_debug_context(false)
		, use_debug_sync(false)
		, use_pinned_memory(false)
		, use_nvidia_fast_download(false)
		, use_async_readback(false)

        , test(-1)
        , mesh_size(2)
        , tex_mode(2)
        , tex_freq(1)
        , use_vbo(false)
        , one_mesh(false)
        , tex_size(64)
        , dont_rnd_cubes(false)
		, procedural_scene(false)
		, blades_per_tuft(16)
		, use_instancing(true)
        //, use_nor_uv(false)
    {}

	bool use_debug_context : 1;
	bool use_debug_sync : 1;
	bool use_pinned_memory : 1;
	bool use_nvidia_fast_download : 1;
	bool use_async_readback : 1;
    bool use_vbo : 1;
    bool one_mesh : 1;
    bool dont_rnd_cubes : 1;
	bool procedural_scene : 1;
    //bool use_nor_uv : 1;

    // tests
    int test;
    int mesh_size;
    int tex_mode;
    int tex_freq;
    int tex_size;

	// grass params

	int blades_per_tuft;
	bool use_instancing;
};

config& cfg();

template<class T>
unsigned create_buffer(const unsigned nelem, T** const ptr, void * const data = 0)
{
    GLuint handle = 0;

    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    const unsigned flags = data == 0
        ? GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
        : 0;
    const unsigned size = sizeof(T) * nelem;

    stats()._buffer_mem += size;

    glBufferStorage(GL_ARRAY_BUFFER, size, data, flags);

    if (ptr != 0) {
        *ptr = reinterpret_cast<T*>(
            glMapBufferRange(GL_ARRAY_BUFFER, 0, size, flags));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return handle;
}

inline glm::ivec2 pack_to_pos3x21b(const glm::dvec3 &pos, const double scale)
{
    const glm::ivec3 pos21(pos * scale);
    return glm::ivec2((pos21.x << 11) | ((pos21.y >> 10) & 0x7ff),
        ((pos21.y & 0x3ff) << 21) | (pos21.z & 0x1fffff));
}

inline glm::vec3 unpack_from_pos3x21b(const glm::ivec2 &pos, const float scale)
{
    return glm::vec3(
        pos.x >> 11,
        ((pos.x << 21) >> 11) | (pos.y >> 21),
        (pos.y << 11) >> 11) * scale;
}

inline glm::ivec2 pack_to_norm2x16b_uv2x15b(const glm::dvec3 &norm, const glm::dvec2 &uv, const double scale_norm, const double scale_uv) {
	const glm::ivec3 norm16(norm*scale_norm);
	const glm::ivec2 uv15(uv*scale_uv);

	return glm::ivec2(norm16.x << 16 | (norm16.y & 0xffff), (uv15.x << 17) | ((uv15.y << 2) & 0x1fffc) | ((unsigned int)(norm16.z & 0x80000000) >> 31));
}

inline void unpack_from_norm2x16b_uv2x15b(const glm::ivec2 &norm_uv, const float scale_norm, const float scale_uv, glm::vec3 &norm_out, glm::vec2 &uv_out) {
	norm_out = glm::vec3(norm_uv.x >> 16, (norm_uv.x << 16) >> 16, 0.0f);
	norm_out *= scale_norm;
	norm_out.z = (norm_uv.y & 0x1) ? -1.0f*(glm::sqrt(1.0f - glm::length2(norm_out))) : glm::sqrt(1.0f - glm::length2(norm_out));
	uv_out = glm::vec2(norm_uv.y >> 17, (norm_uv.y << 15) >> 17)*scale_uv;
}

class rnd_int
{
	uint32 _uval;

public:
	void seed(uint32 useed) {
		_uval = useed;
		rand3();
	}

	uint32 rand()         { return rand0(); }

	uint32 rand0()        { return _uval = 3141592653UL * _uval + 1; }
	uint32 rand1()        { return _uval = 3267000013UL * _uval + 1; }
	uint32 rand2()        { return _uval = 3628273133UL * _uval + 1; }
	uint32 rand3()        { return _uval = 3367900313UL * _uval + 1; }

	void nrand(uint32 n, uint32 *puval) {
		uint32 utv = _uval;
		for (uint32 i = 0; i<n; ++i) {
			utv = 3141592653UL * utv + 1;
			puval[i] = utv;
		}
		_uval = utv;
	}

	uint32 get_old() const        { return _uval; }


	static uint32 get_multiplier()    { return 3141592653UL; }

	static rnd_int * init(uint32 useed) {
		rnd_int *ornd = new rnd_int(useed);
		return ornd;
	}

	rnd_int(uint32 sd = 0)
	{
		if (sd == 0)
			sd = (uint32)::time(0);
		seed(sd);
	}
};

int rndFromInterval(rnd_int & rnd,int min, int max);
float rndNomalized(rnd_int & rnd);

} // end of namespace base

#define SRC_LOCATION base::source_location(__FUNCTION__,__FILE__,__LINE__)

#define SRC_LOCATION_STR base::source_location(__FUNCTION__,__FILE__,__LINE__).to_str()

#endif // __ASYNC_VBO_TRANSFERS_BASE_H__
