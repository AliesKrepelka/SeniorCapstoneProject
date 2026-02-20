I recived this project templete from my professor Shiahi.Tang. Any edits will be made here for documentation.

Everything is being compiled in MSYS2 MINGW64x


----------------
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
-------------------
Has been added to complie main.cc and camera.cc to allow the program to compile

ibutgrpahicsutil.dll and glfw3.dll and glew32.dll missing upon 1st compile and run of menger.exe within the bin

The build works by grabbing the ibutgrpahicsutil.dll and putting in the bin file and runnin the menger.exe this populates the other 2 missing dlls
glfw3.dll and glew32.dll

main.cc line 160 and 192 added
fragment_color = clamp((dot_nl + 0.3) * color, 0.0, 1.0); 
to fix lighting^^

It was fragment_color = clamp(dot_nl * color, 0.0, 1.0); //grey
before 
------------------
CHECK_GL_ERROR(glUniform1i(floor_mode_location, mode)); is incorrect   (Line 153)
and has been changed to 
CHECK_GL_ERROR(glUniform1i(mode_location, mode));  // CORRECT
---------
Line 605 CHECK_GL_ERROR(glUniform1i(floor_mode_location, mode));
changed to 
Line 533*important:   glClearColor(0.2f, 0.2f, 0.2f, 1.0f); changed to this and allows us to see the menger file/waves
---------------
if (g_menger && g_menger->is_dirty()) {
		    g_menger->generate_geometry(obj_vertices, vtx_normals, obj_faces);
			g_menger->set_clean();
			CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
			CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(uint32_t) * obj_faces.size() * 3,
				&obj_faces[0], GL_STATIC_DRAW));
		}

------------
if (g_menger && g_menger->is_dirty()) {
    g_menger->generate_geometry(obj_vertices, vtx_normals, obj_faces);
    g_menger->set_clean();
    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj_vertices.size() * 4, &obj_vertices[0], GL_STATIC_DRAW));
    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kNormalBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vtx_normals.size() * 4, &vtx_normals[0], GL_STATIC_DRAW));
    CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * obj_faces.size() * 3, &obj_faces[0], GL_STATIC_DRAW));
} //New adition


CAMERA.CC

float horizon = 0.0f; //new and before it was 3.14f


-------
float horizon = 0.0f;
float vertical = 0.3f;
float camera_dist = 5.0;
glm::vec3 position(0.0f, 0.0f, 5.0f);  // was 3.0f
