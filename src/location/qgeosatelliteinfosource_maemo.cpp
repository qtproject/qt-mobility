#include "qsatelliteinfosourcemaemo.h"

QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent) : QGeoSatelliteInfoSource(parent)
{
    client_id_ = -1;
}

int QGeoSatelliteInfoSourceMaemo::init()
{
    int status;

    dbusComm = new DBusComm();
    status = dbusComm->init();
    
    QObject::connect(dbusComm, SIGNAL(npeMessage(const QByteArray &)), 
                     this, SLOT(npeMessages(const QByteArray &)));

    return status;
}

// This method receives messages from DBus.

void QGeoSatelliteInfoSourceMaemo::dbusMessages(const QByteArray &msg)
{
    Q_UNUSED(msg)

    return;
}


void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
#if 0
    int len = npe.NewStartTrackingMsg(&msg,client_id_, NpeIf::MethodAll,
                                      NpeIf::OptionNone , 1);

    // cout << "ISI Message len " << len << "\n";
    dbusComm->sendIsiMessage(msg,len);
    delete [] msg;
#endif

#if 0 // Test !
    QList<QGeoSatelliteInfo> list;
    QGeoSatelliteInfo tmp;

    tmp.setPrnNumber(33);
    tmp.setSignalStrength(15);
    tmp.setProperty(QGeoSatelliteInfo::Azimuth, 45.0);
    tmp.setProperty(QGeoSatelliteInfo::Elevation, 25.5);
    list.append(tmp);
    list.append(tmp);
    list.append(tmp);

    emit satellitesInViewUpdated(list);
    emit satellitesInUseUpdated(list);
#endif


}


void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{


}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
    int a;
    a = timeout +1;

}

#if 0
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void requestTimeout();
#endif
