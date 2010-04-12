#include <QApplication>
#include <QObject>
#include "controlpanel.h"
#include "location.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ControlPanel* gui = new ControlPanel();
    Location location(gui);
    gui->show();
    QObject::connect(gui, SIGNAL(quit()), &location, SLOT(shutdown()) );

    return app.exec();
} 

