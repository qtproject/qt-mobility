
#include <QApplication>
#include <QMainWindow>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>

#include "satellitedialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SatelliteDialog *dialog = new SatelliteDialog(0, 30, SatelliteDialog::ExitOnCancel, SatelliteDialog::OrderByPrnNumber, SatelliteDialog::ScaleToMaxPossible);        

    QGeoPositionInfoSource *posSource = QGeoPositionInfoSource::createDefaultSource(0);
    QGeoSatelliteInfoSource *satSource = QGeoSatelliteInfoSource::createDefaultSource(0);

    dialog->connectSources(posSource, satSource);

    posSource->startUpdates();
    satSource->startUpdates();        

    QTimer::singleShot(1, dialog, SLOT(show()));

    int result = app.exec();

    posSource->stopUpdates();
    satSource->stopUpdates();

    delete posSource;
    delete satSource;
    delete dialog;

    return result;
}
