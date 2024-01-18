#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Default Camera Values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

/* An abstract camera class that processes input and calculates the corresponding Euler Angles, 
   Vectors and Matrices for use in OpenGL */
class Camera {
private:
	void updateCameraVectors(void);

public:
	glm::vec3 Position, Front, Up, Right, WorldUp; // Camera Attributes
	float Yaw, Pitch;							   // Euler Angles
	float MovementSpeed, MouseSensitivity, Zoom;   // Camera Options

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	
	glm::mat4 GetViewMatrix(void);
	void ProcessKeyBoard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constraintPitch = true);
	void ProcessMouseScroll(float yoffset);
};

/* Constructor with vectors */
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch):
	Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	this->Position = position;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

/* Constructor with scalar values */
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

/* Returns the view matrix calculated using Euler Anglesand the LookAt Matrix */
glm::mat4 Camera::GetViewMatrix(void) {
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

/* processes input received from any keyboard-like input system. Accepts input parameterin the form of camera defined ENUM (to abstract it from windowing systems) */
void Camera::ProcessKeyBoard(Camera_Movement direction, float deltaTime)
{
	float velocity = this->MovementSpeed * deltaTime;
	if (direction == FORWARD) this->Position += this->Front * velocity;
	if (direction == BACKWARD) this->Position -= this->Front * velocity;
	if (direction == LEFT) this->Position -= this->Right * velocity;
	if (direction == RIGHT) this->Position += this->Right * velocity;
}

/* Processes input received from a mouse input system. Expects the offset value in both the x and y direction. */
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constraintPitch)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constraintPitch) {
		if (this->Pitch > 89.0f) this->Pitch = 89.0f;
		if (this->Pitch < -89.0f) this->Pitch = -89.0f;
	}

	// Update Front, Right and Up vectors using the updated Euler angles
	this->updateCameraVectors();
}

/* Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis */
void Camera::ProcessMouseScroll(float yoffset)
{
	this->Zoom -= (float)yoffset;
	if (this->Zoom < 1.0f) this->Zoom = 1.0f;
	if (this->Zoom > 45.0f) this->Zoom = 45.0f;
}

/* Calculates the front vector from the Camera's (updates) Euler Angles */
void Camera::updateCameraVectors(void)
{
	// Calculate the new Front Vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);

	// Also recalculate the Right and Up Vector
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}

#endif /* CAMERA_HEADER */
