#ifndef __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__
#define __ASYNC_VBO_TRANSFERS_CUBE_GEN_H__
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define CUBE_FACE_COUNT			6


typedef unsigned short ushort;
typedef unsigned int uint32;

namespace gen_cube_name{

struct vert{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
};

struct triangle_pair_vert_indices{
	ushort t1_i1;
	ushort t1_i2;
	ushort t1_i3;
	ushort t2_i1;
	ushort t2_i2;
	ushort t2_i3;
};


const glm::vec3 v1(-1.0f, -1.0f, 1.0f);
const glm::vec3 v2(1.0f, -1.0f, 1.0f);
const glm::vec3 v3(1.0f, 1.0f, 1.0f);
const glm::vec3 v4(-1.0f, 1.0f, 1.0f);

const glm::vec3 v5(-1.0f, -1.0f, -1.0f);
const glm::vec3 v6(1.0f, -1.0f, -1.0f);
const glm::vec3 v7(1.0f, 1.0f, -1.0f);
const glm::vec3 v8(-1.0f, 1.0f, -1.0f);

const glm::vec3& near_v1 = v1;
const glm::vec3& near_v2 = v2;
const glm::vec3& near_v3 = v3;
const glm::vec3& near_v4 = v4;

const glm::vec3& right_v1 = v2;
const glm::vec3& right_v2 = v6;
const glm::vec3& right_v3 = v7;
const glm::vec3& right_v4 = v3;

const glm::vec3& far_v1 = v6;
const glm::vec3& far_v2 = v5;
const glm::vec3& far_v3 = v8;
const glm::vec3& far_v4 = v7;

const glm::vec3& left_v1 = v5;
const glm::vec3& left_v2 = v1;
const glm::vec3& left_v3 = v4;
const glm::vec3& left_v4 = v8;

const glm::vec3& bottom_v1 = v2;
const glm::vec3& bottom_v2 = v1;
const glm::vec3& bottom_v3 = v5;
const glm::vec3& bottom_v4 = v6;

const glm::vec3& top_v1 = v7;
const glm::vec3& top_v2 = v8;
const glm::vec3& top_v3 = v4;
const glm::vec3& top_v4 = v3;

const glm::vec3 normals[6] = { glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0) };
};

void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count) {
	vert_count = (1 << tess_level) + 1; // vert per edge
	element_count = (((vert_count - 1) * (vert_count - 1)) << 1) * CUBE_FACE_COUNT * 3;
	vert_count *= vert_count*CUBE_FACE_COUNT;
}

void gen_cube( uint32 tess_level, float * vert_data, ushort * index_array ){ 
	uint32 vertices_count_per_edge = (1 << tess_level) + 1;;
	uint32 vertices_count_per_face = vertices_count_per_edge * vertices_count_per_edge;
	uint32 triangle_count_per_face_2 = ((vertices_count_per_edge - 1) * (vertices_count_per_edge - 1));
	
	uint32 max_idx = vertices_count_per_edge - 1;
	
#ifdef _DEBUG
	float * vert_data_end = vert_data + (vertices_count_per_face*CUBE_FACE_COUNT)*(sizeof(gen_cube_name::vert) / sizeof(float));
	ushort * index_array_end = index_array + (triangle_count_per_face_2*CUBE_FACE_COUNT)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort));
#endif

	glm::vec3 dx = (gen_cube_name::near_v2 - gen_cube_name::near_v1) / (vertices_count_per_edge - 1.0f);
	glm::vec3 dy = (gen_cube_name::near_v4 - gen_cube_name::near_v1) / (vertices_count_per_edge - 1.0f);
	glm::vec3 start_point = gen_cube_name::near_v1;
	gen_cube_name::vert new_vertex;
	gen_cube_name::triangle_pair_vert_indices indices;
	new_vertex.norm = glm::vec3(0.0, 0.0, 1.0);

	// tesselate near
	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float)) >= vert_data);
