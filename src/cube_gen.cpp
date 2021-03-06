#include "cube_gen.h"
#include "base/base.h"

#include "obj_writer.h"

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

enum en_voxel_pass
{
	vpNear = 0,
	vpRight = 1,
	vpFar,
	vpLeft,
	vpBottom,
	vpTop,
	vpPassCount
};

//#define COORD_TO_IDX(X,Y,Z,MAX_COORD)	(X>=0 && Y >=0 && Z >=0 && X < MAX_COORD && Y < MAX_COORD && Z < MAX_COORD) ?(Z*MAX_COORD*MAX_COORD + Y*MAX_COORD + X) : (-1)

short COORD_TO_IDX(short  X, short Y, short Z, short MAX_COORD) {
	return(X >= 0 && Y >= 0 && Z >= 0 && X < MAX_COORD && Y < MAX_COORD && Z < MAX_COORD) ? (Z*MAX_COORD*MAX_COORD + Y*MAX_COORD + X) : (-1);
}

short COORD_TO_IDX(short  X, short Y, short Z, short MAX_COORD_X, short MAX_COORD_Y, short MAX_COORD_Z) {
	return(X >= 0 && Y >= 0 && Z >= 0 && X < MAX_COORD_X && Y < MAX_COORD_Y && Z < MAX_COORD_Z) ? (Z*MAX_COORD_X*MAX_COORD_Y + Y*MAX_COORD_X + X) : (-1);
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

	base::rnd_int _rnd(4239);
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



void get_face_and_vert_count_for_tess_level(uint32 tess_level, uint32 & element_count, uint32 & vert_count) {
	vert_count = 2 + tess_level; // vert per edge
	element_count = (((vert_count - 1) * (vert_count - 1)) << 1) * CUBE_FACE_COUNT * 3;
	vert_count = element_count;
}


inline void mask_to_right_idx(unsigned char mask, unsigned char face_mask, ushort & idx1_out, ushort & idx2_out){
	ushort idx = 0;

	for (unsigned char m = 0x1; m < MASK_BIT_6; m = m << 1){
		if (m == face_mask){
			idx1_out = idx * 4 + 1;
			idx2_out = idx * 4 + 2;
			return;
		}

		if (mask & m){
			idx++;
		}
	}
}

inline void mask_to_top_idx(unsigned char mask, unsigned char face_mask, ushort & idx1_out, ushort & idx2_out){
	ushort idx = 0;

	for (unsigned char m = 0x1; m < MASK_BIT_6; m = m << 1){
		if (m == face_mask){
			idx1_out = idx * 4 + 3;
			idx2_out = idx * 4 + 2;
			return;
		}

		if (mask & m){
			idx++;
		}
	}
}

inline void uvw_by_mask(const glm::vec3 & uvw, glm::vec2 & uv, unsigned char mask){
	if (mask == MASK_BIT_0 || mask == MASK_BIT_2){
		uv.x = uvw.x;
		uv.y = uvw.y;
	}
	else if (mask == MASK_BIT_1 || mask == MASK_BIT_3){
		uv.x = uvw.z;
		uv.y = uvw.y;
	}
	else if (mask == MASK_BIT_4 || mask == MASK_BIT_5){
		/*uv.x = uvw.x;
		uv.y = uvw.z;*/
		// top bottom zero UVs workaround
		
		uv.x = 0;
		uv.y = 0;
	}
}

////////////////////////////////////////////////////////
//		code for variable x,y,z tesselation level	  //	
////////////////////////////////////////////////////////


void generate_voxel_map(short * voxel_map, const ushort & voxels_per_edge_x, const ushort & voxels_per_edge_y, const ushort & voxels_per_edge_z, bool deform){
	short idx_left = -1;
	short idx_right = -1;
	short idx_bottom = -1;
	short idx_top = -1;
	short idx_near = -1;

	short idx;

	const float miss_prob_whole = 0.0f;
	const float miss_prob_half = 0.2f;
	const float miss_prob_one = 0.3f;

	const float miss_half_bound = miss_prob_whole + miss_prob_half;
	const float miss_one_bound = miss_half_bound + miss_prob_one;

	for (ushort z = 0; z < voxels_per_edge_z; ++z){
		for (ushort y = 0; y < voxels_per_edge_y; ++y){
			for (ushort x = 0; x < voxels_per_edge_x; ++x){
				idx = COORD_TO_IDX(x, y, z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

				voxel_map[idx] = idx;
			}
		}
	}

	if (deform){
		for (ushort z = 0; z < voxels_per_edge_z; ++z){
			for (ushort x = 0; x < voxels_per_edge_x; ++x){
				float miss = base::rndNomalized(gen::_rnd);

				short idx;

				if (miss >= 0.0f && miss < miss_prob_whole){
					for (short y = 0; y < voxels_per_edge_y; ++y){
						idx = COORD_TO_IDX(x, y, z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
						voxel_map[idx] = -1;
					}
				}
				else if (miss >= miss_prob_whole && miss < miss_half_bound){
					for (short y = voxels_per_edge_y - 1; y >= (voxels_per_edge_y >> 1); --y){
						idx = COORD_TO_IDX(x, y, z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
						voxel_map[idx] = -1;
					}
				}
				else if (miss >= miss_half_bound && miss < miss_one_bound){
					idx = COORD_TO_IDX(x, voxels_per_edge_y - 1, z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
					voxel_map[idx] = -1;
				}
			}
		}
	}
}

void triangulate_voxel_face(const glm::uvec3 & voxel_pos,
	voxel_info * this_voxel,
	const voxel_info * left_voxel,
	const voxel_info * bottom_voxel,
	unsigned char face_mask,
	const glm::vec3 & normal,
	const ushort & voxels_per_edge_x,
	const ushort & voxels_per_edge_y,
	const ushort & voxels_per_edge_z,
	float * pos_data,
	float * norm_uv_data,
	ushort * index_array,
	short * voxel_vert_idx,
	ushort & out_added_verts,
	ushort & out_added_elems,
	ushort & out_cur_vert_count,
	const ushort & cur_vox_idx_offset,
	const glm::uvec3 & vert2_displace,
	const glm::uvec3 & vert3_displace,
	const glm::uvec3 & vert4_displace,
	bool use_int
	)
{
	ushort v1_idx = 0xffff;
	ushort v2_idx = 0xffff;
	ushort v3_idx = 0xffff;
	ushort v4_idx = 0xffff;

	gen::vert vert1;
	gen::vert vert2;
	gen::vert vert3;
	gen::vert vert4;

	glm::vec3 vert_uwv;

	gen::triangle_pair_vert_indices indices;

	if (left_voxel != nullptr && (left_voxel->mask & face_mask)){// if left neighbour exist and have required face reuse his vertices
		mask_to_right_idx(left_voxel->mask, face_mask, v1_idx, v4_idx);
		v1_idx = voxel_vert_idx[left_voxel->vert_idx + v1_idx];
		v4_idx = voxel_vert_idx[left_voxel->vert_idx + v4_idx];
	}
	else{
		v4_idx = out_cur_vert_count;
		out_cur_vert_count++;

		vert_uwv = glm::vec3(voxel_pos + vert4_displace) / glm::vec3(voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

		vert4.pos = (gen::origin + (gen::diag*vert_uwv));

		vert4.norm = normal;
		uvw_by_mask(vert_uwv, vert4.uv, face_mask);

		out_added_verts++;

		if (use_int){
			cpy_packed(pos_data, norm_uv_data, vert4);
			pos_data += 2;
			norm_uv_data += 2;
		}
		else{
			cpy_unpacked(pos_data, norm_uv_data, vert4);
			pos_data += 3;
			norm_uv_data += 5;
		}
	}

	if (bottom_voxel != nullptr && (bottom_voxel->mask & face_mask)){// if bottom neighbour exist and have required face, reuse his vertices
		mask_to_top_idx(bottom_voxel->mask, face_mask, v1_idx, v2_idx);
		v1_idx = voxel_vert_idx[bottom_voxel->vert_idx + v1_idx];
		v2_idx = voxel_vert_idx[bottom_voxel->vert_idx + v2_idx];
	}
	else{
		if (v1_idx == 0xffff){
			v1_idx = out_cur_vert_count;
			out_cur_vert_count++;

			vert_uwv = glm::vec3(voxel_pos) / glm::vec3(voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

			vert1.pos = (gen::origin + (gen::diag*vert_uwv));

			vert1.norm = normal;
			uvw_by_mask(vert_uwv, vert1.uv, face_mask);

			if (use_int){
				cpy_packed(pos_data, norm_uv_data, vert1);
				pos_data += 2;
				norm_uv_data += 2;
				out_added_verts++;
			}
			else{
				cpy_unpacked(pos_data, norm_uv_data, vert1);
				pos_data += 3;
				norm_uv_data += 5;
				out_added_verts++;
			}
		}

		v2_idx = out_cur_vert_count;
		out_cur_vert_count++;

		vert_uwv = glm::vec3(voxel_pos + vert2_displace) / glm::vec3(voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

		vert2.pos = (gen::origin + (gen::diag*vert_uwv));

		vert2.norm = normal;
		uvw_by_mask(vert_uwv, vert2.uv, face_mask);



		if (use_int){
			cpy_packed(pos_data, norm_uv_data, vert2);
			pos_data += 2;
			norm_uv_data += 2;
			out_added_verts++;
		}
		else{
			cpy_unpacked(pos_data, norm_uv_data, vert2);
			pos_data += 3;
			norm_uv_data += 5;
			out_added_verts++;
		}
	}



	this_voxel->mask += face_mask;

	// these must create for sure

	v3_idx = out_cur_vert_count;
	out_cur_vert_count++;

	vert_uwv = glm::vec3(voxel_pos + vert3_displace) / glm::vec3(voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

	vert3.pos = (gen::origin + (gen::diag*vert_uwv));

	vert3.norm = normal;
	uvw_by_mask(vert_uwv, vert3.uv, face_mask);

	out_added_verts++;

	if (use_int){
		cpy_packed(pos_data, norm_uv_data, vert3);
		pos_data += 2;
		norm_uv_data += 2;
	}
	else{
		cpy_unpacked(pos_data, norm_uv_data, vert3);
		pos_data += 3;
		norm_uv_data += 5;
	}

	voxel_vert_idx[this_voxel->vert_idx + cur_vox_idx_offset] = v1_idx;
	voxel_vert_idx[this_voxel->vert_idx + cur_vox_idx_offset + 1] = v2_idx;
	voxel_vert_idx[this_voxel->vert_idx + cur_vox_idx_offset + 2] = v3_idx;
	voxel_vert_idx[this_voxel->vert_idx + cur_vox_idx_offset + 3] = v4_idx;

	if (face_mask == MASK_BIT_2 || face_mask == MASK_BIT_3 || face_mask == MASK_BIT_4){
		indices.t1_i1 = v1_idx;
		indices.t1_i2 = v3_idx;
		indices.t1_i3 = v2_idx;

		indices.t2_i1 = v1_idx;
		indices.t2_i2 = v4_idx;
		indices.t2_i3 = v3_idx;
	}
	else{
		indices.t1_i1 = v1_idx;
		indices.t1_i2 = v2_idx;
		indices.t1_i3 = v3_idx;

		indices.t2_i1 = v1_idx;
		indices.t2_i2 = v3_idx;
		indices.t2_i3 = v4_idx;
	}

	memcpy(index_array, &indices, 12);
}

void triangulate_voxel(glm::uvec3 & voxel_pos,
	short * voxel_map,
	voxel_info * voxel_inf_arr,
	short * voxel_vert_idx_arr,
	const ushort & voxels_per_edge_x,
	const ushort & voxels_per_edge_y,
	const ushort & voxels_per_edge_z,
	float * pos_data,
	float * norm_uv_data,
	ushort * index_array,
	ushort & out_added_verts,
	ushort & out_added_elems,
	ushort & out_added_voxel_idx,
	ushort & out_cur_vert_count,
	bool use_int,
	en_voxel_pass pass)
{
	const glm::vec3 norm_right(1.0f, 0.0f, 0.0f);
	const glm::vec3 norm_left(-1.0f, 0.0f, 0.0f);
	const glm::vec3 norm_top(0.0f, 0.0f, 1.0f);
	const glm::vec3 norm_bottom(0.0f, 0.0f, -1.0f);
	const glm::vec3 norm_far(0.0f, 1.0f, 0.0f);
	const glm::vec3 norm_near(0.0f, -1.0f, 0.0f);

	const glm::uvec3 near_far_vert2_displacement(1, 0, 0);
	const glm::uvec3 near_far_vert3_displacement(1, 1, 0);
	const glm::uvec3 near_far_vert4_displacement(0, 1, 0);

	const glm::uvec3 left_right_vert2_displacement(0, 0, 1);
	const glm::uvec3 left_right_vert3_displacement(0, 1, 1);
	const glm::uvec3 left_right_vert4_displacement(0, 1, 0);

	const glm::uvec3 bottom_top_vert2_displacement(1, 0, 0);
	const glm::uvec3 bottom_top_vert3_displacement(1, 0, 1);
	const glm::uvec3 bottom_top_vert4_displacement(0, 0, 1);


	ushort cur_idx_offset = 0;

	ushort this_idx = COORD_TO_IDX(voxel_pos.x, voxel_pos.y, voxel_pos.z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

	short idx_left = COORD_TO_IDX(voxel_pos.x - 1, voxel_pos.y, voxel_pos.z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z); // need for face visibility determination
	short idx_right = COORD_TO_IDX(voxel_pos.x + 1, voxel_pos.y, voxel_pos.z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
	short idx_bottom = COORD_TO_IDX(voxel_pos.x, voxel_pos.y - 1, voxel_pos.z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
	short idx_top = COORD_TO_IDX(voxel_pos.x, voxel_pos.y + 1, voxel_pos.z, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
	short idx_near = COORD_TO_IDX(voxel_pos.x, voxel_pos.y, voxel_pos.z - 1, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);
	short idx_far = COORD_TO_IDX(voxel_pos.x, voxel_pos.y, voxel_pos.z + 1, voxels_per_edge_x, voxels_per_edge_y, voxels_per_edge_z);

	voxel_info * this_vox_info = &voxel_inf_arr[this_idx];
	voxel_info * left_vox_info;
	voxel_info * bottom_vox_info;

	float * cur_pos_data = pos_data;;
	float * cur_norm_uv_data = norm_uv_data;
	ushort * cur_index_array = index_array;

	if ((idx_near == -1 || voxel_map[idx_near] == -1) && (pass == vpNear || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_left != -1 && voxel_map[idx_left] != -1){
			left_vox_info = &voxel_inf_arr[idx_left];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_bottom != -1 && voxel_map[idx_bottom] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_bottom];
		}
		else{
			bottom_vox_info = nullptr;
		}

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_0,
			norm_near,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			near_far_vert2_displacement,
			near_far_vert3_displacement,
			near_far_vert4_displacement,
			use_int
			);

		out_added_elems += 6;
		cur_idx_offset += 4;

		if (use_int){
			cur_pos_data = (pos_data + 2 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 2 * out_added_verts);
		}
		else{
			cur_pos_data = (pos_data + 3 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 5 * out_added_verts);
		}

		cur_index_array = (index_array + out_added_elems);
	}

	if ((idx_right == -1 || voxel_map[idx_right] == -1) & (pass == vpRight || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_near != -1 && voxel_map[idx_near] != -1){
			left_vox_info = &voxel_inf_arr[idx_near];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_bottom != -1 && voxel_map[idx_bottom] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_bottom];
		}
		else{
			bottom_vox_info = nullptr;
		}

		voxel_pos.x += 1;

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_1,
			norm_right,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			left_right_vert2_displacement,
			left_right_vert3_displacement,
			left_right_vert4_displacement,
			use_int
			);

		voxel_pos.x -= 1;

		out_added_elems += 6;
		cur_idx_offset += 4;

		if (use_int){
			cur_pos_data = (pos_data + 2 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 2 * out_added_verts);
		}
		else{
			cur_pos_data = (pos_data + 3 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 5 * out_added_verts);
		}

		cur_index_array = (index_array + out_added_elems);
	}

	if ((idx_far == -1 || voxel_map[idx_far] == -1) && (pass == vpFar || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_left != -1 && voxel_map[idx_left] != -1){
			left_vox_info = &voxel_inf_arr[idx_left];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_bottom != -1 && voxel_map[idx_bottom] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_bottom];
		}
		else{
			bottom_vox_info = nullptr;
		}

		voxel_pos.z += 1;

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_2,
			norm_far,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			near_far_vert2_displacement,
			near_far_vert3_displacement,
			near_far_vert4_displacement,
			use_int
			);

		voxel_pos.z -= 1;

		out_added_elems += 6;
		cur_idx_offset += 4;

		if (use_int){
			cur_pos_data = (pos_data + 2 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 2 * out_added_verts);
		}
		else{
			cur_pos_data = (pos_data + 3 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 5 * out_added_verts);
		}

		cur_index_array = (index_array + out_added_elems);
	}

	if ((idx_left == -1 || voxel_map[idx_left] == -1) && (pass == vpLeft || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_near != -1 && voxel_map[idx_near] != -1){
			left_vox_info = &voxel_inf_arr[idx_near];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_bottom != -1 && voxel_map[idx_bottom] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_bottom];
		}
		else{
			bottom_vox_info = nullptr;
		}

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_3,
			norm_left,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			left_right_vert2_displacement,
			left_right_vert3_displacement,
			left_right_vert4_displacement,
			use_int
			);

		out_added_elems += 6;
		cur_idx_offset += 4;

		if (use_int){
			cur_pos_data = (pos_data + 2 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 2 * out_added_verts);
		}
		else{
			cur_pos_data = (pos_data + 3 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 5 * out_added_verts);
		}

		cur_index_array = (index_array + out_added_elems);
	}

	if ((idx_bottom == -1 || voxel_map[idx_bottom] == -1) && (pass == vpBottom || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_left != -1 && voxel_map[idx_left] != -1){
			left_vox_info = &voxel_inf_arr[idx_left];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_near != -1 && voxel_map[idx_near] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_near];
		}
		else{
			bottom_vox_info = nullptr;
		}

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_4,
			norm_bottom,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			bottom_top_vert2_displacement,
			bottom_top_vert3_displacement,
			bottom_top_vert4_displacement,
			use_int
			);

		out_added_elems += 6;
		cur_idx_offset += 4;

		if (use_int){
			cur_pos_data = (pos_data + 2 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 2 * out_added_verts);
		}
		else{
			cur_pos_data = (pos_data + 3 * out_added_verts);
			cur_norm_uv_data = (norm_uv_data + 5 * out_added_verts);
		}

		cur_index_array = (index_array + out_added_elems);
	}

	if ((idx_top == -1 || voxel_map[idx_top] == -1) && (pass == vpTop || pass == vpPassCount)){ // if near neigh not exist then process near face
		if (idx_left != -1 && voxel_map[idx_left] != -1){
			left_vox_info = &voxel_inf_arr[idx_left];
		}
		else{
			left_vox_info = nullptr;
		}

		if (idx_near != -1 && voxel_map[idx_near] != -1){
			bottom_vox_info = &voxel_inf_arr[idx_near];
		}
		else{
			bottom_vox_info = nullptr;
		}

		voxel_pos.y += 1;

		triangulate_voxel_face(voxel_pos,
			this_vox_info,
			left_vox_info,
			bottom_vox_info,
			MASK_BIT_5,
			norm_top,
			voxels_per_edge_x,
			voxels_per_edge_y,
			voxels_per_edge_z,
			cur_pos_data,
			cur_norm_uv_data,
			cur_index_array,
			voxel_vert_idx_arr,
			out_added_verts,
			out_added_elems,
			out_cur_vert_count,
			cur_idx_offset,
			bottom_top_vert2_displacement,
			bottom_top_vert3_displacement,
			bottom_top_vert4_displacement,
			use_int
			);

		voxel_pos.y -= 1;

		out_added_elems += 6;
		cur_idx_offset += 4;
	}

	out_added_voxel_idx += cur_idx_offset;
}

void gen_cube_imp(
	ushort tess_level_x,
	ushort tess_level_y,
	ushort tess_level_z,
	float * pos_data,
	float * norm_uv_data,
	ushort * index_array,
	short * voxel_map,
	voxel_info * voxel_inf,
	uint32 & element_count,
	uint32 & vert_count,
	bool use_int,
	bool deform,
	bool multipass)
{
	short voxel_m[4096];
	voxel_info voxel_i[4096];
	short voxel_vert_idx[4096 * 3];

	const float miss_prob = 0.0f;



	memset(&voxel_m[0], 0, 8192);
	memset(&voxel_i[0], 0, 32768);
	memset(&voxel_vert_idx[0], 0, 24576);

	const ushort vox_per_edge_count_x = (ushort)(2 + tess_level_x) - 1;
	const ushort vert_per_edge_count_x = vox_per_edge_count_x + 1;

	const ushort vox_per_edge_count_y = (ushort)(2 + tess_level_y) - 1;
	const ushort vert_per_edge_count_y = vox_per_edge_count_y + 1;

	const ushort vox_per_edge_count_z = (ushort)(2 + tess_level_z) - 1;
	const ushort vert_per_edge_count_z = vox_per_edge_count_z + 1;

	ushort idx = -1;

	ushort cur_vert_count = 0;
	ushort cur_element_count = 0;
	ushort cur_vox_idx_count = 0;

	char * cur_pos_data_pos = (char *)pos_data;
	char * cur_norm_uv_data_pos = (char*)norm_uv_data;
	ushort * cur_index_array_pos = index_array;
	short * cur_voxel_vert_idx_pos = voxel_vert_idx;
	generate_voxel_map(voxel_m, vox_per_edge_count_x, vox_per_edge_count_y, vox_per_edge_count_z,deform);

	if (multipass){
		for (ushort pass = ushort(vpNear); pass < ushort(vpPassCount); pass++){
			if (pass > 0){
				memset(&voxel_i[0], 0, 32768);
				memset(&voxel_vert_idx[0], 0, 24576);
			}
			for (ushort z = 0; z < vox_per_edge_count_z; ++z){
				for (ushort y = 0; y < vox_per_edge_count_y; ++y){
					for (ushort x = 0; x < vox_per_edge_count_x; ++x){
						ushort added_vertices = 0;
						ushort added_elements = 0;
						ushort added_vox_idx = 0;
						idx = COORD_TO_IDX(x, y, z, vox_per_edge_count_x, vox_per_edge_count_y, vox_per_edge_count_z);

						if (voxel_m[idx] == -1){
							continue;
						}

						voxel_i[idx].vert_idx = cur_vox_idx_count;

						triangulate_voxel(glm::uvec3(x, y, z),
							voxel_m,
							voxel_i,
							voxel_vert_idx,
							vox_per_edge_count_x,
							vox_per_edge_count_y,
							vox_per_edge_count_z,
							(float*)cur_pos_data_pos,
							(float*)cur_norm_uv_data_pos,
							cur_index_array_pos,
							added_vertices,
							added_elements,
							added_vox_idx,
							cur_vert_count,
							use_int,
							en_voxel_pass(pass));

						cur_element_count += added_elements;
						cur_vox_idx_count += added_vox_idx;

						cur_index_array_pos += added_elements;
						cur_voxel_vert_idx_pos += added_vox_idx;

						if (use_int){
							cur_pos_data_pos += 8 * added_vertices;
							cur_norm_uv_data_pos += 8 * added_vertices;
						}
						else{
							cur_pos_data_pos += 12 * added_vertices;
							cur_norm_uv_data_pos += 20 * added_vertices;

						}
					}
				}
			}
		}
	}
	else{
		for (ushort z = 0; z < vox_per_edge_count_z; ++z){
			for (ushort y = 0; y < vox_per_edge_count_y; ++y){
				for (ushort x = 0; x < vox_per_edge_count_x; ++x){
					ushort added_vertices = 0;
					ushort added_elements = 0;
					ushort added_vox_idx = 0;
					idx = COORD_TO_IDX(x, y, z, vox_per_edge_count_x, vox_per_edge_count_y, vox_per_edge_count_z);

					if (voxel_m[idx] == -1){
						continue;
					}

					voxel_i[idx].vert_idx = cur_vox_idx_count;

					triangulate_voxel(glm::uvec3(x, y, z),
						voxel_m,
						voxel_i,
						voxel_vert_idx,
						vox_per_edge_count_x,
						vox_per_edge_count_y,
						vox_per_edge_count_z,
						(float*)cur_pos_data_pos,
						(float*)cur_norm_uv_data_pos,
						cur_index_array_pos,
						added_vertices,
						added_elements,
						added_vox_idx,
						cur_vert_count,
						use_int,
						vpPassCount);

					cur_element_count += added_elements;
					cur_vox_idx_count += added_vox_idx;

					cur_index_array_pos += added_elements;
					cur_voxel_vert_idx_pos += added_vox_idx;

					if (use_int){
						cur_pos_data_pos += 8 * added_vertices;
						cur_norm_uv_data_pos += 8 * added_vertices;
					}
					else{
						cur_pos_data_pos += 12 * added_vertices;
						cur_norm_uv_data_pos += 20 * added_vertices;

					}
				}
			}
		}
	}

	element_count = cur_element_count;
	vert_count = cur_vert_count;
}

void gen_cube_simple_imp(
	int faces_per_side,
	char * const pos_data,
	char * const norm_uv_data,
	ushort * index_array,
	bool use_int){

	char buf[64];

	itoa(faces_per_side, &buf[0], 10);

	std::string file_name = "cube";
	file_name += buf;
	file_name += ".obj";

	//obj_writer ow;
	//ow.open(file_name);

	float step = 2.0f / faces_per_side;
	char * cur_pos_data = pos_data;;
	char * cur_norm_uv_data = norm_uv_data;


	ushort curr_idx = 0;

	const glm::vec3 up_vector(0.0f, 2.0f, 0.0f);
	glm::vec3 origin;
	glm::vec3 dir;

	gen::vert vert;

	// i == 0 - near
	// i == 1 - right
	// i == 2 - far
	// i == 3 - left 
	for (ushort i = 0; i < 4; i++){
		if (i == 0){
			origin = glm::vec3(-1.0f, -1.0f, 1.0f);
			dir = glm::vec3(step, 0.0f, 0.0f);
			vert.norm = glm::vec3(0.0f,0.0f,1.0f);
		}
		else if (i == 1){
			origin = glm::vec3(1.0f, -1.0f, 1.0f);
			dir = glm::vec3(0.0f, 0.0f, -step);
			vert.norm = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else if (i == 2){
			origin = glm::vec3(1.0f, -1.0f, -1.0f);
			dir = glm::vec3(-step, 0.0f, 0.0f);
			vert.norm = glm::vec3(0.0f, 0.0f, -1.0f);
		}
		else if (i == 3){
			origin = glm::vec3(-1.0f, -1.0f, -1.0f);
			dir = glm::vec3(0.0f, 0.0f, step);
			vert.norm = glm::vec3(-1.0f, 0.0f, 0.0f);
		}

		for (ushort u = 0; u <= faces_per_side; u++){
			for (ushort v = 0; v <= 1; v++){
				vert.pos = origin + u*dir + v*up_vector;
				vert.uv = glm::vec2(u / float(faces_per_side),v);
				if (use_int){
					cpy_packed(cur_pos_data, cur_norm_uv_data, vert);
					cur_pos_data += sizeof(glm::int2);
					cur_norm_uv_data += sizeof(glm::int2);
				}
				else{
					cpy_unpacked(cur_pos_data, cur_norm_uv_data, vert);
					cur_pos_data += sizeof(glm::vec3);
					cur_norm_uv_data += sizeof(glm::vec3) + sizeof(glm::vec2);
				}

				/*ow.write_vertex(vert.pos);
				ow.write_normal(vert.norm);
				ow.write_tex_coords(vert.uv);*/
			}
		}

		for (ushort f = 0; f < faces_per_side; f++){
			ushort offset = 2 * f + i * (2 * faces_per_side + 2);
			index_array[curr_idx++] = offset;
			index_array[curr_idx++] = offset + 3;
			index_array[curr_idx++] = offset + 1;
			index_array[curr_idx++] = offset;
			index_array[curr_idx++] = offset + 2;
			index_array[curr_idx++] = offset + 3;			
		}
	}
	// i == 0 - bottom
	// i == 1 - top
	float uv_step = 1.0f / faces_per_side;

	for (ushort i = 0; i <= 1; i++){
		ushort offset = 8 * faces_per_side + 8 + i * (1 + 4 * faces_per_side);
		vert.uv = glm::vec2(0.5f,0.5f);
		if (i == 0){
			vert.norm = glm::vec3(0.0f, -1.0f, 0.0f);
			vert.pos = vert.norm;

			if (use_int){
				cpy_packed(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::int2);
				cur_norm_uv_data += sizeof(glm::int2);
			}
			else{
				cpy_unpacked(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::vec3);
				cur_norm_uv_data += sizeof(glm::vec3) + sizeof(glm::vec2);
			}

			/*ow.write_vertex(vert.pos);
			ow.write_normal(vert.norm);
			ow.write_tex_coords(vert.uv);*/

			vert.pos = glm::vec3(-1.0f, -1.0f, -1.0f);
			
		}
		else{
			vert.norm = glm::vec3(0.0f, 1.0f, 0.0f);

			vert.pos = vert.norm;

			if (use_int){
				cpy_packed(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::int2);
				cur_norm_uv_data += sizeof(glm::int2);
			}
			else{
				cpy_unpacked(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::vec3);
				cur_norm_uv_data += sizeof(glm::vec3) + sizeof(glm::vec2);
			}

		/*	ow.write_vertex(vert.pos);
			ow.write_normal(vert.norm);
			ow.write_tex_coords(vert.uv);*/

			vert.pos = glm::vec3(-1.0f, 1.0f, -1.0f);
		}

		vert.uv = glm::vec2(0.0f, 0.0f);
		
		for (int j = 0; j < 4 * faces_per_side; j++){
			if (j / faces_per_side == 0){
				vert.pos.x += step;
				vert.uv.x += uv_step;
			}
			else if (j / faces_per_side == 1){
				vert.pos.z += step;
				vert.uv.y += uv_step;
			}
			else if (j / faces_per_side == 2){
				vert.pos.x -= step;
				vert.uv.x -= uv_step;
			}
			else if (j / faces_per_side == 3){
				vert.pos.z -= step;
				vert.uv.y -= uv_step;
			}

			if (use_int){
				cpy_packed(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::int2);
				cur_norm_uv_data += sizeof(glm::int2);
			}
			else{
				cpy_unpacked(cur_pos_data, cur_norm_uv_data, vert);
				cur_pos_data += sizeof(glm::vec3);
				cur_norm_uv_data += sizeof(glm::vec3) + sizeof(glm::vec2);
			}

		/*	ow.write_vertex(vert.pos);
			ow.write_normal(vert.norm);
			ow.write_tex_coords(vert.uv);*/
		}

		for (int j = 0; j < 4 * faces_per_side; j++){
			index_array[curr_idx++] = offset;

			if (i == 0){
				index_array[curr_idx++] = (offset + 1) + j;
				index_array[curr_idx++] = (offset + 1) + (j + 1) % (4 * faces_per_side);
			}
			else{
				index_array[curr_idx++] = (offset + 1) + (j + 1) % (4 * faces_per_side);
				index_array[curr_idx++] = (offset + 1) + j;
			}
		}
	}

	/*for (int i = 0; i < curr_idx; i += 3){
		ow.write_face_idx_from_zero(index_array[i], index_array[i], index_array[i],
			index_array[i+1], index_array[i+1], index_array[i+1], 
			index_array[i+2], index_array[i+2], index_array[i+2]);
	}*/


}



