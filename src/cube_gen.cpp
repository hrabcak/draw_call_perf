#include "cube_gen.h"
#include "base/utils.h"

#include <map>
#include <vector>
#include <set>
#include <algorithm>

#define CUBE_FACE_COUNT			6

typedef std::vector < glm::u8vec3 >		VertMaskVec;
typedef std::map< uint32, uint32 >		VertIdxHashMap;
typedef std::multimap<uint32, uint32>	VertTriangleHashMap;

namespace gen {
	const glm::vec3 origin(-1.0f, -1.0f, 1.0f);
	const glm::vec3 diag(2.0f, 2.0f, -2.0f);

	const glm::i8vec3 v1(0, 0, 0);
	const glm::i8vec3 v2(1, 0, 0);
	const glm::i8vec3 v3(1, 1, 0);
	const glm::i8vec3 v4(0, 1, 0);

	const glm::i8vec3 v5(0, 0, 1);
	const glm::i8vec3 v6(1, 0, 1);
	const glm::i8vec3 v7(1, 1, 1);
	const glm::i8vec3 v8(0, 1, 1);

	const glm::i8vec3* faces[6][4] = { { &v1,&v2,&v3,&v4 },{ &v2,&v6,&v7,&v3 },{ &v6,&v5,&v8,&v7 },{ &v5,&v1,&v4,&v8 },{ &v2,&v1,&v5,&v6 },{ &v7,&v8,&v4,&v3 } };
}



void generate_rand_corner(ushort edge_vert_max_idx, glm::u8vec3 & out_corner) {
	out_corner.x = (ushort)util::rndFromInterval(0, 1) * edge_vert_max_idx;
	out_corner.y = (ushort)util::rndFromInterval(0, 1) * edge_vert_max_idx;
	out_corner.z = (ushort)util::rndFromInterval(0, 1) * edge_vert_max_idx;
}

uint32 add_vert_to_lists(uint32 vert_hash, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map, const glm::u8vec3 & vert) {
	VertIdxHashMap::iterator find_result = vert_hash_map.find(vert_hash);

	if (find_result == vert_hash_map.end()) {
		vert_hash_map[vert_hash] = vert_vec.size();
		vert_vec.push_back(vert);
		return vert_vec.size() - 1;
	}
	else {
		return find_result->second;
	}
}

uint32 get_vert_idx_from_hash(ushort tess_level, uint32 vert_hash, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map) {
	VertIdxHashMap::iterator find_result = vert_hash_map.find(vert_hash);
	ushort edge_vert_count = 2 + tess_level;

	if (find_result == vert_hash_map.end()) {
		ushort x = (vert_hash & 0xFF);
		ushort y = ((vert_hash >> 8) & 0xFF);
		ushort z = ((vert_hash >> 16) & 0xFF);

		glm::u8vec3 vert(x,y,z);

		return add_vert_to_lists(vert_hash, vert_vec, vert_hash_map, vert);
	}
	else {
		return find_result->second;
	}
}

uint32 u8vec3_to_hash(const glm::u8vec3 & vert_mask) {
	uint32 result = 0;
	result |= (vert_mask.x & 0xFF) | ((vert_mask.y & 0xFF) << 8) | ((vert_mask.z & 0xFF) << 16);
	return result;
}

void u8vec3_to_vec3(ushort tess_level, const glm::u8vec3 & in_vec, glm::vec3 & out_vec) {
	ushort edge_vert_count = tess_level + 2;
	float step = 1.0 / (edge_vert_count - 1);
	out_vec.x = gen::origin.x + in_vec.x * step * gen::diag.x;
	out_vec.y = gen::origin.y + in_vec.y * step * gen::diag.y;
	out_vec.z = gen::origin.z + in_vec.z * step * gen::diag.z;
}

