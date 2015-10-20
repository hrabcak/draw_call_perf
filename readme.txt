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
	for(int i = 0; i < 32768; ++i, ++dc) {
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
	for(int i = 0; i < 32768; ++i, ++dc) {
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


Texture mode 3: Bindless
In this mode 64bit texture handles are stored in texture buffer and fetched in vertex shader by texture_id and pass to fragment shader.

Mesh sizes
0: 24 vertices, 12 faces
1: 
2: 96 vertices, 112 faces
3: 150 vertices, 
4: 

