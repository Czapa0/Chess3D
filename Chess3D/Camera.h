// THIS CLASS IS BASED ON: https://learnopengl.com/Getting-started/Camera

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 InitialFront;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MovementSpeed_Slow;
    float MovementSpeed_Fast;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovementAndChangePosition(const float& xoffset, const float& yoffset, const float& range, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    // changes orientation (useful when going over 90 degrees in Y axis)
    void Flip(float& step);

    // revert to original WorldUp
    void RestoreWorldUp();

private:
    bool m_isWorldUpOriginal = true;
    glm::vec3 m_originalWorldUp;
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};
#endif