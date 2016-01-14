#pragma once

#include <string>
#include <fstream>
#include <glm/glm.hpp>

class obj_writer
{
public:
	obj_writer();
	~obj_writer();

	bool open(const std::string & file_name);
	void close();

	void write_vertex(const glm::vec3 & vertex);
	void write_vertex(float x, float y, float z);
	
	void write_normal(const glm::vec3 & normal);
	void write_normal(float x, float y, float z);

	void write_tex_coords(const glm::vec2 & uv);
	void write_tex_coords(float u, float v);

	void write_face_idx_from_zero(int v1, int vn1, int vt1,
		int v2, int vn2, int vt2,
		int v3, int vn3, int vt3);
	void write_face(int v1, int vn1, int vt1,
		int v2, int vn2, int vt2,
		int v3, int vn3, int vt3);

private:
	bool _is_opened;
	std::ofstream _ofs;
};

