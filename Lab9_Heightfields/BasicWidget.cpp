#include "BasicWidget.h"

#include "TerrainQuad.h"
#include "UnitQuad.h"

static QString drawModeToString(DrawMode drawMode) {
	switch (drawMode) {
	case DrawMode::DEFAULT:
		return "Default";
	case DrawMode::WIREFRAME:
		return "Wireframe";
	case DrawMode::TEX_DEBUG:
		return "Texture debug";
	case DrawMode::NORM_DEBUG:
		return "Normal debug";
	}
}

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), logger_(this), isFilled_(true), drawMode_(DrawMode::DEFAULT)
{
  setFocusPolicy(Qt::StrongFocus);
  camera_.setPosition(QVector3D(0.5, 0.5, -0.5));
  camera_.setLookAt(QVector3D(0.0, 0.5, 2.0));
  world_.setToIdentity();
}

BasicWidget::~BasicWidget()
{
    for (auto renderable : renderables_) {
        delete renderable;
    }
    renderables_.clear();
}

//////////////////////////////////////////////////////////////////////
// Privates
///////////////////////////////////////////////////////////////////////
// Protected
void BasicWidget::quit(QString message, int exitCode) {
	qDebug() << "Quitting:" << message;
	close();
	((QWidget*)parent())->close();
	exit(exitCode);
}

void BasicWidget::setDrawMode(DrawMode drawMode) {
	// If draw mode is already set to given mode, set to default instead
	drawMode_ = drawMode == drawMode_ ? DrawMode::DEFAULT : drawMode;
	qDebug() << "Draw mode:" << drawModeToString(drawMode_);
	makeCurrent();
	glPolygonMode(GL_FRONT_AND_BACK, drawMode_ == DrawMode::WIREFRAME ? GL_LINE : GL_FILL);
	update();
}

void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
  // Handle key events here.
	switch (keyEvent->key()) {
		case Qt::Key_Left:
			qDebug() << "Left Arrow Pressed";
			update();
			break;
		case Qt::Key_Right:
			qDebug() << "Left Arrow Pressed";
			update();
			break;
		case Qt::Key_R:
			camera_.setPosition(QVector3D(0.5, 0.5, -0.5));
			camera_.setLookAt(QVector3D(0.0, 0.5, 2.0));
			update();
			break;
		case Qt::Key_Q:
			quit("Manual quit", 0);
			break;
		case Qt::Key_W:
			setDrawMode(DrawMode::WIREFRAME);
			break;
		default:
			qDebug() << "You Pressed an unsupported Key!";
	}
}

void BasicWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
  if (mouseEvent->button() == Qt::LeftButton) {
    mouseAction_ = Rotate;
  } else if (mouseEvent->button() == Qt::RightButton) {
    mouseAction_ = Zoom;
  }
  lastMouseLoc_ = mouseEvent->pos();
}

void BasicWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (mouseAction_ == NoAction) {
		return;
	}
	QPoint delta = mouseEvent->pos() - lastMouseLoc_;
	lastMouseLoc_ = mouseEvent->pos();
	if (mouseAction_ == Rotate) {
		float rotateScale = 0.1f;
		QPoint scaledDelta = delta * rotateScale;
		camera_.rotateAboutFocus(delta.x(), delta.y());
	}
	else if (mouseAction_ == Zoom) {
		float zoomScale = 0.1f;
		camera_.zoomCamera(delta.y() * zoomScale);
	}
	update();
}

void BasicWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    mouseAction_ = NoAction;
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  qDebug() << QDir::currentPath();
  // TODO:  You may have to change these paths.
  QString terrainTex = "../../colormap.ppm";

  TerrainQuad* terrain = new TerrainQuad();
  terrain->init(terrainTex);
  QMatrix4x4 floorXform;
  floorXform.setToIdentity();
	floorXform.scale(2.0, 2.0, 2.0);
  floorXform.translate(-0.5, 0.0, 0.5);
  terrain->setModelMatrix(floorXform);
  renderables_.push_back(terrain);

  glViewport(0, 0, width(), height());
  frameTimer_.start();
}

void BasicWidget::resizeGL(int w, int h)
{
    if (!logger_.isLogging()) {
        logger_.initialize();
        // Setup the logger for real-time messaging
        connect(&logger_, &QOpenGLDebugLogger::messageLogged, [=]() {
            const QList<QOpenGLDebugMessage> messages = logger_.loggedMessages();
            for (auto msg : messages) {
                qDebug() << msg;
            }
            });
        logger_.startLogging();
    }
  glViewport(0, 0, w, h);

  camera_.setPerspective(70.f, (float)w / (float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  qint64 msSinceRestart = frameTimer_.restart();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(61.f / 255.f, 84.f / 255.f, 103.f / 255.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  for (auto renderable : renderables_) {
      //renderable->update(msSinceRestart);
      // TODO:  Understand that the camera is now governing the view and projection matrices
      renderable->draw(world_, camera_.getViewMatrix(), camera_.getProjectionMatrix());
  }
  update();
}
