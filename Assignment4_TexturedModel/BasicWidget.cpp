#include "BasicWidget.h"
#include "OBJLoader.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), logger_(this), wireframe_(false), currObj_(0)
{
  setFocusPolicy(Qt::StrongFocus);
}

BasicWidget::~BasicWidget()
{
	makeCurrent();
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
		currObj_ = (renderables_.size() + (currObj_ - 1)) % renderables_.size();
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_Right) {
    qDebug() << "Right Arrow Pressed";
		currObj_ = (currObj_ + 1) % renderables_.size();
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_Q) {
		qDebug() << "Quitting...";
		close();
		((QWidget*)parent())->close();
	}
	else if (keyEvent->key() == Qt::Key_W) {
		wireframe_ = !wireframe_;
		qDebug() << "Wireframe" << (wireframe_ ? "enabled" : "disabled");
		makeCurrent();
		glPolygonMode(GL_FRONT_AND_BACK, wireframe_ ? GL_LINE : GL_FILL);
		update();  // We call update after we handle a key press to trigger a redraw when we are ready
  }
	else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}
void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();
	
	qDebug() << "Current path:";
  qDebug() << QDir::currentPath();
	
	// Find objects dir
	QDir dir = QDir::current();
	while (!dir.exists("objects")) {
		dir.cdUp();
	}
	dir.cd("objects");
	
	// Declare object files
	QList<QDir> objs = {
		QDir("house/house_obj.obj"),
		QDir("windmill/windmill.obj"),
		QDir("chapel/chapel_obj.obj"),
		QDir("capsule/capsule.obj")
	};
	
	// Load objects
	qDebug() << "Loaded objects:";
	for (QDir& obj : objs) {
		QString path = dir.filePath(obj.path());
		qDebug() << path;
		Renderable* ren = OBJLoader::loadOBJ(path);
		renderables_.push_back(ren);
	}
	
	// Prepare for render
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
  view_.setToIdentity();
  view_.lookAt(QVector3D(0.0f, 0.0f, 5.0f),
      QVector3D(0.0f, 0.0f, 0.0f),
      QVector3D(0.0f, 1.0f, 0.0f));
  projection_.setToIdentity();
  projection_.perspective(70.f, (float)w/(float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  qint64 msSinceRestart = frameTimer_.restart();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update all renderables, but only draw the selected one
  for (auto renderable : renderables_) {
      renderable->update(msSinceRestart);
  }
	renderables_[currObj_]->draw(view_, projection_, wireframe_);
	
  update();
}