#endif
			memcpy(vert_data + (v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort((v + 1)*vertices_count_per_edge + u);
				
				indices.t2_i1 = ushort(v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort((v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort((v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (v*(vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (v*(vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif
				memcpy(index_array + (v*(vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices)/sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}
	
	// tesselate right
	dx = (gen_cube_name::right_v2 - gen_cube_name::right_v1) / (vertices_count_per_edge - 1.0f);
	dy = (gen_cube_name::right_v4 - gen_cube_name::right_v1) / (vertices_count_per_edge - 1.0f);
	start_point = gen_cube_name::right_v1;
	new_vertex.norm = glm::vec3(1.0, 0.0, 0.0);

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (vertices_count_per_face + v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float))< vert_data_end);
			assert(vert_data + (vertices_count_per_face + v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float))>= vert_data);
#endif


			memcpy(vert_data + (vertices_count_per_face + v*vertices_count_per_edge + u)*(sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				
				
				indices.t2_i1 = ushort(vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u)*(sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}
	
	// tesselate far
	dx = (gen_cube_name::far_v2 - gen_cube_name::far_v1) / (vertices_count_per_edge - 1.0f);
	dy = (gen_cube_name::far_v4 - gen_cube_name::far_v1) / (vertices_count_per_edge - 1.0f);
	start_point = gen_cube_name::far_v1;
	new_vertex.norm = glm::vec3(0.0, 0.0, -1.0);

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (2 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (2 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) >= vert_data);
#endif

			memcpy(vert_data + (2 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(2 * vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(2 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(2 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				
				
				indices.t2_i1 = ushort(2 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(2 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(2 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (2 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (2 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (2 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}

	// tesselate left
	dx = (gen_cube_name::left_v2 - gen_cube_name::left_v1) / (vertices_count_per_edge - 1.0f);
	dy = (gen_cube_name::left_v4 - gen_cube_name::left_v1) / (vertices_count_per_edge - 1.0f);
	start_point = gen_cube_name::left_v1;
	new_vertex.norm = glm::vec3(-1.0, 0.0, 0.0);

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (3 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (3 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) >= vert_data);
#endif

			memcpy(vert_data + (3 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(3 * vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(3 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(3 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				
				
				indices.t2_i1 = ushort(3 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(3 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(3 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (3 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (3 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (3 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}

	// tesselate top
	dx = (gen_cube_name::top_v2 - gen_cube_name::top_v1) / (vertices_count_per_edge - 1.0f);
	dy = (gen_cube_name::top_v4 - gen_cube_name::top_v1) / (vertices_count_per_edge - 1.0f);
	start_point = gen_cube_name::top_v1;
	new_vertex.norm = glm::vec3(0.0, 1.0, 0.0);

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (4 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (4 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) >= vert_data);
#endif

			memcpy(vert_data + (4 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(4 * vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(4 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(4 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				
				
				indices.t2_i1 = ushort(4 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(4 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(4 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (4 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (4 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (4 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}
	
	// tesselate bottom
	dx = (gen_cube_name::bottom_v2 - gen_cube_name::bottom_v1) / (vertices_count_per_edge - 1.0f);
	dy = (gen_cube_name::bottom_v4 - gen_cube_name::bottom_v1) / (vertices_count_per_edge - 1.0f);
	start_point = gen_cube_name::bottom_v1;
	new_vertex.norm = glm::vec3(0.0, -1.0, 0.0);

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (5 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (5 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)) >= vert_data);
#endif

			memcpy(vert_data + (5 * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen_cube_name::vert) / sizeof(float)), &new_vertex, sizeof(gen_cube_name::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(5 * vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(5 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(5 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				
				
				indices.t2_i1 = ushort(5 * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(5 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(5 * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);
				

#ifdef _DEBUG
				assert(index_array + (5 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (5 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (5 * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen_cube_name::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen_cube_name::triangle_pair_vert_indices));
			}
		}
	}
} 

#endif