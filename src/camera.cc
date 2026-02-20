#define GLM_ENABLE_EXPERIMENTAL //Alies
#include "camera.h"
#include <stdio.h>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/glm.hpp> //Alies
namespace {
	float pan_speed = 0.1f;
	float roll_speed = 0.1f;
	float rotation_speed = 0.01f;
	float zoom_speed = 0.1f;
	float horizon = 0.0f;
float vertical = 0.3f;
float camera_dist = 5.0;
	//float roll = 0.0;
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 pan(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0, 0.0f); //up
   glm::vec3 position(0.0f, 0.0f, 5.0f);  // was 3.0f
};

// FIXME: Calculate the view matrix
glm::mat4 Camera::get_view_matrix() const
{
    if(orbital)
        return get_orbital();
    else return get_fps();
}

glm::mat4 Camera::get_orbital() const {

    //Set rotation
    horizon  += (rotation_speed * (x - prev_x));
    vertical += (rotation_speed * -(y - prev_y));

    glm::vec3 position_(  //Position of camera in orbit
    cos(vertical) * sin(horizon),
    sin(vertical),
    cos(vertical) * cos(horizon));

    position = position_;

    //Set zoom
    camera_dist += (zoom - prev_zoom) * zoom_speed;
    if (camera_dist < 1.0) camera_dist = 1.0;
    position = camera_dist * glm::normalize(position);



    glm::vec3 zaxis = glm::normalize(position - target);
    glm::vec3 xaxis = 1.0f * glm::vec3(sin(horizon - 3.14f/2.0f), 0, cos(horizon - 3.14f/2.0f));
    glm::vec3 yaxis = glm::cross(xaxis, zaxis);

    //Set roll 
    up = glm::rotate(yaxis, float(roll * roll_speed), zaxis);
    glm::vec3 right = glm::rotate(xaxis, float(roll * roll_speed), zaxis);

    pan.x += pan_speed * (x_pan - prev_x_pan);
    pan.y += pan_speed * (y_pan - prev_y_pan);
    target = pan.x * right + pan.y * up;
    position += pan.x * right + pan.y * up;
    //position -= float((zoom - prev_zoom) * zoom_speed) * zaxis;

    //Reset input info
    prev_x = x; prev_y = y;
    prev_zoom = zoom;
    prev_x_pan = x_pan; prev_y_pan = y_pan;
 

 
    return look_at(position, target, up);
}

glm::mat4 Camera::get_fps() const {
    //Set rotation
    horizon  += (rotation_speed * (x - prev_x));
    vertical += (rotation_speed * -(y - prev_y));


    glm::vec3 direction( //Look direction
    cos(vertical) * sin(horizon),
    sin(vertical),
    cos(vertical) * cos(horizon));

    direction = -direction;

    glm::vec3 zaxis = glm::normalize(direction);
    glm::vec3 xaxis = -1.0f * glm::vec3(sin(horizon - 3.14f/2.0f), 0, cos(horizon - 3.14f/2.0f));
    glm::vec3 yaxis = glm::cross(xaxis, direction);

    //Roll
    up = glm::rotate(yaxis, -float(roll * roll_speed), zaxis);
    glm::vec3 right = glm::rotate(xaxis, -float(roll * roll_speed), zaxis);


    //Zoom and pan
    position -= float((zoom - prev_zoom) * zoom_speed) * zaxis;
    position += float(pan_speed * (x_pan - prev_x_pan)) * right;
    position += float(pan_speed * (y_pan - prev_y_pan)) * up;

    //Settings to transfer to orbital mode;
    //pan.x += pan_speed * (x_pan - prev_x_pan);
    //pan.y += pan_speed * (y_pan - prev_y_pan);
    //camera_dist += (zoom - prev_zoom) * zoom_speed;
    //target = position + (camera_dist * direction);


    //Reset input info
    prev_x = x; prev_y = y;
    prev_zoom = zoom;
    prev_x_pan = x_pan; prev_y_pan = y_pan;

    return look_at(position, position + direction , up);
}

glm::mat4 Camera::look_at(glm::vec3 position, glm::vec3 target, glm::vec3 up) const
{
    // LookAt implementation
    glm::mat4 translation;
    translation[3][0] = -position.x;
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;
    glm::mat4 rotation;
    glm::vec3 zaxis = glm::normalize(position - target);
    glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
    glm::vec3 yaxis = glm::cross(zaxis, xaxis);
    rotation[0][0] = xaxis.x; // First column, first row
    rotation[1][0] = xaxis.y;
    rotation[2][0] = xaxis.z;
    rotation[0][1] = yaxis.x; // First column, second row
    rotation[1][1] = yaxis.y;
    rotation[2][1] = yaxis.z;
    rotation[0][2] = zaxis.x; // First column, third row
    rotation[1][2] = zaxis.y;
    rotation[2][2] = zaxis.z; 

    return rotation * translation;
}
