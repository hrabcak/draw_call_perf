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
#include "pci_c.h"

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

    const size_t offset = str.size();

	str.resize(str.size() + size + 1);

    const int file = _open(filename, _O_BINARY | _O_RDONLY);

	if(file==-1)
		throw base::exception(loc.to_str())<<"Cannot open GLSL source file \""<<filename<<"\"!";
    
	if(_read(file, &str[offset], st.st_size)!=st.st_size)
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
	std::string src = cfg;
    	
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

		throw base::exception(loc.to_str()) << ", " << filename << ": " << str;
    }
	return shader;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_program(
    const GLuint vs,
    const GLuint gs,
    const GLuint fs,
    const GLuint cs)
{
    GLuint prg = glCreateProgram();

    if (vs) glAttachShader(prg, vs);
    if (gs) glAttachShader(prg, gs);
    if (fs) glAttachShader(prg, fs);
    if (cs) glAttachShader(prg, cs);

    return prg;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_program(
	const GLuint vs,
	const GLuint gs,
	const GLuint fs,
	const GLuint cs,
	const GLuint tcs,
	const GLuint tes)
{
	GLuint prg = base::create_program(vs, gs, fs, cs);

	if (tcs) glAttachShader(prg, tcs);
	if (tes) glAttachShader(prg, tes);

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
	const void * const data,
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
			data);
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
			data);
	}

    glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,0);
    
    return handle;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_texture_storage(
    const int width,
    const int height,
    const base::pixelfmt pf,
    const void * const data,
    const unsigned buffer,
    const bool generate_mips)
{
    const base::pfd * const pfd = base::get_pfd(pf);

    GLuint handle;
    glGenTextures(1, &handle);

    const int nmips = 1 + int(glm::floor(glm::log2(glm::max(width, height))));

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(
        handle,
        nmips,
        pfd->_internal,
        width,
        height);

    if (data != 0) {
        glTextureSubImage2D(
            handle,
            0,
            0, 0,
            width, height,
            GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
            data);
    }

    base::stats()._texture_mem += width * height * pfd->_size;

    if (generate_mips) {
        glGenerateMipmap(GL_TEXTURE_2D);
        for (int i = 1; i < nmips; ++i)
            base::stats()._texture_mem += glm::max(width >> i, 1) * glm::max(height, 1) * pfd->_size;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    return handle;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLuint base::create_texture_array(
    const int width,
    const int height,
    const int nslices,
    const base::pixelfmt pf,
    const unsigned buffer)
{
    const base::pfd* pfd = base::get_pfd(pf);
    const int nmips = 1 + int(glm::floor(glm::log2(glm::max(width, height))));

    GLuint handle;
    glGenTextures(1, &handle);

    

    if (pfd->_compressed) {
        
    }
    else {
        glTextureStorage3DEXT(
            handle,
            GL_TEXTURE_2D_ARRAY,
            nmips,
            pfd->_internal,
            width,
            height,
            nslices);
    }

    base::stats()._texture_mem += nslices * width * height * pfd->_size;

    // always counting with mips!!!
    for (int i = 1; i < nmips; ++i)
        base::stats()._texture_mem += nslices * glm::max(width >> i, 1) * glm::max(height >> i, 1) * pfd->_size;

	glTextureParameterfEXT(handle, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glTextureParameterfEXT(handle, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameterfEXT(handle, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameterfEXT(handle, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameterfEXT(handle, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

int base::rndFromInterval(rnd_int & rnd, int min, int max){
    glm::uint result = rnd.rand();// &0x0000ffff;
	result %= (max - min + 1);
	return int(result) + min;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

float base::rndNomalized(rnd_int & rnd)
{
	uint32 rand = rnd.rand();
	return (((rand & 0x0000ffff) ^ ((rand & 0xffff0000)>>16) ) / 65535.0f);
}

base::config& base::cfg() { static config ___c; return ___c; }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char * base::ven_dev_id_to_ati_card_name(unsigned short vendor_id, unsigned short device_id){
	int t_len = sizeof(PciDevTable) / sizeof(PCI_DEVTABLE);

	for (int i = 0; i < t_len; i++){
		if (PciDevTable[i].VenId == vendor_id && PciDevTable[i].DevId == device_id){
			return PciDevTable[i].ChipDesc;
		}
	}

	return "";

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