bool tri_same(uint32 tri_idx1, uint32 tri_idx2, std::vector<ushort> &index_array) {
	uint32 t1[3];
	uint32 t2[3];
	t1[0] = index_array[tri_idx1];
	t1[1] = index_array[tri_idx1 + 1];
	t1[2] = index_array[tri_idx1 + 2];
	t2[0] = index_array[tri_idx2];
	t2[1] = index_array[tri_idx2 + 1];
	t2[2] = index_array[tri_idx2 + 2];

	auto cmp = [](const void * v1, const void * v2)->int {
		if ((*(uint32*)v1) == (*(uint32*)v2)) {
			return 0;
		}

		if ((*(uint32*)v1) < (*(uint32*)v2)) {
			return -1;
		}

		return 1;
	};

	qsort(t1, 3, sizeof(uint32), cmp);
	qsort(t2, 3, sizeof(uint32), cmp);

	if (t1[0] == t2[0] && t1[1] == t2[1] && t1[2] == t2[2]) {
		return true;
	}

	return false;
} 

void deform_corners(ushort tess_level, VertIdxHashMap & vert_idx_map, VertMaskVec & vert_vec, VertTriangleHashMap & vert_tri_hash_map, std::vector<ushort> &index_array) {
	std::set<uint32> used_corners_hashes;

	ushort edge_vert_max_idx = 1 + tess_level;

	glm::u8vec3 c1, c2;
	uint32 c1_hash, c2_hash;
	for (ushort i = 0; i < 4; i++) {
		
		if (util::rndFromInterval(0, 1) == 0) {
			continue;
		}
		
		do {
			generate_rand_corner(edge_vert_max_idx, c1);
			c1_hash = u8vec3_to_hash(c1);
		} while (used_corners_hashes.find(c1_hash) != used_corners_hashes.end());

		for (ushort j = 0; j < 3; j++) {
			c2 = c1;
			(*(&c2.x + j)) = (*(&c1.x + j) == 0) ? edge_vert_max_idx : 0;

			c2_hash = u8vec3_to_hash(c2);

			if (used_corners_hashes.find(c2_hash) == used_corners_hashes.end()) {
				used_corners_hashes.insert(c1_hash);
				used_corners_hashes.insert(c2_hash);

				glm::i8vec3 d = glm::i8vec3(glm::vec3(edge_vert_max_idx/2.0f, edge_vert_max_idx / 2.0f, edge_vert_max_idx / 2.0f) - glm::vec3(vert_vec[vert_idx_map[c1_hash]]));
				d.x = (glm::sign(d.x));
				d.y = (glm::sign(d.y));
				d.z = (glm::sign(d.z));
				(*(&d.x + j)) = 0;

				if ((*(&c1.x + j)) > (*(&c2.x + j))) {
					c1 = c2;
				}

				for (ushort k = 0; k <= edge_vert_max_idx; k++) {
					c1_hash = u8vec3_to_hash(c1);
					glm::u8vec3 v;
					v.x = c1.x + d.x;
					v.y = c1.y + d.y;
					v.z = c1.z + d.z;
					
					c2_hash = u8vec3_to_hash(v);
					
					uint32 idx1 = get_vert_idx_from_hash(tess_level, c1_hash, vert_vec, vert_idx_map);
					uint32 idx2 = get_vert_idx_from_hash(tess_level, c2_hash, vert_vec, vert_idx_map);

					std::pair <std::multimap<uint32, uint32>::iterator, std::multimap<uint32, uint32>::iterator> rng_iter =  vert_tri_hash_map.equal_range(c1_hash);
					for (std::multimap<uint32, uint32>::iterator it = rng_iter.first; it != rng_iter.second; it++) {
						uint32 tri_idx = it->second;
						vert_tri_hash_map.insert(std::pair<uint32,uint32>(c2_hash,tri_idx));

						if (index_array[tri_idx] == idx1) {
							index_array[tri_idx] = idx2;
						}else if (index_array[tri_idx + 1] == idx1) {
							index_array[tri_idx + 1] = idx2;
						}
						else if (index_array[tri_idx + 2] == idx1) {
							index_array[tri_idx + 2] = idx2;
						}
					}
					
					if (k == 0 || k == edge_vert_max_idx) {
						rng_iter = vert_tri_hash_map.equal_range(c2_hash);

						std::vector<uint32> all_tris_of_vert;

						for (std::multimap<uint32, uint32>::iterator it = rng_iter.first; it != rng_iter.second; it++) {
							all_tris_of_vert.push_back(it->second);
						}

						for (int ft = 0; ft < all_tris_of_vert.size() - 1; ft++) {
							for (int st = ft + 1; st < all_tris_of_vert.size(); st++) {
								if (tri_same(all_tris_of_vert[ft], all_tris_of_vert[st], index_array)) {
									index_array[all_tris_of_vert[ft]] = 0;
									index_array[all_tris_of_vert[ft] + 1] = 0;
									index_array[all_tris_of_vert[ft] + 2] = 0;
									index_array[all_tris_of_vert[st]] = 0;
									index_array[all_tris_of_vert[st] + 1] = 0;
									index_array[all_tris_of_vert[st] + 2] = 0;
								}
							}
						}
					}

					vert_tri_hash_map.erase(c1_hash);

					(*(&c1.x + j))++;
				}

				break;
			}
		}
	}
}

