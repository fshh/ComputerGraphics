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
BasicWidget::BasicWidget(QList<QDir> objectFiles, QWidget* parent) : QOpenGLWidget(parent), objectFiles_(objectFiles), logger_(this), drawMode_(DrawMode::DEFAULT), currObj_(0), paused_(false)
{
  setFocusPolicy(Qt::StrongFocus);
	camera_.setPosition(QVector3D(0.0, 0.0, 4.0));
	camera_.setLookAt(QVector3D(0.0, 0.0, 0.0));
	world_.setToIdentity();
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
		case Qt::Key_Space:
			paused_ = !paused_;
			qDebug() << "Rotation" << (paused_ ? "paused." : "unpaused.");
			break;
		case Qt::Key_R:
			camera_.setPosition(QVector3D(0.0, 0.0, 4.0));
			camera_.setLookAt(QVector3D(0.0, 0.0, 0.0));
			qDebug() << "Camera orientation reset.";
			update();
			break;
		default:
			qDebug() << "You Pressed an unsupported Key!";
			break;
	}
}

void BasicWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton) {
		mouseAction_ = Rotate;
	}
	else if (mouseEvent->button() == Qt::RightButton) {
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
			dir.filePath("brickWall_lowRes/brickWall.obj"),
			//dir.filePath("brickWall_highRes/brickWall.obj"),
			dir.filePath("house/house_obj.obj"),
			dir.filePath("windmill/windmill.obj"),
			dir.filePath("chapel/chapel_obj.obj")
		};
	}
	
	// Load objects
	qDebug() << "Loading objects...";
	for (QDir& obj : objectFiles_) {
		QString path = obj.path();
		qDebug() << "  Loading" << path.right(path.size() - path.lastIndexOf("/") - 1);
		Renderable* ren = OBJLoader::loadOBJ(path);
		if (ren) {
			qDebug() << "    Success";
			renderables_.push_back(ren);
		}
		else {
			qDebug() << "    Failure";
		}
	}

	if (renderables_.isEmpty()) {
		quit("No objects loaded correctly", 1);
	}

	// Print instructions
	qDebug() << "\n\nPass object files to the program like so: ./App \"path/to/object1.obj\" \"path/to/object2.obj\" ...";
	qDebug() << "If no object files are provided, the program will automatically load in the brick wall, house, windmill, and chapel.";
	qDebug() <<
		"Hotkeys:\n" <<
		"  Camera Controls:\n" <<
		"    Left click and drag to move the camera.\n" <<
		"    Right click and drag to zoom the camera in/out.\n" <<
		"    Press R to reset the camera to its original orientation.\n" <<
		"  Model Controls:\n" <<
		"    Press left and right arrow keys to cycle through models.\n" <<
		"    Press spacebar to toggle the model rotation.\n" <<
		"  Draw Modes:\n" <<
		"    Press W to enter wireframe mode. Press again to return to default.\n" <<
		"    Press T to enter texture debug mode. Press again to return to default.\n" <<
		"    Press N to enter normal debug mode. Press again to return to default.\n" <<
		"    Press D to return to default drawing mode.\n" <<
		"  Quitting:\n" <<
		"    Press Q to quit.\n\n";
	
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

	camera_.setPerspective(70.f, (float)w / (float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  qint64 msSinceRestart = frameTimer_.restart();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.1f, 0.1f, 0.1f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update all renderables, but only draw the selected one
	for (int ii = 0; ii < renderables_.size(); ++ii) {
		Renderable* renderable = renderables_[ii];
		if (!paused_) { 
			renderable->update(msSinceRestart); 
		}
		if (ii == currObj_) {
			renderable->draw(world_, camera_.getViewMatrix(), camera_.getProjectionMatrix(), drawMode_);
		}
  }
	
  update();
}
