#include "testmodelui.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestModelUi *ui = new TestModelUi;
    ui->show();

    return app.exec();
}