void tesselate_face2(uint32 tess_level, ushort face_ord, std::vector<ushort> &index_array, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map, VertTriangleHashMap & vert_tri_hash_map) {
	auto i8v_mul = [](glm::i8vec3 & v, ushort s)->glm::i8vec3 {
		return glm::i8vec3(v.x*s, v.y*s, v.z*s);
	};

	ushort edge_vert_count = 2 + tess_level;
	glm::i8vec3 origin = *gen::faces[face_ord][0];
	origin = i8v_mul(origin, edge_vert_count - 1);
	glm::i8vec3 dx = *gen::faces[face_ord][1] - *gen::faces[face_ord][0];
	glm::i8vec3 dy = *gen::faces[face_ord][3] - *gen::faces[face_ord][0];

	glm::i8vec3 vert_hash_vec;
	uint32 vert_hash;

	uint32 v1_idx;
	uint32 v2_idx;
	uint32 v3_idx;
	uint32 v4_idx;

	gen::triangle_pair_vert_indices indices;

	for (ushort y = 0; y < edge_vert_count - 1; y++) {
		for (ushort x = 0; x < edge_vert_count - 1; x++) {
			vert_hash_vec = origin + i8v_mul(dx, x) + i8v_mul(dy, y);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v1_idx = get_vert_idx_from_hash(tess_level, vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32,uint32>(vert_hash,index_array.size()));

			vert_hash_vec = origin + i8v_mul(dx, x + 1) + i8v_mul(dy, y);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v2_idx = get_vert_idx_from_hash(tess_level, vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size()));
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size()+3));

			vert_hash_vec = origin + i8v_mul(dx, x + 1) + i8v_mul(dy, y + 1);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v3_idx = get_vert_idx_from_hash(tess_level, vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size() + 3));

			vert_hash_vec = origin + i8v_mul(dx, x) + i8v_mul(dy, y + 1);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v4_idx = get_vert_idx_from_hash(tess_level, vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size()));
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size() + 3));

			index_array.push_back(v1_idx);
			index_array.push_back(v2_idx);
			index_array.push_back(v4_idx);


			index_array.push_back(v2_idx);
			index_array.push_back(v3_idx);
			index_array.push_back(v4_idx);
		}
	}
}

void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count) {
	vert_count = 2 + tess_level; // vert per edge
	element_count = (((vert_count - 1) * (vert_count - 1)) << 1) * CUBE_FACE_COUNT * 3;
	vert_count = element_count;
}

