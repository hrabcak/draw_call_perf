If you want to compile async_vbo_transfers examples by yourself
you will need GLM math library.

For those users which have instaled GIT there are
prepared scripts get_glm.sh/get_glm.bat that download GLM repository
into ./etx/ogl-math folder.

GLM git url:
git://ogl-math.git.sourceforge.net/gitroot/ogl-math/ogl-math

Otherwise you have to download GLM library by yourself from here 
http://glm.g-truc.net/ and unpack it to ./ext/ogl-math folder.

All examples were prepared with GLM version 0.9.2.6.

Test 0: Draw naive

Call pair of glVertexAttribI4i and glDrawElements per draw call. glVertexAttrib is there to provide drawcall data. (instance_id, material_id, vertex offset etc.)

[code]
	for(int i = 0; i < nmeshes; ++i, ++dc) {
		glVertexAttribI3i(
			13,
			ctx->_scene_data_offset + offset,	// instance id
			dc->_first_vertex,					// vertex offset (used if vertex data are pulled from texture buffer)
			i,									// materia id / texture id
			0);
		glDrawElements(
			GL_TRIANGLES,
			dc->_nelements,
			GL_UNSIGNED_SHORT,
			(void*)dc->_first_index);
	}
[/code]


Test 1: Draw with base instance
Call one glDrawElementsInstancedBaseInstance (TB) or glDrawElementsInstancedBaseVertexBaseInstance (VBO) per draw call
depends on the mode. Mesh data are provides from buffer as int4

[code]
	// init array with mesh data
	glBindBuffer(GL_ARRAY_BUFFER, _drawid_vbo);
	glVertexAttribIPointer(13, 4, GL_INT, 0, (GLvoid*)0);
	glVertexAttribDivisor(13, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//
	for(int i = 0; i < nmeshes; ++i, ++dc) {
		glDrawElementsInstancedBaseInstance(
			GL_TRIANGLES,
			dc->_nelements,
			GL_UNSIGNED_SHORT,
			(void*)dc->_first_index,
			1,
			i);
	}
[/code]

Test 2: Multi draw indirect
Whole scene is draw with one function call. Mesh data are provided from buffer as int4 per instance.

[code]
	glMultiDrawElementsIndirect(
		GL_TRIANGLES,
		GL_UNSIGNED_SHORT,
		(void*)ctx->_scene_data_offset,
		num_instances,
		0);
[/code]

Test 3: Instancing
In this test whole scene is draw with one function call. This test is not fair because it's using slightly different data. In texture buffer mode it's using unique data per isntance like other test but in VBO mode it's repeating the same instance for whole scene. Purpose of this test is to show fastest possible way but with limits.

[code]
	glDrawElementsInstanced(
		GL_TRIANGLES,
		_dc_data[0]._nelements,
		GL_UNSIGNED_SHORT,
		0,
		nblocks);
[/code]

Texture mode 0: None
No textures

Texture mode 1: Naive
glBindMultiTextureEXT is call per draw call.

Texture mode 2: Texture Array
All textures are in multiple texture arrays.

Texture mode 3: Bindless
In this mode 64bit texture handles are stored in texture buffer and fetched in vertex shader by texture_id and pass to fragment shader.

Command line switches:
	--debug-ctx  Enable debug contex and GL_ARB_debug_output
	--debug-ctx-sync  Same as --debug-ctx but enable GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
					  
	--test0 - glVertexAttribI3i + glDrawElements
	--test1 - glDrawElementsBaseInstance
	--test2 - glMultiDrawIndirect (default)
	--test3 - glMultiDrawElementsIndirect
	
	--tex-mode0 - No textures
	--tex-mode1 - Classis texture binding with glBindMultiTextureEXT
	--tex-mode2 - Texture arrays with max possible slices (default)
	--tex-mode3 - Bindless texturing handles are stored in texture buffer
	
	--tex-size32 - Texture 32x32 BGRA8
	--tex-size64 - Texture 64x64 BGRA8 (default)
	--tex-size128 - Texture 128x128 BGRA8
	
	--mesh-size0 - Meshes with 12 faces
	--mesh-size1 - Meshes with ~48 faces
	--mesh-size2 - Meshes with ~108 faces (default)
	--mesh-size3 - Meshes with ~192 faces
	--mesh-size4 - Meshes with ~300 faces
	
	--tex-freq0 - One texture for all meshes
	--tex-freq1 - One unique texture for each mesh (default)
	--tex-freq2 - One unique texture for two meshes
	--tex-freq3 - One unique texture for four meshes
	
	--one-mesh - Use one geometry for all meshes
	--use-vbo - Use classic buffer for vertex data instead of texture buffer
	--dont-rnd-cubes - Turn off cube randomization (better utilization of post-transform cache)
	--use_nor_uv - Will use additional vertex data normal and uv compressed to 8 bytes (16bytes vertex data)

