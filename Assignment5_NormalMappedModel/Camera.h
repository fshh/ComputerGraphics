#pragma once

#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>

class Camera
{
protected:
	QVector3D position_;
	QVector3D lookAt_;
	QVector3D up_;
	QMatrix4x4 projection_;

public:
	Camera(QVector3D position = QVector3D(0, 0, -1), QVector3D lookAt = QVector3D(0, 0, 0), QVector3D up = QVector3D(0, 1, 0));
	virtual ~Camera();

	// Manipulate our Camera
	void setPerspective(float fov, float aspect, float near, float far);

	// Move our position.
	void setPosition(const QVector3D& position);
	QVector3D position() const;
	void translateCamera(const QVector3D& delta);
	void zoomCamera(const float zoomDistance);

	// Rotate our camera
	void rotateAboutFocus(const float yaw, const float pitch);
	void rotateInPlace(const float yaw, const float pitch);

	// Move our gaze
	void setGazeVector(const QVector3D& gaze);
	QVector3D gazeVector() const;
	QVector3D upVector() const;
	void setLookAt(const QVector3D& lookAt);
	void translateLookAt(const QVector3D& delta);
	QVector3D lookAt() const;

	// Get our camera matrix
	QMatrix4x4 getViewMatrix() const;
	QMatrix4x4 getProjectionMatrix() const;

	// Reset camera to inital orientation
	void reset();

private:
	const QVector3D initialPosition_;
	const QVector3D initialLookAt_;
	const QVector3D initialUp_;

};