void gen_cube(uint32 tess_level, float * vert_data, ushort * index_array, uint32 & element_count, uint32 & vert_count) {
	srand(time(NULL));

	VertMaskVec vertex_list;
	VertIdxHashMap vertex_hash_map;
	VertTriangleHashMap vert_tri_hash_map;
	std::vector<ushort> index_vec;
	for (ushort i = 0; i < CUBE_FACE_COUNT; i++) {
		tesselate_face2(tess_level, i, index_vec, vertex_list, vertex_hash_map,vert_tri_hash_map);
	}

	deform_corners(tess_level, vertex_hash_map, vertex_list,vert_tri_hash_map,index_vec);

	gen::vert vert1;
	gen::vert vert2;
	gen::vert vert3;
	glm::vec3 norm;

	float norm_sign;



	gen::triangle_pair_vert_indices indices;

	uint32 cur_pos = 0;

	for (uint32 i = 0; i < index_vec.size(); i += 3) {
		if (index_vec[i] == 0 && index_vec[i + 1] == 0 && index_vec[i + 2] == 0) {
			continue;
		}

		indices.t1_i1 = cur_pos;
		indices.t1_i2 = cur_pos + 1;
		indices.t1_i3 = cur_pos + 2;

		

		u8vec3_to_vec3(tess_level,vertex_list[index_vec[i]], vert1.pos);
		u8vec3_to_vec3(tess_level, vertex_list[index_vec[i + 1]], vert2.pos);
		u8vec3_to_vec3(tess_level, vertex_list[index_vec[i + 2]], vert3.pos);

		norm = glm::cross(vert2.pos - vert1.pos, vert3.pos - vert1.pos);
		
			norm = glm::normalize(norm);



			vert1.norm = norm;
			vert2.norm = norm;
			vert3.norm = norm;

			norm_sign = glm::sign(norm.x + norm.y + norm.z);

			if (glm::abs(norm.x) > 0.0000001) {
				vert1.uv.x = (vert1.pos.z + norm_sign) / (2.0f * norm_sign);
				vert1.uv.y = (vert1.pos.y + norm_sign) / (2.0f * norm_sign);

				vert2.uv.x = (vert2.pos.z + norm_sign) / (2.0f * norm_sign);
				vert2.uv.y = (vert2.pos.y + norm_sign) / (2.0f * norm_sign);

				vert3.uv.x = (vert3.pos.z + norm_sign) / (2.0f * norm_sign);
				vert3.uv.y = (vert3.pos.y + norm_sign) / (2.0f * norm_sign);
			}
			else if (glm::abs(norm.y) > 0.0000001) {
				vert1.uv.x = (vert1.pos.x + norm_sign) / (2.0f * norm_sign);
				vert1.uv.y = (vert1.pos.z + norm_sign) / (2.0f * norm_sign);

				vert2.uv.x = (vert2.pos.x + norm_sign) / (2.0f * norm_sign);
				vert2.uv.y = (vert2.pos.z + norm_sign) / (2.0f * norm_sign);

				vert3.uv.x = (vert3.pos.x + norm_sign) / (2.0f * norm_sign);
				vert3.uv.y = (vert3.pos.z + norm_sign) / (2.0f * norm_sign);
			}
			else {
				vert1.uv.x = (vert1.pos.x + norm_sign) / (2.0f * norm_sign);
				vert1.uv.y = (vert1.pos.y + norm_sign) / (2.0f * norm_sign);

				vert2.uv.x = (vert2.pos.x + norm_sign) / (2.0f * norm_sign);
				vert2.uv.y = (vert2.pos.y + norm_sign) / (2.0f * norm_sign);

				vert3.uv.x = (vert3.pos.x + norm_sign) / (2.0f * norm_sign);
				vert3.uv.y = (vert3.pos.y + norm_sign) / (2.0f * norm_sign);
			}
		
		memcpy(vert_data + cur_pos * (sizeof(gen::vert) / sizeof(float)), &vert1, sizeof(gen::vert));
		memcpy(vert_data + (cur_pos + 1) * (sizeof(gen::vert) / sizeof(float)), &vert2, sizeof(gen::vert));
		memcpy(vert_data + (cur_pos + 2) * (sizeof(gen::vert) / sizeof(float)), &vert3, sizeof(gen::vert));
		memcpy(index_array + cur_pos, &indices, 3 * sizeof(ushort));

		cur_pos += 3;
	}

	element_count = cur_pos;
	vert_count = cur_pos;
}

