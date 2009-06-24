#include <QtGui/QApplication>
#include <QtPlugin>
#include "imageviewer.h"

Q_IMPORT_PLUGIN(qm3u)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageViewer w;
    w.show();
    return a.exec();
}
