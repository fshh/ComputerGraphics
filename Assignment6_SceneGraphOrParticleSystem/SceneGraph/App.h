/**
 * The primary application code.
 */

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

class App : public QMainWindow
{
  Q_OBJECT

public:
  App(int argc, char** argv, QWidget* parent=0);
  virtual ~App();
  
signals:

public slots:

private:
  void buildGui();
};
