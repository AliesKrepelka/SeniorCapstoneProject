#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include "main.cc"

extern int window_width;
extern int window_height;
extern GLFWwindow* window;
extern double x, y, prev_x, prev_y;
extern double x_pan, y_pan, prev_x_pan, prev_y_pan;
extern double zoom, prev_zoom;
extern double roll;
extern bool orbital;

class Camera {
public:
	glm::mat4 get_view_matrix() const;
	glm::mat4 look_at(glm::vec3 position, glm::vec3 target, glm::vec3 up) const;
	glm::mat4 get_orbital() const;
	glm::mat4 get_fps() const;
	// FIXME: add functions to manipulate camera objects.
private:
	float camera_distance_ = 3.0;
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0, 0.0f);
	glm::vec3 eye_ = glm::vec3(0.0f, 0.0f, camera_distance_);
	// Note: you may need additional member variables
};

#endif
