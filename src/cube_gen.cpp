#include "cube_gen.h"
#include "base/base.h"

#include <map>
#include <vector>
#include <set>
#include <algorithm>

#define CUBE_FACE_COUNT			6

#define MASK_BIT_0 0x1
#define MASK_BIT_1 0x2
#define MASK_BIT_2 0x4
#define MASK_BIT_3 0x8
#define MASK_BIT_4 0x10
#define MASK_BIT_5 0x20
#define MASK_BIT_6 0x40
#define MASK_BIT_7 0x80

//#define COORD_TO_IDX(X,Y,Z,MAX_COORD)	(X>=0 && Y >=0 && Z >=0 && X < MAX_COORD && Y < MAX_COORD && Z < MAX_COORD) ?(Z*MAX_COORD*MAX_COORD + Y*MAX_COORD + X) : (-1)

short COORD_TO_IDX(short  X, short Y, short Z, short MAX_COORD) {
	return(X >= 0 && Y >= 0 && Z >= 0 && X < MAX_COORD && Y < MAX_COORD && Z < MAX_COORD) ? (Z*MAX_COORD*MAX_COORD + Y*MAX_COORD + X) : (-1);
}

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

	const glm::i8vec3* faces[6][4] = { { &v1, &v2, &v3, &v4 }, { &v2, &v6, &v7, &v3 }, { &v6, &v5, &v8, &v7 }, { &v5, &v1, &v4, &v8 }, { &v2, &v1, &v5, &v6 }, { &v7, &v8, &v4, &v3 } };
}

inline void mask_to_idx(unsigned char mask,unsigned char face_mask, ushort & idx1_out, ushort & idx2_out){
	ushort idx = 0;

	for (unsigned char m = 0x1; m < MASK_BIT_6; m = m << 1){
		if (mask & face_mask){
			idx1_out = idx * 4 + 1;
			idx2_out = idx * 4 + 2;
			return;
		}

		if (mask & m){
			idx++;
		}
	}
}

inline void cpy_unpacked(void * pos_data_ptr, void * norm_uv_data_ptr, const gen::vert & vert_data){
	memcpy(pos_data_ptr, &vert_data, 12);
	memcpy(norm_uv_data_ptr, ((char *)(&vert_data))+12, 20);
}

inline void cpy_packed(void * pos_data_ptr, void * norm_uv_data_ptr, const gen::vert & vert_data){
	int pack[4];
	const float scale_pos = 1048575.0;//(glm::pow(2.0, 20.0) - 1.0);
	const float scale_norm = 32767.0;//(glm::pow(2.0, 15.0) - 1.0);
	const float scale_uv = 16383.0; //(glm::pow(2.0, 14.0) - 1.0);
	(*reinterpret_cast<glm::ivec2*>(&pack[0])) = base::pack_to_pos3x21b(glm::dvec3(vert_data.pos), scale_pos);
	(*reinterpret_cast<glm::ivec2*>(&pack[2])) = base::pack_to_norm2x16b_uv2x15b(glm::dvec3(vert_data.norm), glm::dvec2(vert_data.uv), scale_norm, scale_uv);
	memcpy(pos_data_ptr, &pack[0], 8);
	memcpy(norm_uv_data_ptr, &pack[2], 8);
}


void generate_rand_corner(ushort edge_vert_max_idx, glm::u8vec3 & out_corner) {
	out_corner.x = (uint8)((ushort)base::rndFromInterval(0, 1) * edge_vert_max_idx);
	out_corner.y = (uint8)((ushort)base::rndFromInterval(0, 1) * edge_vert_max_idx);
	out_corner.z = (uint8)((ushort)base::rndFromInterval(0, 1) * edge_vert_max_idx);
}

ushort add_vert_to_lists(uint32 vert_hash, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map, const glm::u8vec3 & vert) {
	VertIdxHashMap::iterator find_result = vert_hash_map.find(vert_hash);

	if (find_result == vert_hash_map.end()) {
		vert_hash_map[vert_hash] = vert_vec.size();
		vert_vec.push_back(vert);
		return (ushort)vert_vec.size() - 1;
	}
	else {
		return (ushort)find_result->second;
	}
}

