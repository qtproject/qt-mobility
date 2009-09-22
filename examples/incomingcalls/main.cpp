#include "testmodelui.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestModelUi ui;
    ui.show();
    int retn = app.exec();
    return retn;
}
