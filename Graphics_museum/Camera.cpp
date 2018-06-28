#include "Camera.h"


const kmuvcl::math::vec3f Camera::center_position() const
{

  kmuvcl::math::vec3f center_position(position_(0) + front_dir_(0),
                 position_(1) + front_dir_(1),
                 position_(2) + front_dir_(2));
  return center_position;
}

// TODO: fill up the following functions properly
void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
}

void Camera::move_backward(float delta)
{
  position_ -= delta * front_dir_;
}

void Camera::move_left(float delta)
{
  position_ -= delta * right_dir_;
}

void Camera::move_right(float delta)
{
  position_ += delta * right_dir_;
}

void Camera::rotate_left(float delta)
{
  front_dir_ -= delta * right_dir_;
  right_dir_ = kmuvcl::math::cross(front_dir_, up_dir_);
}

void Camera::rotate_right(float delta)
{
  front_dir_ += delta * right_dir_;
  right_dir_ = kmuvcl::math::cross(front_dir_, up_dir_);
}
