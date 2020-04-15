#pragma once

#include <QtGui>
#include <QtWidgets>
#include <QtOpenGL>

#include "Renderable.h"
#include "Camera.h"
#include "SceneNode.h"

/**
 * This is just a basic OpenGL widget that will allow a change of background color.
 */
class BasicWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

private:
	Camera camera_;
  SceneNode* root;
  
  QElapsedTimer frameTimer_;

  QOpenGLDebugLogger logger_;
	
	DrawMode drawMode_;
	bool paused_;

	// Mouse controls.
	enum class MouseControl { NoAction = 0, Rotate, Zoom };
	QPoint lastMouseLoc_;
	MouseControl mouseAction_;

protected:
  void drawNode(SceneNode* node);
	
  // Required interaction overrides
  void keyReleaseEvent(QKeyEvent* keyEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

  // Required overrides form QOpenGLWidget
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

	void quit(QString message, int exitCode);
	void setDrawMode(DrawMode drawMode);
  
public:
  BasicWidget(QWidget* parent=nullptr);
  virtual ~BasicWidget();

  void updateScene(const qint64 msSinceLastFrame);
  void renderScene();
  
  // Make sure we have some size that makes sense.
  QSize sizeHint() const {return QSize(800,600);}
};
