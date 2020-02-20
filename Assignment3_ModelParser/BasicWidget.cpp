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
  vao_.release();
  vao_.destroy();
}

//////////////////////////////////////////////////////////////////////
// Privates
QString BasicWidget::vertexShaderString() const
{
  QString str =
	"#version 330\n"
	"uniform vec3 lightDir = vec3(0.2, -1, 0.2);\n"
	"layout(location = 0) in vec3 position;\n"
  "layout(location = 1) in vec3 normal;\n"
	"varying float intensity;\n"
	"void main()\n"
	"{\n"
	"  gl_Position = vec4(position, 1.0);\n"
	"  intensity = dot(lightDir, normal)"
  "}\n";
  return str;
}

QString BasicWidget::fragmentShaderString() const
{
  QString str =
	"#version 330\n"
  "varying float intensity;\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"  vec4 vertColor;\n"
	"	 if (intensity > 0.95)\n"
	"    vertColor = vec4(1.0, 0.5, 0.5, 1.0);\n"
	"  else if (intensity > 0.5)\n"
	"    vertColor = vec4(0.6, 0.3, 0.3, 1.0);\n"
	"  else if (intensity > 0.25)\n"
	"    vertColor = vec4(0.4, 0.2, 0.2, 1.0);\n"
	"  else\n"
	"    vertColor = vec4(0.2, 0.1, 0.1, 1.0);\n"
	"  color = vertColor;\n"
	"}\n";
  return str;
}

void BasicWidget::createShader()
{
  QOpenGLShader vert(QOpenGLShader::Vertex);
  vert.compileSourceCode(vertexShaderString());
  QOpenGLShader frag(QOpenGLShader::Fragment);
  frag.compileSourceCode(fragmentShaderString());
  bool ok = shaderProgram_.addShader(&vert);
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
  ok = shaderProgram_.addShader(&frag);
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
  ok = shaderProgram_.link();
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
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

  // Set up our shaders.
  createShader();

	// declare our model names
	QVector<std::string> modelFiles{
		"./objects/monkey.obj",
		"./objects/bunny.obj"
	};
	
	// load obj data
	for (std::string& file : modelFiles) {
		models.append(OBJLoader::loadOBJ(file, &shaderProgram_));
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

  shaderProgram_.bind();
  models[currentModel].vao_.bind();
  glDrawElements(GL_TRIANGLES, models[currentModel].vertsToDraw, GL_UNSIGNED_INT, 0);
	models[currentModel].vao_.release();
  shaderProgram_.release();
}
