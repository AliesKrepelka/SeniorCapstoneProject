#define GLM_ENABLE_EXPERIMENTAL
#include "camera.h"
#include <stdio.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

namespace {
	float pan_speed      = 0.01f;  // FIX: was 0.1f, caused huge jump on first pan
	float roll_speed     = 0.1f;
	float rotation_speed = 0.01f;
	float zoom_speed     = 0.05f;  // FIX: was 0.1f, smoother zoom
	float horizon        = 0.0f;
	float vertical       = 0.3f;
	float camera_dist    = 5.0f;

	glm::vec3 target(0.0f, 0.0f, 0.0f);
	glm::vec3 pan(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 position(0.0f, 0.0f, 5.0f);
};

glm::mat4 Camera::get_view_matrix() const
{
	if (orbital)
		return get_orbital();
	else
		return get_fps();
}

glm::mat4 Camera::get_orbital() const
{
	// Rotate based on mouse delta
	horizon  += rotation_speed * (x - prev_x);
	vertical += rotation_speed * -(y - prev_y);

	// Clamp vertical to avoid gimbal flip
	if (vertical >  1.5f) vertical =  1.5f;
	if (vertical < -1.5f) vertical = -1.5f;

	// Spherical coordinates -> cartesian position
	glm::vec3 position_(
		cos(vertical) * sin(horizon),
		sin(vertical),
		cos(vertical) * cos(horizon)
	);
	position = position_;

	// Zoom
	camera_dist += (zoom - prev_zoom) * zoom_speed;
	if (camera_dist < 1.0f) camera_dist = 1.0f;
	position = camera_dist * glm::normalize(position);

	// Build axes
	glm::vec3 zaxis = glm::normalize(position - target);
	glm::vec3 xaxis = glm::normalize(glm::vec3(
		sin(horizon - 3.14159f / 2.0f),
		0.0f,
		cos(horizon - 3.14159f / 2.0f)
	));
	glm::vec3 yaxis = glm::cross(xaxis, zaxis);

	// Roll
	up = glm::rotate(yaxis, float(roll * roll_speed), zaxis);
	glm::vec3 right = glm::rotate(xaxis, float(roll * roll_speed), zaxis);

	// Pan
	pan.x += pan_speed * (x_pan - prev_x_pan);
	pan.y += pan_speed * (y_pan - prev_y_pan);
	target   = pan.x * right + pan.y * up;
	position = position + pan.x * right + pan.y * up;

	// Reset deltas
	prev_x     = x;     prev_y     = y;
	prev_zoom  = zoom;
	prev_x_pan = x_pan; prev_y_pan = y_pan;

	return look_at(position, target, up);
}

glm::mat4 Camera::get_fps() const
{
	// Rotate based on mouse delta
	horizon  += rotation_speed * (x - prev_x);
	vertical += rotation_speed * -(y - prev_y);

	// Clamp vertical
	if (vertical >  1.5f) vertical =  1.5f;
	if (vertical < -1.5f) vertical = -1.5f;

	glm::vec3 direction(
		cos(vertical) * sin(horizon),
		sin(vertical),
		cos(vertical) * cos(horizon)
	);
	direction = -direction;

	glm::vec3 zaxis = glm::normalize(direction);
	glm::vec3 xaxis = -1.0f * glm::vec3(
		sin(horizon - 3.14159f / 2.0f),
		0.0f,
		cos(horizon - 3.14159f / 2.0f)
	);
	glm::vec3 yaxis = glm::cross(xaxis, direction);

	// Roll
	up = glm::rotate(yaxis, -float(roll * roll_speed), zaxis);
	glm::vec3 right = glm::rotate(xaxis, -float(roll * roll_speed), zaxis);

	// Move
	position -= float((zoom - prev_zoom) * zoom_speed) * zaxis;
	position += float(pan_speed * (x_pan - prev_x_pan)) * right;
	position += float(pan_speed * (y_pan - prev_y_pan)) * up;

	// Reset deltas
	prev_x     = x;     prev_y     = y;
	prev_zoom  = zoom;
	prev_x_pan = x_pan; prev_y_pan = y_pan;

	return look_at(position, position + direction, up);
}

glm::mat4 Camera::look_at(glm::vec3 position, glm::vec3 target, glm::vec3 up) const
{
	glm::vec3 zaxis = glm::normalize(position - target);
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 rotation(1.0f);
	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	glm::mat4 translation(1.0f);
	translation[3][0] = -position.x;
	translation[3][1] = -position.y;
	translation[3][2] = -position.z;

	return rotation * translation;
}