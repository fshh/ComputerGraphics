#include "BasicWidget.h"

#include "UnitQuad.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), logger_(this)
{
  setFocusPolicy(Qt::StrongFocus);
  camera_.setPosition(QVector3D(0.5, 0.5, -2.0));
  camera_.setLookAt(QVector3D(0.5, 0.5, 0.0));
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
void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
  // Handle key events here.
  if (keyEvent->key() == Qt::Key_Left) {
    qDebug() << "Left Arrow Pressed";
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_Right) {
    qDebug() << "Right Arrow Pressed";
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_R) {
    camera_.setPosition(QVector3D(0.5, 0.5, -2.0));
    camera_.setLookAt(QVector3D(0.5, 0.5, 0.0));
    update();
  } else {
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
  } else if (mouseAction_ == Zoom) {
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
  QString brickTex = "../../brick.ppm";
  QString grassTex = "../../grass.ppm";
	
	std::shared_ptr<PointLight> red = std::make_shared<PointLight>(
		QVector3D(1.0f, 0.0f, 0.0f),
		QVector3D(0.5f, 0.5f, 0.0f));
	 
	std::shared_ptr<PointLight> green = std::make_shared<PointLight>(
		QVector3D(0.0f, 1.0f, 0.0f),
		QVector3D(0.5f, 0.5f, -2.0f));
		
	std::shared_ptr<PointLight> blue = std::make_shared<PointLight>(
		QVector3D(0.0f, 0.0f, 1.0f),
		QVector3D(0.5f, 0.5f, -4.0f));
	 
	lights_ << red << green << blue;
	
  UnitQuad* backWall = new UnitQuad();
  backWall->init(brickTex, lights_);
  QMatrix4x4 backXform;
  backXform.setToIdentity();
  backXform.scale(1.0, 1.0, -1.0);
  backWall->setModelMatrix(backXform);
  renderables_.push_back(backWall);

  UnitQuad* rightWall = new UnitQuad();
  rightWall->init(brickTex, lights_);
  QMatrix4x4 rightXform;
  rightXform.setToIdentity();
  rightXform.rotate(90.0, 0.0, 1.0, 0.0);
  rightWall->setModelMatrix(rightXform);
  renderables_.push_back(rightWall);

  UnitQuad* leftWall = new UnitQuad();
  leftWall->init(brickTex, lights_);
  QMatrix4x4 leftXform;
  leftXform.setToIdentity();
  leftXform.translate(1.0, 0.0, -1.0);
  leftXform.rotate(-90.0, 0.0, 1.0, 0.0);
  leftWall->setModelMatrix(leftXform);
  renderables_.push_back(leftWall);

  UnitQuad* floor = new UnitQuad();
  floor->init(grassTex, lights_);
  QMatrix4x4 floorXform;
  floorXform.setToIdentity();
  floorXform.translate(-0.5, 0.0, 0.5);
  floorXform.scale(2.0, 2.0, 2.0);
  floorXform.rotate(-90.0, 1.0, 0.0, 0.0);
  floor->setModelMatrix(floorXform);
  renderables_.push_back(floor);

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

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
	
	for (auto light : lights_) {
		light->update(msSinceRestart);
	}

  for (auto renderable : renderables_) {
      renderable->update(msSinceRestart);
      renderable->draw(world_, camera_.getViewMatrix(), camera_.getProjectionMatrix());
  }
  update();
}