ushort get_vert_idx_from_hash(uint32 vert_hash, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map) {
	VertIdxHashMap::iterator find_result = vert_hash_map.find(vert_hash);

	if (find_result == vert_hash_map.end()) {
		ushort x = (vert_hash & 0xFF);
		ushort y = ((vert_hash >> 8) & 0xFF);
		ushort z = ((vert_hash >> 16) & 0xFF);

		glm::u8vec3 vert(x, y, z);

		return add_vert_to_lists(vert_hash, vert_vec, vert_hash_map, vert);
	}
	else {
		return (ushort)find_result->second;
	}
}

uint32 u8vec3_to_hash(const glm::u8vec3 & vert_mask) {
	uint32 result = 0;
	result |= (vert_mask.x & 0xFF) | ((vert_mask.y & 0xFF) << 8) | ((vert_mask.z & 0xFF) << 16);
	return result;
}

void u8vec3_to_vec3(ushort tess_level, const glm::u8vec3 & in_vec, glm::vec3 & out_vec) {
	ushort edge_vert_count = tess_level + 2;
	float step = 1.0f / (edge_vert_count - 1);
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

		if (base::rndFromInterval(0, 1) == 0) {
			continue;
		}

		do {
			generate_rand_corner(edge_vert_max_idx, c1);
			c1_hash = u8vec3_to_hash(c1);
		} while (used_corners_hashes.find(c1_hash) != used_corners_hashes.end());

		for (ushort j = 0; j < 3; j++) {
			c2 = c1;
			(*(&c2.x + j)) = (*(&c1.x + j) == 0) ? (uint8)edge_vert_max_idx : (uint8)0;

			c2_hash = u8vec3_to_hash(c2);

			if (used_corners_hashes.find(c2_hash) == used_corners_hashes.end()) {
				used_corners_hashes.insert(c1_hash);
				used_corners_hashes.insert(c2_hash);

				glm::i8vec3 d = glm::i8vec3(glm::vec3(edge_vert_max_idx / 2.0f, edge_vert_max_idx / 2.0f, edge_vert_max_idx / 2.0f) - glm::vec3(vert_vec[vert_idx_map[c1_hash]]));
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

					uint32 idx1 = get_vert_idx_from_hash(c1_hash, vert_vec, vert_idx_map);
					uint32 idx2 = get_vert_idx_from_hash(c2_hash, vert_vec, vert_idx_map);

					std::pair <std::multimap<uint32, uint32>::iterator, std::multimap<uint32, uint32>::iterator> rng_iter = vert_tri_hash_map.equal_range(c1_hash);
					for (std::multimap<uint32, uint32>::iterator it = rng_iter.first; it != rng_iter.second; it++) {
						uint32 tri_idx = it->second;
						vert_tri_hash_map.insert(std::pair<uint32, uint32>(c2_hash, tri_idx));

						if (index_array[tri_idx] == idx1) {
							index_array[tri_idx] = (ushort)idx2;
						}
						else if (index_array[tri_idx + 1] == idx1) {
							index_array[tri_idx + 1] = (ushort)idx2;
						}
						else if (index_array[tri_idx + 2] == idx1) {
							index_array[tri_idx + 2] = (ushort)idx2;
						}
					}

					if (k == 0 || k == edge_vert_max_idx) {
						rng_iter = vert_tri_hash_map.equal_range(c2_hash);

						std::vector<uint32> all_tris_of_vert;

						for (std::multimap<uint32, uint32>::iterator it = rng_iter.first; it != rng_iter.second; it++) {
							all_tris_of_vert.push_back(it->second);
						}

						for (uint32 ft = 0; ft < all_tris_of_vert.size() - 1; ft++) {
							for (uint32 st = ft + 1; st < all_tris_of_vert.size(); st++) {
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

void tesselate_face2(ushort tess_level, ushort face_ord, std::vector<ushort> &index_array, VertMaskVec & vert_vec, VertIdxHashMap & vert_hash_map, VertTriangleHashMap & vert_tri_hash_map) {
	auto i8v_mul = [](glm::i8vec3 & v, ushort s)->glm::i8vec3 {
		return glm::i8vec3(v.x*s, v.y*s, v.z*s);
	};

	ushort edge_vert_count = (ushort)(2 + tess_level);
	glm::i8vec3 origin = *gen::faces[face_ord][0];
	origin = i8v_mul(origin, edge_vert_count - 1);
	glm::i8vec3 dx = *gen::faces[face_ord][1] - *gen::faces[face_ord][0];
	glm::i8vec3 dy = *gen::faces[face_ord][3] - *gen::faces[face_ord][0];

	glm::i8vec3 vert_hash_vec;
	uint32 vert_hash;

	ushort v1_idx;
	ushort v2_idx;
	ushort v3_idx;
	ushort v4_idx;

	for (ushort y = 0; y < edge_vert_count - 1; y++) {
		for (ushort x = 0; x < edge_vert_count - 1; x++) {
			vert_hash_vec = origin + i8v_mul(dx, x) + i8v_mul(dy, y);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v1_idx = get_vert_idx_from_hash(vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size()));

			vert_hash_vec = origin + i8v_mul(dx, x + 1) + i8v_mul(dy, y);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v2_idx = get_vert_idx_from_hash(vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size()));
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size() + 3));

			vert_hash_vec = origin + i8v_mul(dx, x + 1) + i8v_mul(dy, y + 1);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v3_idx = get_vert_idx_from_hash(vert_hash, vert_vec, vert_hash_map);
			vert_tri_hash_map.insert(std::pair<uint32, uint32>(vert_hash, index_array.size() + 3));

			vert_hash_vec = origin + i8v_mul(dx, x) + i8v_mul(dy, y + 1);
			vert_hash = (vert_hash_vec.x & 0xFF) | ((vert_hash_vec.y & 0xFF) << 8) | ((vert_hash_vec.z & 0xFF) << 16);
			v4_idx = get_vert_idx_from_hash(vert_hash, vert_vec, vert_hash_map);
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


void gen_cube_imp(
    ushort tess_level,
    float * pos_data,
    float * norm_uv_data,
    ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
    uint32 & element_count,
    uint32 & vert_count,
    bool use_int)
{
	short voxel_m[4096];
	voxel_info voxel_i[4096];
	short v_idx[4096 * 3];

	const float miss_prob = 0.0f;

	const glm::vec3 norm_right(1.0f, 0.0f, 0.0f);
	const glm::vec3 norm_left(-1.0f, 0.0f, 0.0f);
	const glm::vec3 norm_top(0.0f, 0.0f, 1.0f);
	const glm::vec3 norm_bottom(0.0f, 0.0f, -1.0f);
	const glm::vec3 norm_far(0.0f, 1.0f, 0.0f);
	const glm::vec3 norm_near(0.0f, -1.0f, 0.0f);

	memset(&voxel_m[0], 0, 8192);
	memset(&voxel_i[0], 0, 32768);
	memset(&v_idx[0], 0, 24576);

	const ushort vert_per_edge_count = (ushort)(2 + tess_level);

	voxel_info * v_i;

	ushort idx = -1;

	short idx_left = -1;
	short idx_right = -1;
	short idx_bottom = -1;
	short idx_top = -1;
	short idx_near = -1;
	short idx_far = -1;

	ushort v1_idx;
	ushort v2_idx;
	ushort v3_idx;
	ushort v4_idx;

	ushort cur_vert_count = 0;
	ushort cur_element_count = 0;
	ushort cur_vert_idx_count = 0;

	char * cur_pos_data_pos = (char *)pos_data;
	char * cur_norm_uv_data_pos = (char*)norm_uv_data;
	ushort * cur_index_array_pos = index_array;

	gen::vert vert1;
	gen::vert vert2;
	gen::vert vert3;
	gen::vert vert4;

	gen::triangle_pair_vert_indices indices;

	for (ushort z = 0; z < vert_per_edge_count-1; ++z){
		for (ushort y = 0; y < vert_per_edge_count-1; ++y){
			for (ushort x = 0; x < vert_per_edge_count-1; ++x){
				idx = COORD_TO_IDX(x, y, z, vert_per_edge_count);
				
				// temp voxel miss control

				idx_left = COORD_TO_IDX(x - 1, y, z, vert_per_edge_count);
				idx_right = COORD_TO_IDX(x + 1, y, z, vert_per_edge_count);
				idx_bottom = COORD_TO_IDX(x, y - 1, z, vert_per_edge_count);
				idx_top = COORD_TO_IDX(x, y + 1, z, vert_per_edge_count);
				idx_near = COORD_TO_IDX(x, y, z - 1, vert_per_edge_count);

				if (idx_left >= 0 && idx_right >= 0 && idx_bottom >= 0 && // have all naighbours control
					idx_top >= 0 && idx_near >= 0 && voxel_m[idx_left] >= 0 &&
					voxel_m[idx_right] >= 0 && voxel_m[idx_bottom] >= 0
					&& voxel_m[idx_top] >= 0 && voxel_m[idx_near] >= 0){
				
					voxel_m[idx] = idx;
				}
				else{
					if (base::rndNomalized() <= miss_prob){
						voxel_m[idx] = -1;
					}else{
						voxel_m[idx] = idx;
					}
				}
				////////////////////////// 
			}
		}
	}


	for (ushort z = 0; z < vert_per_edge_count-1; ++z){
		for (ushort y = 0; y < vert_per_edge_count-1; ++y){
			for (ushort x = 0; x < vert_per_edge_count-1; ++x){
				idx = COORD_TO_IDX(x, y, z, vert_per_edge_count);

				if (voxel_m[idx] == -1){
					continue;
				}

				idx_left = COORD_TO_IDX(x - 1, y, z, vert_per_edge_count); // need for face visibility determination
				idx_right = COORD_TO_IDX(x + 1, y, z, vert_per_edge_count);
				idx_bottom = COORD_TO_IDX(x, y - 1, z, vert_per_edge_count);
				idx_top = COORD_TO_IDX(x, y + 1, z, vert_per_edge_count);
				idx_near = COORD_TO_IDX(x, y, z - 1, vert_per_edge_count);
				idx_far = COORD_TO_IDX(x, y, z + 1, vert_per_edge_count);

				ushort cur_idx_offset = 0;

				if (idx_near == -1 || voxel_m[idx_near] == -1){ // if near neigh not exist then process near face
					if (idx_left >= 0 && voxel_m[idx_left] >= 0){// if left neighbour exist reuse his vertices
						v_i = &voxel_i[idx_left];
						mask_to_idx(v_i->mask & 0xff, MASK_BIT_0, v1_idx, v4_idx);
						v1_idx = v_idx[v_i->vert_idx + v1_idx];
						v4_idx = v_idx[v_i->vert_idx + v4_idx];
					}
					else{
						v1_idx = cur_vert_count;
						cur_vert_count++;
						vert1.pos = (gen::origin + (gen::diag*glm::vec3(x/float(vert_per_edge_count),
							y / float(vert_per_edge_count), z / float(vert_per_edge_count))));

						vert1.norm = norm_near;
						vert1.uv.x = x / (float(vert_per_edge_count));
						vert1.uv.y = z / (float(vert_per_edge_count));

						v4_idx = cur_vert_count;
						cur_vert_count++;
						vert4.pos = (gen::origin + (gen::diag*glm::vec3(x / float(vert_per_edge_count),
							y / float(vert_per_edge_count), (z + 1) / float(vert_per_edge_count))));

						vert4.norm = norm_near;
						vert4.uv.x = x / (float(vert_per_edge_count));
						vert4.uv.y = (z + 1) / (float(vert_per_edge_count));

						if (use_int){
							cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert1);
							cur_pos_data_pos += 8;
							cur_norm_uv_data_pos += 8;

							cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert4);
							cur_pos_data_pos += 8;
							cur_norm_uv_data_pos += 8;
						}
						else{
							cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert1);
							cur_pos_data_pos += 12;
							cur_norm_uv_data_pos += 20;

							cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert4);
							cur_pos_data_pos += 12;
							cur_norm_uv_data_pos += 20;
						}
					}

					// these must create for sure

					v_i = &voxel_i[idx];
					v_i->mask |= MASK_BIT_0;
					v_i->vert_idx = cur_vert_idx_count;

					v2_idx = cur_vert_count;
					cur_vert_count++;
					vert2.pos = (gen::origin + (gen::diag*glm::vec3( (x + 1) / float(vert_per_edge_count),
						y / float(vert_per_edge_count), z / float(vert_per_edge_count))));

					vert2.norm = norm_near;
					vert2.uv.x = (x + 1) / (float(vert_per_edge_count));
					vert2.uv.y = z / (float(vert_per_edge_count));

					v3_idx = cur_vert_count;
					cur_vert_count++;
					vert3.pos = (gen::origin + (gen::diag*glm::vec3( (x + 1) / float(vert_per_edge_count),
						y / float(vert_per_edge_count), (z + 1) / float(vert_per_edge_count))));

					vert3.norm = norm_near;
					vert3.uv.x = (x + 1) / (float(vert_per_edge_count));
					vert3.uv.y = (z + 1) / (float(vert_per_edge_count));

					if (use_int){
						cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert2);
						cur_pos_data_pos += 8;
						cur_norm_uv_data_pos += 8;

						cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert3);
						cur_pos_data_pos += 8;
						cur_norm_uv_data_pos += 8;
					}
					else{
						cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert2);
						cur_pos_data_pos += 12;
						cur_norm_uv_data_pos += 20;

						cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert3);
						cur_pos_data_pos += 12;
						cur_norm_uv_data_pos += 20;
					}

					v_idx[v_i->vert_idx + cur_idx_offset * 4] = v1_idx;
					v_idx[v_i->vert_idx + cur_idx_offset * 4 + 1] = v2_idx;
					v_idx[v_i->vert_idx + cur_idx_offset * 4 + 2] = v3_idx;
					v_idx[v_i->vert_idx + cur_idx_offset * 4 + 3] = v4_idx;

					indices.t1_i1 = v1_idx;
					indices.t1_i2 = v2_idx;
					indices.t1_i3 = v3_idx;

					indices.t2_i1 = v1_idx;
					indices.t2_i2 = v3_idx;
					indices.t2_i3 = v4_idx;

					memcpy(cur_index_array_pos, &indices, 12);
					cur_index_array_pos += 6;

					cur_vert_idx_count += 4;
					cur_element_count += 6;
					cur_idx_offset++;
				}

				if (idx_left == -1 || voxel_m[idx_left] == -1){ // if left neigh not exist then process left face
					/*if (idx_near >= 0 && voxel_m[idx_near] >= 0){ // if near neighbour exist  reuse his vertices
						v_i = &voxel_i[idx_near];
						mask_to_idx(v_i->mask & 0xff, MASK_BIT_4, MASK_BIT_7, v2_idx, v3_idx);
						v2_idx = v_idx[v_i->vert_idx + v2_idx];
						v3_idx = v_idx[v_i->vert_idx + v3_idx];
					}

					v_i = &voxel_i[idx];
					v_i->vert_idx = cur_vert_idx_count;
					
					// add v1
					v1_idx = vert_count;
					v_i->mask |= MASK_BIT_4;
					vert_count++;


					// add v4 
					v4_idx = vert_count;
					v_i->mask |= MASK_BIT_7;
					vert_count++;

					cur_vert_idx_count += 4;*/
				}
				
				if (idx_right == -1 || voxel_m[idx_right] == -1){ // if visible then process face

				}

				if (idx_bottom == -1 || voxel_m[idx_bottom] == -1){ // if visible then process face

				}

				if (idx_top == -1 || voxel_m[idx_top] == -1){ // if visible then process face

				}

				if (idx_far == -1 || voxel_m[idx_far] == -1){ // if visible then process face

				}
			}
		}
	}

	element_count = cur_element_count;
	vert_count = cur_vert_count; 

	/*srand((ushort)time(NULL));

	VertMaskVec vertex_list;
	VertIdxHashMap vertex_hash_map;
	VertTriangleHashMap vert_tri_hash_map;
	std::vector<ushort> index_vec;
	for (ushort i = 0; i < CUBE_FACE_COUNT; i++) {
		tesselate_face2(tess_level, i, index_vec, vertex_list, vertex_hash_map, vert_tri_hash_map);
	}

	
	//deform_corners(tess_level, vertex_hash_map, vertex_list, vert_tri_hash_map, index_vec);
	
	gen::vert vert1;
	gen::vert vert2;
	gen::vert vert3;
	glm::vec3 norm;

	float norm_sign;



	gen::triangle_pair_vert_indices indices;

	ushort cur_pos = 0;
	char * cur_pos_data_pos = (char *)pos_data;
	char * cur_norm_uv_data_pos = (char*)norm_uv_data;

	for (uint32 i = 0; i < index_vec.size(); i += 3) {
		if (index_vec[i] == 0 && index_vec[i + 1] == 0 && index_vec[i + 2] == 0) {
			continue;
		}

		indices.t1_i1 = cur_pos;
		indices.t1_i2 = cur_pos + 1;
		indices.t1_i3 = cur_pos + 2;



		u8vec3_to_vec3(tess_level, vertex_list[index_vec[i]], vert1.pos);
		u8vec3_to_vec3(tess_level, vertex_list[index_vec[i + 1]], vert2.pos);
		u8vec3_to_vec3(tess_level, vertex_list[index_vec[i + 2]], vert3.pos);

		norm = glm::cross(vert2.pos - vert1.pos, vert3.pos - vert1.pos);

		norm = glm::normalize(norm);



		vert1.norm = norm;
		vert2.norm = norm;
		vert3.norm = norm;

		norm_sign = glm::sign(norm.x + norm.y + norm.z);
		const float norm_sign_div = 0.5f*norm_sign;

		glm::vec3 v1_uv((vert1.pos+norm_sign)*norm_sign_div);
		glm::vec3 v2_uv((vert2.pos + norm_sign)*norm_sign_div);
		glm::vec3 v3_uv((vert3.pos + norm_sign)*norm_sign_div);

		if (glm::abs(norm.x) > 0.0000001) {

			vert1.uv.x = v1_uv.z;
			vert1.uv.y = v1_uv.y;

			vert2.uv.x = v2_uv.z;
			vert2.uv.y = v2_uv.y;

			vert3.uv.x = v3_uv.z;
			vert3.uv.y = v3_uv.y;
		}
		else if (glm::abs(norm.y) > 0.0000001) {
			vert1.uv.x = v1_uv.x;
			vert1.uv.y = v1_uv.z;

			vert2.uv.x = v2_uv.x;
			vert2.uv.y = v2_uv.z;

			vert3.uv.x = v3_uv.x;
			vert3.uv.y = v3_uv.z;
		}
		else {
			vert1.uv.x = v1_uv.x;
			vert1.uv.y = v1_uv.y;

			vert2.uv.x = v2_uv.x;
			vert2.uv.y = v2_uv.y;

			vert3.uv.x = v3_uv.x;
			vert3.uv.y = v3_uv.y;
		}

		if (use_int){
			cpy_packed(cur_pos_data_pos,cur_norm_uv_data_pos, vert1);
			cur_pos_data_pos += 8;
			cur_norm_uv_data_pos += 8;

			cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert2);
			cur_pos_data_pos += 8;
			cur_norm_uv_data_pos += 8;

			cpy_packed(cur_pos_data_pos, cur_norm_uv_data_pos, vert3);
			cur_pos_data_pos += 8;
			cur_norm_uv_data_pos += 8;
		}
		else{
			cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert1);
			cur_pos_data_pos += 12;
			cur_norm_uv_data_pos += 20;
			
			cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert2);
			cur_pos_data_pos += 12;
			cur_norm_uv_data_pos += 20;

			cpy_unpacked(cur_pos_data_pos, cur_norm_uv_data_pos, vert3);
			cur_pos_data_pos += 12;
			cur_norm_uv_data_pos += 20;
		}

		memcpy(index_array + cur_pos, &indices, 3 * sizeof(ushort));

		cur_pos += 3;
	}

	element_count = cur_pos;
	vert_count = cur_pos;*/

	
}

