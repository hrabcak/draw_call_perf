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
    uint32 _buffer_mem;
    uint32 _texture_mem;
    uint32 _ntriangles;
    uint32 _nvertices;
    uint32 _ndrawcalls;

    stats_data()
        : _buffer_mem(0)
        , _texture_mem(0)
        , _ntriangles(0)
        , _nvertices(0)
        , _ndrawcalls(0)
    {}
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
void run_app_win(app *a, const bool initgl=true);

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
	const GLuint fs);

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
	const void *data,
	const unsigned buffer = 0);

GLuint create_texture_array(
    const int width,
    const int height,
    const int nslices,
    const base::pixelfmt pf,
    const void *data,
    const unsigned buffer);

/// The only supported format is TGA 32bit RGBA8 uncompressed
GLuint create_texture_from_file(
	const base::source_location &loc,
	const char *filename,
    bool srgb);

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
	{}

	bool use_debug_context : 1;
	bool use_debug_sync : 1;
	bool use_pinned_memory : 1;
	bool use_nvidia_fast_download : 1;
	bool use_async_readback : 1;
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


int rndFromInterval(int min, int max);
float rndNomalized();


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

	return glm::ivec2(norm16.x << 16 | (norm16.y & 0xffff), (uv15.x << 17) | ((uv15.y << 2) & 0x3fffc) | ((unsigned int)(norm16.z & 0x80000000) >> 31));
}

inline void unpack_from_norm2x16b_uv2x15b(const glm::ivec2 &norm_uv, const float scale_norm, const float scale_uv, glm::vec3 &norm_out, glm::vec2 &uv_out) {
	norm_out = glm::vec3(norm_uv.x >> 16, (norm_uv.x << 16) >> 16, 0.0f);
	norm_out *= scale_norm;
	norm_out.z = (norm_uv.y & 0x1) ? -1.0f*(glm::sqrt(1.0f - glm::length2(norm_out))) : glm::sqrt(1.0f - glm::length2(norm_out));
	uv_out = glm::vec2(norm_uv.y >> 17, (norm_uv.y << 15) >> 17)*scale_uv;
}


} // end of namespace base

#define SRC_LOCATION base::source_location(__FUNCTION__,__FILE__,__LINE__)

#define SRC_LOCATION_STR base::source_location(__FUNCTION__,__FILE__,__LINE__).to_str()

#endif // __ASYNC_VBO_TRANSFERS_BASE_H__
