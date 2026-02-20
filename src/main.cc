#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL //Aies added

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// OpenGL library includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <debuggl.h>
#include <math.h> 
#include "menger.h"
#include "camera.h"
#include <glm/glm.hpp> //Alies added

int window_width = 800, window_height = 600;
GLFWwindow* window;
glm::mat4 view_matrix = glm::mat4();

int mode = 0;

// VBO and VAO descriptors.
enum { kVertexBuffer, kNormalBuffer, kIndexBuffer, kNumVbos };

// These are our VAOs.
enum { kGeometryVao, kFloorVao, kNumVaos };

GLuint g_array_objects[kNumVaos];  // This will store the VAO descriptors.
GLuint g_buffer_objects[kNumVaos][kNumVbos];  // These will store VBO descriptors.

// C++ 11 String Literal
// See http://en.cppreference.com/w/cpp/language/string_literal
/*const char* vertex_shader =
R"zzz(#version 330 core
in vec4 vertex_position;
in vec4 vertex_normal;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 light_position;
out vec4 light_direction;
out vec4 normal;
void main()
{
// Transform vertex into clipping coordinates
	gl_Position = projection * view * vertex_position;
//  Lighting in camera coordinates
//  Compute light direction and transform to camera coordinates
    light_direction = view * (light_position - vertex_position);
//  Transform normal to camera coordinates
    normal = view * vertex_normal;
}
)zzz";
*/


const char* vertex_shader =
R"zzz(#version 330 core
in vec4 vertex_position;
in vec4 vertex_normal;
uniform mat4 view;
uniform vec4 light_position;
out vec4 vs_light_direction;
out vec4 v_norm;
out vec4 m_norm;
out vec4 wp;
void main()
{

	gl_Position = view * vertex_position;
	wp = vertex_position;
	v_norm = view * vertex_normal;
	m_norm = vertex_normal;
	vs_light_direction = -(gl_Position) + view * light_position;
}
)zzz";

const char* geometry_shader =
R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform mat4 projection;
in vec4 vs_light_direction[];
in vec4 v_norm[];
in vec4 m_norm[];
in vec4 wp[];
flat out vec4 normal;
flat out vec4 model_normal;
out vec4 light_direction;
out vec4 world_position;

void main()
{
	int n = 0;
	for (n = 0; n < gl_in.length(); n++) {
		normal = v_norm[n];
		world_position = wp[n];
		model_normal = m_norm[n];
		light_direction = vs_light_direction[n];
		gl_Position = projection * gl_in[n].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz";

const char* fragment_shader =
R"zzz(#version 330 core
flat in vec4 normal;
flat in vec4 model_normal;
in vec4 light_direction;
out vec4 fragment_color;
uniform int mode;
void main()
{ 
    vec4 purple = vec4(144.0/255.0, 100.0/255.0, 179.0/255.0, 1.0); 
    vec4 b      = vec4(139.0/255.0, 215.0/255.0, 248.0/255.0, 1.0); 
    vec4 pink   = vec4(206.0/255.0, 109.0/255.0, 168.0/255.0, 1.0); 

    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);




	vec4 color = vec4(0.0, 0.5, 0.2, 1.0);
	if (model_normal[0] == 1.0f){
        if(mode == 0) color = red;
		else color = pink;
	}
	if (model_normal[1] == 1.0f){
        if(mode == 0) color = green;
		else color = b;
	}

	if (model_normal[2] == 1.0f){
		if(mode == 0) color = blue;
		else color = purple;

	}
	if (model_normal[0] == -1.0f){
        if(mode == 0) color = red;
		else color = pink;
	}
	if (model_normal[1] == -1.0f){
		if(mode == 0) color = green;
		else color = b;
	}
	if (model_normal[2] == -1.0f){
		if(mode == 0) color = blue;
		else color = purple;
	}
	float dot_nl = dot(normalize(light_direction), normalize(normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);
	fragment_color = clamp((dot_nl + 0.3) * color, 0.0, 1.0);
}
)zzz";

// FIXME: Implement shader effects with an alternative shader.
const char* floor_fragment_shader =
R"zzz(#version 330 core
flat in vec4 normal;
in vec4 light_direction;
in vec4 world_position;
out vec4 fragment_color;
uniform int mode;
void main()
{
    vec4 pink1 = vec4(226.0/255.0, 153.0/255.0, 197.0/255.0, 1.0); 
    vec4 pink2 = vec4(155.0/255.0, 40.0/255.0, 110.0/255.0, 1.0);
    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 color = vec4(0.0, 0.5, 0.2, 1.0);
	int x =  int(floor(world_position.x));
    int z =  int(floor(world_position.z));
    if((x + z) % 2 == 0){
        if(mode == 0) color = white;
		else color = pink1;
	}	
    else {
       if(mode == 0) color = black;
		else color = pink2;
    }   

	float dot_nl = dot(normalize(light_direction), normalize(normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);
	fragment_color = clamp((dot_nl + 0.3) * color, 0.0, 1.0);
}
)zzz";

void
ErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << "\n";
}

