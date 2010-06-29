#include <QDebug>
#include <QApplication>
#include <qlandmarkmanager.h>
#include "landmarkbrowser.h"

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLandmarkManager *manager = new QLandmarkManager();

    LandmarkBrowser browser;
    browser.show();
    return app.exec();
}
