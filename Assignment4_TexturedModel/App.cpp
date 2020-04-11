#include "App.h"
#include "BasicWidget.h"

#include <iostream>

App::App(int argc, char** argv, QWidget* parent) : QMainWindow(parent)
{
	QList<QDir> objectFiles;
	for (int ii = 1; ii < argc; ++ii) {
		objectFiles << QDir(argv[ii]);
	}

  buildGui(objectFiles);
}

App::~App()
{}

void App::buildGui(QList<QDir> objectFiles)
{
  // A simple menubar.
  QMenuBar* menu = menuBar();
  QMenu* file = menu->addMenu("File");
  QAction* exit = file->addAction("Quit", [this]() {close();});

  // Our basic widget.
  BasicWidget* widget = new BasicWidget(objectFiles, this);
  setCentralWidget(widget);
}
