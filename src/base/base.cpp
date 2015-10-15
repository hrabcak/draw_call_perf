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

#include "base.h"
#include "tga_utils.h"

#include <sys/stat.h>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>

#include "gl/glew.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void read_glsl_file(
	const base::source_location &loc,
	const char* filename,
	std::string &str)
{
    struct _stat st;
    if(0 != ::_stat(filename, &st))
		throw base::exception(loc.to_str())<<"Cannot find GLSL source file \""<<filename<<"\"!";

    unsigned int size = (unsigned int)st.st_size;

	str.resize(str.size() + size + 1);

    const int file = _open(filename, _O_BINARY | _O_RDONLY);

	if(file==-1)
		throw base::exception(loc.to_str())<<"Cannot open GLSL source file \""<<filename<<"\"!";
    
	if(_read(file, &str[0], st.st_size)!=st.st_size)
		throw base::exception(loc.to_str())<<"File read error on GLSL source file \""<<filename<<"\"!";
    
	_close(file);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::read_file(
	const base::source_location &loc,
	const char* filename,
	std::vector<unsigned char> &data)
{
    struct _stat st;
    if(0 != ::_stat(filename, &st))
		throw base::exception(loc.to_str())<<"Cannot find file \""<<filename<<"\"!";

	data.resize(st.st_size);

    int file = _open(filename, _O_BINARY | _O_RDONLY);

	if(file==-1)
		throw base::exception(loc.to_str())<<"Cannot open file \""<<filename<<"\"!";
    
	if(_read(file, &data[0], st.st_size)!=st.st_size)
		throw base::exception(loc.to_str())<<"File read error on file \""<<filename<<"\"!";
    
	_close(file);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_and_compile_shader(
	const base::source_location &loc,
    const std::string &cfg,
	const std::string &filename,
	const GLuint type)
{
	std::string src;
	
	read_glsl_file(loc, filename.c_str(),src);

	GLuint shader = glCreateShader(type);
	GLint len = GLint(src.length());
	const char *psrc=src.c_str();

	std::cout<<"Compiling shader \""<<filename<<"\"..."<<std::endl;

    glShaderSource(shader, 1, &psrc, &len);
    glCompileShader(shader);

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(!status) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		std::string str;
		str.resize(len + 1);

        glGetShaderInfoLog(shader, len+1, &len, &str[0]);

        glDeleteShader(shader);
        shader = 0;

		throw base::exception(loc.to_str()) << str;
    }
	return shader;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_program(
	const GLuint vs,
	const GLuint gs,
	const GLuint fs)
{
	assert(vs != 0 && fs != 0);

	GLuint prg = glCreateProgram();
   
	glAttachShader(prg, vs); 
	if(gs) glAttachShader(prg, gs); 
	glAttachShader(prg, fs);

	return prg;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::link_program(
	const base::source_location &loc,
	const GLuint prg)
{
	glLinkProgram(prg);

    GLint status;

	glGetProgramiv(prg, GL_LINK_STATUS, &status);

	if(!status) {
	    GLsizei len;
		glGetProgramiv(prg, GL_INFO_LOG_LENGTH, &len);

		std::string str;
		str.resize(len+1);
		glGetProgramInfoLog(prg, len, &len, &str[0]);

		glDeleteProgram(prg);

		throw base::exception(loc.to_str()) << str;
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLint base::get_uniform_location(
	const base::source_location &loc,
	const GLuint prg,
	const char *name)
{
	const GLint param_loc = glGetUniformLocation(prg, name);
	if(param_loc == -1)
		throw base::exception(loc.to_str())
			<< "Cannot find shader uniform parameter \"" << name << "\"!";
	return param_loc;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_texture(
	const int width, 
	const int height,
	const base::pixelfmt pf,
	const void *data,
	const unsigned buffer)
{
	const base::pfd* pfd=base::get_pfd(pf);

    GLuint handle;
	glGenTextures(1, &handle);

	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(pfd->_compressed) {
		const int pitch = (width >> 2) * pfd->_size;
		const int rows = height >> 2;
		glCompressedTexImage2D(
			GL_TEXTURE_2D,
			0,
			pfd->_internal,
			width,
			height,
			0,
			pitch * rows,
			buffer ? 0 : data);
	}
	else {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			pfd->_internal,
			width,
			height,
			0,
			pfd->_format,
			pfd->_type,
			buffer ? 0 : data);
	}

    glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
    
    return handle;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_texture_from_file(
	const base::source_location &loc,
	const char *filename,
    bool srgb)
{
	std::vector<unsigned char> data;

	base::read_file(SRC_LOCATION, filename, data);

	const base::tga_header *hdr = reinterpret_cast<const base::tga_header*>(&data[0]);

	if(hdr->bits!=32 || hdr->imagetype!=2)
		throw base::exception(loc.to_str())
			<< "Unsupported TGA file format \""<<filename<<"\"!";

	GLuint handle = create_texture(
		hdr->width,
		hdr->height,
		srgb ? PF_BGRA8_SRGB : PF_BGRA8,
		hdr+1);

	return handle;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint __vbo=0;

void base::set_attr0_vbo_amd_wa()
{
	if(__vbo==0) {
		glGenBuffers(1, &__vbo);
		glBindBuffer(GL_ARRAY_BUFFER, __vbo);
		glBufferData(GL_ARRAY_BUFFER,16384*4,0,GL_STATIC_DRAW);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, __vbo);
	}

	glVertexAttribPointer(0,4,GL_BYTE,GL_FALSE,0,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glEnableVertexAttribArray(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::clear_attr0_vbo_amd_wa()
{
	glDisableVertexAttribArray(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int base::rndFromInterval(int min, int max){
	int result = rand();
	result %= (max - min + 1);
	return result + min;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

float base::rndNomalized()
{
	return rand()/(float)RAND_MAX;
}

base::config& base::cfg() { static config ___c; return ___c; }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*
float snoise(glm::vec2 v)
{
	const glm::vec4 C = glm::vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
		0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
		-0.577350269189626,  // -1.0 + 2.0 * C.x
		0.024390243902439); // 1.0 / 41.0
	// First corner
	glm::vec2 i = glm::floor(v + glm::dot(v, glm::vec2(C.y)));
	glm::vec2 x0 = v - i + glm::dot(i, glm::vec2(C.x));

	// Other corners
	glm::vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? glm::vec2(1.0, 0.0) : glm::vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	glm::vec4 x12 = glm::vec4(x0.x, x0.y, x0.x, x0.y) + glm::vec4(C.x, C.x, C.z, C.z);
	x12.x -= i1.x;
	x12.y -= i1.y;

	// Permutations
	i = glm::mod289(i); // Avoid truncation effects in permutation
	glm::vec3 p = glm::permute(glm::permute(i.y + glm::vec3(0.0, i1.y, 1.0))
		+ i.x + glm::vec3(0.0, i1.x, 1.0));

	glm::vec3 m = glm::max(0.5f - glm::vec3(glm::dot(x0, x0), glm::dot(glm::vec2(x12), glm::vec2(x12)), glm::dot(glm::vec2(x12.z, x12.w), glm::vec2(x12.z, x12.w))), 0.0);
	m = m*m;
	m = m*m;

	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

	glm::vec3 x = 2.0f * glm::fract(p * glm::vec3(C.w)) - 1.0f;
	glm::vec3 h = glm::abs(x) - 0.5f;
	glm::vec3 ox = glm::floor(x + 0.5f);
	glm::vec3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159f - 0.85373472095314f * (a0*a0 + h*h);

	// Compute final noise value at P
	glm::vec3 g;
	g.x = a0.x  * x0.x + h.x  * x0.y;
	g.y = a0.y * x12.x + h.y * x12.y;
	g.z = a0.z * x12.z + h.z * x12.w;
	return 130.0 * glm::dot(m, g);
}
*/
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

