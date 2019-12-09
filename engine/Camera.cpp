#include <GLFW/glfw3.h>

#include "Camera.h"

#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) :
  _position(position), _lookAt(lookAt), _up(up)
{
  updateAngles();
  updateFront();
}

void Camera::updateSpeed()
{
  float currentFrame = glfwGetTime();
  _deltaTime = currentFrame - _lastFrame;
  _lastFrame = currentFrame;
  _speed = 10.5f * _deltaTime; // adjust accordingly
}

void Camera::zoomIn()
{
  _position += _speed * _front;
}

void Camera::zoomOut()
{
  _position -= _speed * _front;
}

void Camera::rotateLeft()
{
  _yaw -= 1.0f * _rotationSpeed;
  updatePosition();
}

void Camera::rotateRight()
{
  _yaw += 1.0f * _rotationSpeed;
  updatePosition();
}

void Camera::moveForward()
{
  _position.y += _moveSpeed;
  updateAngles();
  updateFront();
}

void Camera::moveBackward()
{
  _position.y -= _moveSpeed;
  updateAngles();
  updateFront();
}

void Camera::moveLeft()
{

  _position.x -= _moveSpeed;
  updateAngles();
  updateFront();
}

void Camera::moveRight()
{
  _position.x += _moveSpeed;
  updateAngles();
  updateFront();
}

void Camera::tilt(double x, double y)
{
  if (_firstMouse) {
    _lastX = x;
    _lastY = y;
    _firstMouse = false;
  }
  float xOffset = x - _lastX;
  float yOffset = _lastY - y;
  _lastX = x;
  _lastY = y;

  float sensitivity = 0.05f;
  xOffset *= sensitivity;
  yOffset *= sensitivity;

  _yaw -= xOffset;
  _pitch += yOffset;

  if (_pitch > 89.0f) {
    _pitch = 89.0f;
  }
  if (_pitch < -89.0f) {
    _pitch = -89.0f;
  }

  updateFront();
  std::cout << "_pitch= " << _pitch << std::endl;
  std::cout << "_yaw= " << _yaw << std::endl;
}

void Camera::zoom(double factor)
{
  if (_fov >= 1.0f && _fov <= 45.0f)
    _fov -= factor;
  if (_fov <= 1.0f)
    _fov = 1.0f;
  if (_fov >= 45.0f)
    _fov = 45.0f;
}

float Camera::fov() const
{
  return _fov;
}

glm::vec3 Camera::eye() const
{
  return _position;
}

glm::vec3 Camera::reference() const
{
  return _position + _front;
}

glm::vec3 Camera::up() const
{
  return _up;
}

void Camera::updateFront()
{
  glm::vec3 front;
  front.x = ::cos(glm::radians(_pitch)) * ::cos(glm::radians(_yaw));
  front.y = ::cos(glm::radians(_pitch)) * ::sin(glm::radians(_yaw));
  front.z = ::sin(glm::radians(_pitch));
  _front = glm::normalize(front);
}

float Camera::getPitch() const
{
  return _pitch;
}

void Camera::updatePosition()
{
  _position.x = -::sin(glm::radians(_yaw + 90)) * _camRadius;
  _position.y = ::cos(glm::radians(_yaw + 90)) * _camRadius;
  updateFront();
}

void Camera::setEye(glm::vec3 p)
{
  _position = p;
  updateAngles();
  updateFront();
}

void Camera::updateAngles()
{
  float a = ::abs(_position.z - _lookAt.z);
  auto rCamPosition = ::sqrt(::pow(_position.x, 2) + ::pow(_position.y, 2));
  auto rLookAt = ::sqrt(::pow(_lookAt.x, 2) + ::pow(_lookAt.y, 2));
  float b = ::abs(rCamPosition - rLookAt);

  _pitch = -glm::degrees(::atan(a / b));
  _camRadius = b;
}
