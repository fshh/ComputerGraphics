#include "BasicWidget.h"
#include "OBJLoader.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);
	models = QVector<Model*>();
	
	// set up matrices
	worldToCamera_.setToIdentity();
	worldToCamera_.translate(0.0f, 0.0f, 0.0f);
	projection_.setToIdentity();
	projection_.perspective(90.0f, 800.0f/600.0f, 0.1f, 1000.0f);
}

BasicWidget::~BasicWidget()
{
	for (Model* model : models) {
		delete model;
	}
}

///////////////////////////////////////////////////////////////////////
// Protected
void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
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
		close();
		((QWidget*)parent())->close();
	}
	else if (keyEvent->key() == Qt::Key_W) {
		qDebug() << "W Pressed";
		wireframe = !wireframe;
		makeCurrent();
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
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
	
	QDir dir = QDir::current();
	while (!dir.exists("objects")) {
		dir.cdUp();
	}
	dir.cd("objects");

	// declare our model names
	QVector<QString> modelFiles{
		//"cube.obj"
		"monkey_centered.obj",
		"bunny_centered.obj"
	};
	
	// load obj data
	for (QString& file : modelFiles) {
		QString path = dir.filePath(file);
		models.append(OBJLoader::loadOBJ(path.toStdString()));
	}

  glViewport(0, 0, width(), height());
}

void BasicWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Model* model = models[currentModel];

  model->shaderProgram_.bind();
	int u_modelToWorld = model->shaderProgram_.uniformLocation("modelToWorld");
	int u_worldToCamera = model->shaderProgram_.uniformLocation("worldToCamera");
	int u_cameraToView = model->shaderProgram_.uniformLocation("cameraToView");
	model->shaderProgram_.setUniformValue(u_worldToCamera, worldToCamera_);
	model->shaderProgram_.setUniformValue(u_cameraToView, projection_);
	model->shaderProgram_.setUniformValue(u_modelToWorld, model->modelToWorld_);
  model->vao_.bind();
  glDrawElements(GL_TRIANGLES, model->vertsToDraw, GL_UNSIGNED_INT, 0);
	model->vao_.release();
  model->shaderProgram_.release();
}
