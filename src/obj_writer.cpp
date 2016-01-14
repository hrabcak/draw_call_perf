#include "obj_writer.h"


obj_writer::obj_writer()
{
}


obj_writer::~obj_writer()
{
	if (_is_opened){
		close();
	}
}

bool obj_writer::open(const std::string & file_name)
{
	_ofs.open(file_name);
	_is_opened = _ofs.is_open();
	return _is_opened;
}

void obj_writer::close()
{
	_ofs.close();
	_is_opened = false;
}

void obj_writer::write_vertex(const glm::vec3 & vertex)
{
	write_vertex(vertex.x, vertex.y, vertex.z);
}

void obj_writer::write_vertex(float x, float y, float z)
{
	if (!_is_opened){
		return;
	}

	_ofs << "v " << x << " " << y << " " << z << std::endl;
}

void obj_writer::write_normal(const glm::vec3 & normal)
{
	write_normal(normal.x, normal.y, normal.z);
}

void obj_writer::write_normal(float x, float y, float z)
{
	if (!_is_opened){
		return;
	}

	_ofs << "vn " << x << " " << y << " " << z << std::endl;
}

void obj_writer::write_tex_coords(const glm::vec2 & uv)
{
	write_tex_coords(uv.x, uv.y);
}

void obj_writer::write_tex_coords(float u, float v)
{
	if (!_is_opened){
		return;
	}

	_ofs << "vt " << u << " " << v << std::endl;
}

void obj_writer::write_face_idx_from_zero(int v1, int vn1, int vt1,
	int v2, int vn2, int vt2,
	int v3, int vn3, int vt3)
{
	write_face(v1 + 1, vn1 + 1, vt1 + 1,
		v2 + 1, vn2 + 1, vt2 + 1,
		v3 + 1, vn3 + 1, vt3 + 1);
}

void obj_writer::write_face(int v1, int vn1, int vt1,
	int v2, int vn2, int vt2,
	int v3, int vn3, int vt3)
{
	if (!_is_opened){
		return;
	}

	_ofs << "f " << v1 << "/" << vn1 << "/" << vt1 << " " <<
		v2 << "/" << vn2 << "/" << vt2 << " " <<
		v3 << "/" << vn3 << "/" << vt3 << std::endl;
}	