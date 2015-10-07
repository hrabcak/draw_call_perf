#include "cube_gen.h"

#define CUBE_FACE_COUNT			6

namespace gen{
	const glm::vec3 v1(-1.0f, -1.0f, 1.0f);
	const glm::vec3 v2(1.0f, -1.0f, 1.0f);
	const glm::vec3 v3(1.0f, 1.0f, 1.0f);
	const glm::vec3 v4(-1.0f, 1.0f, 1.0f);

	const glm::vec3 v5(-1.0f, -1.0f, -1.0f);
	const glm::vec3 v6(1.0f, -1.0f, -1.0f);
	const glm::vec3 v7(1.0f, 1.0f, -1.0f);
	const glm::vec3 v8(-1.0f, 1.0f, -1.0f);

	const glm::vec3* faces[6][4] = { {&v1,&v2,&v3,&v4}, {&v2,&v6,&v7,&v3}, {&v6,&v5,&v8,&v7}, {&v5,&v1,&v4,&v8}, {&v2,&v1,&v5,&v6}, {&v7,&v8,&v4,&v3} };
	const glm::vec3 normals[6] = { glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0) };
}

void tesselate_face(uint32 tess_level, ushort face_ord, const glm::vec3* v1, const glm::vec3* v2, const glm::vec3* v3, float * vert_data, ushort * index_array){
	uint32 vertices_count_per_edge = 2 + tess_level;
	uint32 vertices_count_per_face = vertices_count_per_edge * vertices_count_per_edge;
	uint32 triangle_count_per_face_2 = ((vertices_count_per_edge - 1) * (vertices_count_per_edge - 1));

	uint32 max_idx = vertices_count_per_edge - 1;

#ifdef _DEBUG
	float * vert_data_end = vert_data + (vertices_count_per_face*CUBE_FACE_COUNT)*(sizeof(gen::vert) / sizeof(float));
	ushort * index_array_end = index_array + (triangle_count_per_face_2*CUBE_FACE_COUNT)*(sizeof(gen::triangle_pair_vert_indices) / sizeof(ushort));
#endif

	glm::vec3 dx = (*v2 - *v1) / (vertices_count_per_edge - 1.0f);
	glm::vec3 dy = (*v3 - *v1) / (vertices_count_per_edge - 1.0f);
	glm::vec3 start_point = *v1;
	gen::vert new_vertex;
	gen::triangle_pair_vert_indices indices;

	new_vertex.norm = gen::normals[face_ord];

	for (uint32 v = 0; v <= max_idx; v++) {
		if (v > 0) {
			start_point += dy;
		}

		new_vertex.uv.y = v / (float)max_idx;

		for (uint32 u = 0; u <= max_idx; u++) {
			new_vertex.pos = start_point + ((float)u * dx);
			new_vertex.uv.x = u / (float)max_idx;

#ifdef _DEBUG
			assert(vert_data + (face_ord * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen::vert) / sizeof(float)) < vert_data_end);
			assert(vert_data + (face_ord * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen::vert) / sizeof(float)) >= vert_data);
#endif

			memcpy(vert_data + (face_ord * vertices_count_per_face + v * vertices_count_per_edge + u) * (sizeof(gen::vert) / sizeof(float)), &new_vertex, sizeof(gen::vert));

			if (v < max_idx && u < max_idx){
				indices.t1_i1 = ushort(face_ord * vertices_count_per_face + v*vertices_count_per_edge + u);
				indices.t1_i2 = ushort(face_ord * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t1_i3 = ushort(face_ord * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);


				indices.t2_i1 = ushort(face_ord * vertices_count_per_face + v*vertices_count_per_edge + u + 1);
				indices.t2_i2 = ushort(face_ord * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u + 1);
				indices.t2_i3 = ushort(face_ord * vertices_count_per_face + (v + 1)*vertices_count_per_edge + u);


#ifdef _DEBUG
				assert(index_array + (face_ord * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen::triangle_pair_vert_indices) / sizeof(ushort)) < index_array_end);
				assert(index_array + (face_ord * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen::triangle_pair_vert_indices) / sizeof(ushort)) >= index_array);
#endif

				memcpy(index_array + (face_ord * triangle_count_per_face_2 + v * (vertices_count_per_edge - 1) + u) * (sizeof(gen::triangle_pair_vert_indices) / sizeof(ushort)), &indices, sizeof(gen::triangle_pair_vert_indices));
			}
		}
	}

}

void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count) {
	vert_count = 2 + tess_level; // vert per edge
	element_count = (((vert_count - 1) * (vert_count - 1)) << 1) * CUBE_FACE_COUNT * 3;
	vert_count *= vert_count*CUBE_FACE_COUNT;
}

void gen_cube(uint32 tess_level, float * vert_data, ushort * index_array){
	for (ushort i = 0; i < CUBE_FACE_COUNT; i++){
		tesselate_face(tess_level, i, gen::faces[i][0], gen::faces[i][1], gen::faces[i][3],vert_data,index_array);
	}
}

