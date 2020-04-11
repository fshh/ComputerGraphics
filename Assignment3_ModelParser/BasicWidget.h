#pragma once

#include <QtGui>
#include <QtWidgets>
#include <QtOpenGL>
#include "Model.h"

/**
 * This is just a basic OpenGL widget that will allow a change of background color.
 */
class BasicWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

private:
	QVector<Model*> models;
	size_t currentModel = 0;
	
	QMatrix4x4 worldToCamera_;
	QMatrix4x4 projection_;
	
	bool wireframe = false;

protected:
  // Required interaction overrides
  void keyReleaseEvent(QKeyEvent* keyEvent) override;

  // Required overrides form QOpenGLWidget
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

public:
  BasicWidget(QWidget* parent=nullptr);
  virtual ~BasicWidget();
  
  // Make sure we have some size that makes sense.
  QSize sizeHint() const {return QSize(800,600);}
};
