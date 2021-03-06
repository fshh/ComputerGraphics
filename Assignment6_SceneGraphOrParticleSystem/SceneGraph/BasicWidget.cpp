#include "BasicWidget.h"
#include "SolarSystem.h"

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
	case DrawMode::LIGHTING_DEBUG:
		return "Lighting debug";
	}
}

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), camera_(QVector3D(0, 5, 35)),
	logger_(this), drawMode_(DrawMode::DEFAULT), paused_(false), mouseAction_(MouseControl::NoAction)
{
  setFocusPolicy(Qt::StrongFocus);
}

BasicWidget::~BasicWidget()
{
	makeCurrent();
	delete root;
}

void BasicWidget::updateScene(const qint64 msSinceLastFrame)
{
	root->update(msSinceLastFrame);
}

void BasicWidget::renderScene()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glClearColor(0.01f, 0.01f, 0.01f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw our scene
	drawNode(root);

	// Swap buffers
	update();
}

//////////////////////////////////////////////////////////////////////
// Privates
///////////////////////////////////////////////////////////////////////
// Protected
void BasicWidget::drawNode(SceneNode* node)
{
	if (node->getRenderable())
	{
		QMatrix4x4 scale = QMatrix4x4();
		scale.scale(node->getModelScale());
		const QMatrix4x4 worldSpaceModelMatrix = node->getWorldTransform() * scale;

		node->draw(worldSpaceModelMatrix, camera_.position(), camera_.getViewMatrix(), camera_.getProjectionMatrix(), drawMode_);
	}

	for (auto it = node->begin(); it != node->end(); ++it)
	{
		drawNode(*it);
	}
}

void BasicWidget::quit(QString message, int exitCode) {
	qDebug() << "Quitting:" << message;
	close();
	((QWidget*)parent())->close();
	delete this;
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
		case Qt::Key_L:
			setDrawMode(DrawMode::LIGHTING_DEBUG);
			break;
		case Qt::Key_Space:
			paused_ = !paused_;
			qDebug() << "Rotation" << (paused_ ? "paused." : "unpaused.");
			break;
		case Qt::Key_R:
			camera_.reset();
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
		mouseAction_ = MouseControl::Rotate;
	}
	else if (mouseEvent->button() == Qt::RightButton) {
		mouseAction_ = MouseControl::Zoom;
	}
	lastMouseLoc_ = mouseEvent->pos();
}

void BasicWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (mouseAction_ == MouseControl::NoAction) {
		return;
	}
	QPoint delta = mouseEvent->pos() - lastMouseLoc_;
	lastMouseLoc_ = mouseEvent->pos();
	if (mouseAction_ == MouseControl::Rotate) {
		float rotateScale = 0.1f;
		QPoint scaledDelta = delta * rotateScale;
		camera_.rotateAboutFocus(delta.x(), delta.y());
	}
	else if (mouseAction_ == MouseControl::Zoom) {
		float zoomScale = 0.1f;
		camera_.zoomCamera(delta.y() * zoomScale);
	}
	update();
}

void BasicWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	mouseAction_ = MouseControl::NoAction;
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();
	
	qDebug() << "Current path:";
  qDebug() << QDir::currentPath();
	
	// Load solar system
	root = new SolarSystem();

	if (!root) {
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
		"    Press spacebar to toggle the model rotation.\n" <<
		"  Draw Modes:\n" <<
		"    Press W to enter wireframe mode. Press again to return to default.\n" <<
		"    Press T to enter texture debug mode. Press again to return to default.\n" <<
		"    Press N to enter normal debug mode. Press again to return to default.\n" <<
		"    Press L to enter lighting debug mode. Press again to return to default.\n" <<
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

	updateScene(msSinceRestart);
	
	renderScene();
}
