
#include <QApplication>
#include <QMainWindow>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>

#include "qgeosatellitedialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QGeoSatelliteDialog *dialog = new QGeoSatelliteDialog(0, 30, QGeoSatelliteDialog::ExitOnCancel, QGeoSatelliteDialog::OrderByPrnNumber, QGeoSatelliteDialog::ScaleToMaxPossible);    
    dialog->setModal(true);

    QGeoPositionInfoSource *posSource = QGeoPositionInfoSource::createDefaultSource(0);
    QGeoSatelliteInfoSource *satSource = QGeoSatelliteInfoSource::createDefaultSource(0);

    QObject::connect(posSource, SIGNAL(positionUpdated(const QGeoPositionInfo &)), dialog, SLOT(positionUpdated(const QGeoPositionInfo &)));
    QObject::connect(satSource, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)), dialog, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QObject::connect(satSource, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)), dialog, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

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
