#include "BasicWidget.h"
#include "OBJLoader.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);
	models = QVector<Model>();
}

BasicWidget::~BasicWidget()
{
}

///////////////////////////////////////////////////////////////////////
// Protected
void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
  // TODO: Handle key events here.
  if (keyEvent->key() == Qt::Key_1) {
    qDebug() << "1 Pressed";
		currentModel = 0;
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_2) {
    qDebug() << "2 Pressed";
		currentModel = 1;
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
	}
	else if (keyEvent->key() == Qt::Key_Q) {
		qDebug() << "Q Pressed";
		update();  // We call update after we handle a key press to trigger a redraw when we are ready
	}
	else if (keyEvent->key() == Qt::Key_W) {
		qDebug() << "W Pressed";
		update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}
void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  QOpenGLContext* curContext = this->context();
  qDebug() << "[BasicWidget]::initializeGL() -- Context Information:";
  qDebug() << "  Context Valid: " << std::string(curContext->isValid() ? "true" : "false").c_str();
  qDebug() << "  GL Version Used: " << curContext->format().majorVersion() << "." << curContext->format().minorVersion();
  qDebug() << "  Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  qDebug() << "  Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  qDebug() << "  Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << "  GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

	// declare our model names
	QVector<std::string> modelFiles{
		"./objects/cube.obj"
	};
	
	// load obj data
	for (std::string& file : modelFiles) {
		models.append(OBJLoader::loadOBJ(file));
	}
	
	for (Model& model : models) {
		qDebug() << model.vertsToDraw;
	}

  glViewport(0, 0, width(), height());
}

void BasicWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  models[currentModel].shaderProgram_.bind();
  models[currentModel].vao_.bind();
  glDrawElements(GL_TRIANGLES, models[currentModel].vertsToDraw, GL_UNSIGNED_INT, 0);
	models[currentModel].vao_.release();
  models[currentModel].shaderProgram_.release();
}
