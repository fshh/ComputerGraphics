#include "BasicWidget.h"
#include "OBJLoader.h"

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
BasicWidget::BasicWidget(QList<QDir> objectFiles, QWidget* parent) : QOpenGLWidget(parent), objectFiles_(objectFiles), logger_(this), drawMode_(DrawMode::DEFAULT), currObj_(0)
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
			currObj_ = (renderables_.size() + (currObj_ - 1)) % renderables_.size();
			update();  // We call update after we handle a key press to trigger a redraw when we are ready
			break;
		case Qt::Key_Right:
			qDebug() << "Right Arrow Pressed";
			currObj_ = (renderables_.size() + (currObj_ + 1)) % renderables_.size();
			update();  // We call update after we handle a key press to trigger a redraw when we are ready
			break;
		case Qt::Key_Q:
			quit("Manual quit", 0);
			break;
		case Qt::Key_D:
			setDrawMode(DrawMode::DEFAULT);
			break;
		case Qt::Key_W:
			setDrawMode(DrawMode::WIREFRAME);
			break;
		case Qt::Key_T:
			setDrawMode(DrawMode::TEX_DEBUG);
			break;
		case Qt::Key_N:
			setDrawMode(DrawMode::NORM_DEBUG);
			break;
		default:
			qDebug() << "You Pressed an unsupported Key!";
			break;
	}
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();
	
	qDebug() << "Current path:";
  qDebug() << QDir::currentPath();
	
	// Declare object files if none are given
	if (objectFiles_.isEmpty()) {
		// Find objects dir
		QDir dir = QDir::current();
		while (!dir.exists("objects")) {
			dir.cdUp();
		}
		dir.cd("objects");

		objectFiles_ = {
			dir.filePath(QDir("house/house_obj.obj").path()),
			dir.filePath(QDir("windmill/windmill.obj").path()),
			dir.filePath(QDir("chapel/chapel_obj.obj").path()),
			dir.filePath(QDir("capsule/capsule.obj").path())
		};
	}
	
	// Load objects
	qDebug() << "Loaded objects:";
	for (QDir& obj : objectFiles_) {
		QString path = obj.path();
		Renderable* ren = OBJLoader::loadOBJ(path);
		if (ren) {
			qDebug() << path;
			renderables_.push_back(ren);
		}
	}

	if (renderables_.isEmpty()) {
		quit("No objects loaded correctly", 1);
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
  projection_.perspective(70.f, (float)w/(float)h, 0.001f, 1000.0f);
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
	for (int ii = 0; ii < renderables_.size(); ++ii) {
		Renderable* renderable = renderables_[ii];
		renderable->update(msSinceRestart);
		if (ii == currObj_) {
			renderable->draw(view_, projection_, drawMode_);
		}
  }
	
  update();
}
