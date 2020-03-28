#include "Camera.h"

Camera::Camera() : position_(0.0, 0.0, 0.0), lookAt_(0.0, 0.0, 0.0), up_(0.0, 1.0, 0.0)
{
	projection_.setToIdentity();
}

Camera::~Camera()
{}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	projection_.setToIdentity();
	projection_.perspective(fov, aspect, near, far);
}

void Camera::setPosition(const QVector3D& position)
{
	position_ = position;
}

QVector3D Camera::position() const
{
	return position_;
}

void Camera::translateCamera(const QVector3D& delta)
{
	setPosition(position() + delta);
	setLookAt(lookAt() + delta);
}

void Camera::zoomCamera(const float zoomDistance)
{
	float maxDistance = position_.distanceToPoint(lookAt_);
	float cappedDistance = qMin(maxDistance - 0.01f, zoomDistance);
	position_ += gazeVector() * cappedDistance;
}

// Returns the projection of v onto u
QVector3D project(const QVector3D& v, const QVector3D& u)
{
	return (QVector3D::dotProduct(v, u) / QVector3D::dotProduct(u, u)) * u;
}

// Ortho-normalize 3 vectors using the Gram-Schmidt method
void orthoNormalize(QVector3D& v1, QVector3D& v2, QVector3D& v3)
{
	v2 = v2 - project(v2, v1);
	v3 = v3 - project(v3, v1) - project(v3, v2);

	v1.normalize();
	v2.normalize();
	v3.normalize();
}

// Code based on advice from this post: https://gamedev.stackexchange.com/a/20769
void Camera::rotateAboutFocus(const float yaw, const float pitch)
{
	// Get up and right vectors
	QVector3D gaze = gazeVector();
	QVector3D up(0.0f, 1.0f, 0.0f);
	QVector3D right = QVector3D::crossProduct(gaze, up);
	orthoNormalize(gaze, up, right);
	
	// Get the vector from the focus point to the camera
	QVector3D camFocusVector = position_ - lookAt_;
	
	// Create rotation matrices for yaw and pitch
	QMatrix4x4 yawMatrix;
	yawMatrix.rotate(yaw, up);
	QMatrix4x4 pitchMatrix;
	pitchMatrix.rotate(pitch, right);

	// Rotate camFocusVector by yaw and pitch matrices to get position of camera relative to origin
	camFocusVector = camFocusVector * yawMatrix * pitchMatrix;

	// Add focus point to camFocusVector to get position relative to focus point
	position_ = lookAt_ + camFocusVector;
	
	// Set up vector
	up_ = up;
}

void Camera::rotateInPlace(const float yaw, const float pitch)
{

}

void Camera::setGazeVector(const QVector3D& gaze)
{
	lookAt_ = gaze + position_;
	lookAt_.normalize();
}

QVector3D Camera::gazeVector() const
{
	QVector3D gaze = lookAt_ - position_;
	gaze.normalize();
	return gaze;
}

QVector3D Camera::lookAt() const
{
	return lookAt_;
}

QVector3D Camera::upVector() const
{
	return up_;
}

void Camera::setLookAt(const QVector3D& lookAt)
{
	lookAt_ = lookAt;
}

void Camera::translateLookAt(const QVector3D& delta)
{
	lookAt_ += delta;
}

QMatrix4x4 Camera::getViewMatrix() const
{
	QMatrix4x4 ret;
	ret.setToIdentity();
	ret.lookAt(position_, lookAt_, up_);
	return ret;
}

QMatrix4x4 Camera::getProjectionMatrix() const
{
	return projection_;
}