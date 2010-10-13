#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::Auto);
    viewer.setMainQmlFile(QLatin1String("qrc:/organizer.qml"));
    viewer.show();

    return app.exec();
}
