#include "Camera.h"

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));

}

void Camera::move(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::look(float xoffset, float yoffset)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 80.0f)  Pitch = 80.0f;
    if (Pitch < -80.0f) Pitch = -80.0f;

    updateCameraVectors();
}