std::shared_ptr<Menger> g_menger;
Camera g_camera;

void
KeyCallback(GLFWwindow* window,
            int key,
            int scancode,
            int action,
            int mods)
{
	// Note:
	// This is only a list of functions to implement.
	// you may want to re-organize this piece of code.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
		prev_zoom = zoom; zoom -= 1; 
	} else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
		prev_zoom = zoom; zoom += 1; 
	} else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
		prev_x_pan = x_pan; x_pan -= 1;
	} else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
		prev_x_pan = x_pan; x_pan += 1;
	} else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
		roll += 1;
	} else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
		roll -= 1;
	} else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
		prev_y_pan = y_pan; y_pan -= 1;
	} else if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
		prev_y_pan = y_pan; y_pan += 1;
	} else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		if (orbital == true) orbital = false;
		else orbital = true;
	} else if (key == GLFW_KEY_P && action != GLFW_RELEASE) {
		if (mode == 1) mode = 0;
		else mode = 1;
	}
	if (!g_menger)
		return ; // 0-4 only available in Menger mode.
	if (key == GLFW_KEY_0 && action != GLFW_RELEASE) {
		// FIXME: Change nesting level of g_menger
		// Note: GLFW_KEY_0 - 4 may not be continuous.
		g_menger->set_nesting_level(0);
	} else if (key == GLFW_KEY_1 && action != GLFW_RELEASE) {
		g_menger->set_nesting_level(1);
	} else if (key == GLFW_KEY_2 && action != GLFW_RELEASE) {
		g_menger->set_nesting_level(2);
	} else if (key == GLFW_KEY_3 && action != GLFW_RELEASE) {
		g_menger->set_nesting_level(3);
	} else if (key == GLFW_KEY_4 && action != GLFW_RELEASE) {
		g_menger->set_nesting_level(4);
	}
}

int g_current_button;
bool g_mouse_pressed;
double x = 0, y = 0, prev_x = 0, prev_y = 0;
double zoom = 0.0, prev_zoom = 0.0;
double x_pan = 0, y_pan = 0;
double prev_x_pan = 0, prev_y_pan = 0;
double roll = 0;
bool orbital = true;


void
MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{

	if (!g_mouse_pressed){
		

		return;
	}
	if (g_current_button == GLFW_MOUSE_BUTTON_LEFT) {
		prev_x = x; prev_y = y;	
		x = mouse_x; y = mouse_y;
	} else if (g_current_button == GLFW_MOUSE_BUTTON_RIGHT) {
		prev_zoom = zoom;
		zoom = mouse_x;
	} else if (g_current_button == GLFW_MOUSE_BUTTON_MIDDLE) {
		prev_x_pan = x_pan; prev_y_pan = y_pan;	
		x_pan = mouse_x; y_pan = mouse_y;
	}
}

void
MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	g_mouse_pressed = (action == GLFW_PRESS);
	g_current_button = button;
}

