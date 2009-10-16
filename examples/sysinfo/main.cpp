#include <QtGui/QApplication>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    
#ifdef Q_OS_SYMBIAN
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
