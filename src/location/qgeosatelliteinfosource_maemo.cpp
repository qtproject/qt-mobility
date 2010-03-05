/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosatelliteinfosource_maemo_p.h"

QTM_BEGIN_NAMESPACE

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
    int len = npe.NewStartTrackingMsg(&msg, client_id_, NpeIf::MethodAll,
                                      NpeIf::OptionNone , 1);

    // cout << "ISI Message len " << len << "\n";
    dbusComm->sendIsiMessage(msg, len);
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
    a = timeout + 1;

}

#if 0
void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
void requestTimeout();
#endif

#include "moc_qgeosatelliteinfosource_maemo_p.cpp"
QTM_END_NAMESPACE