int main(int argc, char* argv[])
{
	std::string window_title = "Menger";
	if (!glfwInit()) exit(EXIT_FAILURE);
	g_menger = std::make_shared<Menger>();
	glfwSetErrorCallback(ErrorCallback);

	// Ask an OpenGL 3.3 core profile context 
	// It is required on OSX and non-NVIDIA Linux
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(window_width, window_height,
			&window_title[0], nullptr, nullptr);
	CHECK_SUCCESS(window != nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MousePosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	std::vector<glm::vec4> obj_vertices;
	std::vector<glm::vec4> vtx_normals;
	std::vector<glm::uvec3> obj_faces;
	std::vector<glm::vec4> floor_vertices;
	std::vector<glm::vec4> floor_normals;
	std::vector<glm::uvec3> floor_faces;

	//Make floor
	floor_vertices.push_back(glm::vec4(-1000.0f, -2.0f, -1000.0f, 1.0f));
	floor_vertices.push_back(glm::vec4(-1000.0f, -2.0f, 1000.0f, 1.0f));
	floor_vertices.push_back(glm::vec4(1000.0f,  -2.0f,  -1000.0f,1.0f));
	floor_vertices.push_back(glm::vec4(1000.0f,  -2.0f,  1000.0f, 1.0f));

	floor_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f,0.0f));
	floor_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f,0.0f));
	floor_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f,0.0f));
	floor_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f,0.0f));

	floor_faces.push_back(glm::uvec3(0, 1, 2));
	floor_faces.push_back(glm::uvec3(3, 1, 2));
        
        //FIXME: Create the geometry from a Menger object (in menger.cc).
	g_menger->set_nesting_level(1);
	g_menger->generate_geometry(obj_vertices, vtx_normals, obj_faces);
	g_menger->set_clean();

	glm::vec4 min_bounds = glm::vec4(std::numeric_limits<float>::max());
	glm::vec4 max_bounds = glm::vec4(-std::numeric_limits<float>::max());
	for (int i = 0; i < obj_vertices.size(); ++i) {
		min_bounds = glm::min(obj_vertices[i], min_bounds);
		max_bounds = glm::max(obj_vertices[i], max_bounds);
	}
	std::cout << "min_bounds = " << glm::to_string(min_bounds) << "\n";
	std::cout << "max_bounds = " << glm::to_string(max_bounds) << "\n";

	// Setup our VAO array.
	CHECK_GL_ERROR(glGenVertexArrays(kNumVaos, &g_array_objects[0]));

	// Switch to the VAO for Geometry.
	CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kGeometryVao]));

	// Generate buffer objects
	CHECK_GL_ERROR(glGenBuffers(kNumVbos, &g_buffer_objects[kGeometryVao][0]));

	// Setup vertex data in a VBO.
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
	// NOTE: We do not send anything right now, we just describe it to OpenGL.
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * obj_vertices.size() * 4, nullptr,
				GL_STATIC_DRAW));
	CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
	CHECK_GL_ERROR(glEnableVertexAttribArray(0));

	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kNormalBuffer]));
	// NOTE: We do not send anything right now, we just describe it to OpenGL.
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * vtx_normals.size() * 4, nullptr,
				GL_STATIC_DRAW));
	CHECK_GL_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
	CHECK_GL_ERROR(glEnableVertexAttribArray(1));

	// Setup element array buffer.
	CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
	CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(uint32_t) * obj_faces.size() * 3,
				&obj_faces[0], GL_STATIC_DRAW));

	/*
 	 * So far the geometry is loaded into g_buffer_objects[kGeometryVao][*].
	 * These buffers are bound to g_array_objects[kGeometryVao]
	 */

	// FIXME: load the floor into g_buffer_objects[kFloorVao][*],
	//        and bind the VBO to g_array_objects[kFloorVao]
	CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kFloorVao]));
    CHECK_GL_ERROR(glGenBuffers(kNumVbos, &g_buffer_objects[kFloorVao][0]));

	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kFloorVao][kVertexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * floor_vertices.size() * 4, &floor_vertices[0] ,
			GL_STATIC_DRAW));
    CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
   CHECK_GL_ERROR(glEnableVertexAttribArray(0));

 	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kFloorVao][kNormalBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * floor_normals.size() * 4, &floor_normals[0] ,
			GL_STATIC_DRAW));
    CHECK_GL_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
	CHECK_GL_ERROR(glEnableVertexAttribArray(1));

    CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kFloorVao][kIndexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(uint32_t) * floor_faces.size() * 3, &floor_faces[0], GL_STATIC_DRAW));


	// Setup vertex shader.
	GLuint vertex_shader_id = 0;
	const char* vertex_source_pointer = vertex_shader;
	CHECK_GL_ERROR(vertex_shader_id = glCreateShader(GL_VERTEX_SHADER));
	CHECK_GL_ERROR(glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr));
	glCompileShader(vertex_shader_id);
	CHECK_GL_SHADER_ERROR(vertex_shader_id);

	// Setup geometry shader.
	GLuint geometry_shader_id = 0;
	const char* geometry_source_pointer = geometry_shader;
	CHECK_GL_ERROR(geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER));
	CHECK_GL_ERROR(glShaderSource(geometry_shader_id, 1, &geometry_source_pointer, nullptr));
	glCompileShader(geometry_shader_id);
	CHECK_GL_SHADER_ERROR(geometry_shader_id);

	// Setup fragment shader.
	GLuint fragment_shader_id = 0;
	const char* fragment_source_pointer = fragment_shader;
	CHECK_GL_ERROR(fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER));
	CHECK_GL_ERROR(glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr));
	glCompileShader(fragment_shader_id);
	CHECK_GL_SHADER_ERROR(fragment_shader_id);

	// Let's create our program.
	GLuint program_id = 0;
	CHECK_GL_ERROR(program_id = glCreateProgram());
	CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
	CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));
	CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));

	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
	// NOTE: We do not send anything right now, we just describe it to OpenGL.
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * obj_vertices.size() * 4, nullptr,
				GL_STATIC_DRAW));
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kNormalBuffer]));
	// NOTE: We do not send anything right now, we just describe it to OpenGL.
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * vtx_normals.size() * 4, nullptr,
				GL_STATIC_DRAW));
	// Bind attributes.
	CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));

	CHECK_GL_ERROR(glBindAttribLocation(program_id, 1, "vertex_normal"));
	CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));
	glLinkProgram(program_id);
	CHECK_GL_PROGRAM_ERROR(program_id);

	// Get the uniform locations.
	GLint projection_matrix_location = 0;
	CHECK_GL_ERROR(projection_matrix_location =
			glGetUniformLocation(program_id, "projection"));
	GLint view_matrix_location = 0;
	CHECK_GL_ERROR(view_matrix_location =
			glGetUniformLocation(program_id, "view"));
	GLint light_position_location = 0;
	CHECK_GL_ERROR(light_position_location =
			glGetUniformLocation(program_id, "light_position"));
		GLint mode_location = 0;
	CHECK_GL_ERROR(mode_location = 
			glGetUniformLocation(program_id, "mode"));

	// Setup fragment shader for the floor
	GLuint floor_fragment_shader_id = 0;
	const char* floor_fragment_source_pointer = floor_fragment_shader;
	CHECK_GL_ERROR(floor_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER));
	CHECK_GL_ERROR(glShaderSource(floor_fragment_shader_id, 1,
				&floor_fragment_source_pointer, nullptr));
	glCompileShader(floor_fragment_shader_id);
	CHECK_GL_SHADER_ERROR(floor_fragment_shader_id);

	// FIXME: Setup another program for the floor, and get its locations.
	// Note: you can reuse the vertex and geometry shader objects
	GLuint floor_program_id = 0;
	CHECK_GL_ERROR(floor_program_id = glCreateProgram());
	CHECK_GL_ERROR(glAttachShader(floor_program_id, vertex_shader_id));
	CHECK_GL_ERROR(glAttachShader(floor_program_id, geometry_shader_id));
	CHECK_GL_ERROR(glAttachShader(floor_program_id, floor_fragment_shader_id));

	CHECK_GL_ERROR(glBindAttribLocation(floor_program_id, 0, "vertex_position"));
	CHECK_GL_ERROR(glBindAttribLocation(floor_program_id, 1, "vertex_normal"));
	CHECK_GL_ERROR(glBindFragDataLocation(floor_program_id, 0, "fragment_color"));
	glLinkProgram(floor_program_id);
	CHECK_GL_PROGRAM_ERROR(floor_program_id);

	// Get the uniform locations.
	GLint floor_projection_matrix_location = 0;
	CHECK_GL_ERROR(floor_projection_matrix_location =
			glGetUniformLocation(floor_program_id, "projection"));
	GLint floor_view_matrix_location = 0;
	CHECK_GL_ERROR(floor_view_matrix_location =
			glGetUniformLocation(floor_program_id, "view"));
	GLint floor_light_position_location = 0;
	CHECK_GL_ERROR(floor_light_position_location =
			glGetUniformLocation(floor_program_id, "light_position"));
	GLint floor_mode_location = 0;
	CHECK_GL_ERROR(floor_mode_location = 
			glGetUniformLocation(floor_program_id, "mode"));

	glm::vec4 light_position = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
	float aspect = 0.0f;
	float theta = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);

		// Switch to the Geometry VAO.
		CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kGeometryVao]));

		if (g_menger && g_menger->is_dirty()) {
			g_menger->generate_geometry(obj_vertices, vtx_normals, obj_faces);
			g_menger->set_clean();
			CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
			CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj_vertices.size() * 4, &obj_vertices[0], GL_STATIC_DRAW));
			CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kNormalBuffer]));
			CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vtx_normals.size() * 4, &vtx_normals[0], GL_STATIC_DRAW));
			CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
			CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * obj_faces.size() * 3, &obj_faces[0], GL_STATIC_DRAW));
}

		// Compute the projection matrix.
		aspect = static_cast<float>(window_width) / window_height;
		glm::mat4 projection_matrix =
			glm::perspective(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);

		// Compute the view matrix
		// FIXME: change eye and center through mouse/keyboard events.
		//int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        //if (state == GLFW_PRESS)
        //if (g_mouse_pressed)	
		view_matrix = g_camera.get_view_matrix();

		// Send vertices to the GPU.
		CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
		                            g_buffer_objects[kGeometryVao][kVertexBuffer]));
		CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
		                            sizeof(float) * obj_vertices.size() * 4,
		                            &obj_vertices[0], GL_STATIC_DRAW));
		CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
		                            g_buffer_objects[kGeometryVao][kNormalBuffer]));
		CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
		                            sizeof(float) * vtx_normals.size() * 4,
		                            &vtx_normals[0], GL_STATIC_DRAW));
		// Use our program.
		CHECK_GL_ERROR(glUseProgram(program_id));

		// Pass uniforms in.
		CHECK_GL_ERROR(glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE,
					&projection_matrix[0][0]));
		CHECK_GL_ERROR(glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE,
					&view_matrix[0][0]));
		CHECK_GL_ERROR(glUniform4fv(light_position_location, 1, &light_position[0]));
		CHECK_GL_ERROR(glUniform1i(mode_location, mode));  // CORRECT

		// Draw our triangles.
		CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, obj_faces.size() * 3, GL_UNSIGNED_INT, 0));


		// FIXME: Render the floor
		// Note: What you need to do is
		// 	1. Switch VAO
		CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kFloorVao]));
		CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
		                            g_buffer_objects[kFloorVao][kIndexBuffer]));
		/*CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(uint32_t) * floor_faces.size() * 3,
		&floor_faces[0], GL_STATIC_DRAW));*/
		// 	2. Switch Program
		CHECK_GL_ERROR(glUseProgram(floor_program_id));
		// 	3. Pass Uniforms
		CHECK_GL_ERROR(glUniformMatrix4fv(floor_projection_matrix_location, 1, GL_FALSE,
			&projection_matrix[0][0]));
        CHECK_GL_ERROR(glUniformMatrix4fv(floor_view_matrix_location, 1, GL_FALSE,
			&view_matrix[0][0]));
        CHECK_GL_ERROR(glUniform4fv(floor_light_position_location, 1, &light_position[0]));
        CHECK_GL_ERROR(glUniform1i(floor_mode_location, mode)); //AK changed
		// 	4. Call glDrawElements, since input geometry is
		// 	indicated by VAO.
		CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, floor_faces.size() * 3, GL_UNSIGNED_INT, 0));



		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
