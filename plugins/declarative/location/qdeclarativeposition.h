/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
***************************************************************************/

#ifndef QDECLARATIVELOCATION_H
#define QDECLARATIVELOCATION_H

#include <QtCore>
#include <QDateTime>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativePosition : public QObject {
    Q_OBJECT
    // Q_INTERFACES(QDeclarativePosition)

    Q_PROPERTY(QUrl nmeaSource READ nmeaSource WRITE setNmeaSource)
    Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)
    Q_PROPERTY(double latitude READ latitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longtitude READ longtitude NOTIFY longtitudeChanged)
    Q_PROPERTY(double altitude READ altitude NOTIFY altitudeChanged)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(bool positionLatest READ isPositionLatest NOTIFY positionLatestChanged);
    Q_PROPERTY(bool altitudeLatest READ isAltitudeLatest NOTIFY altitudeLatestChanged);
    Q_PROPERTY(bool speedLatest READ isSpeedLatest NOTIFY speedLatestChanged);
    Q_PROPERTY(PositioningMethod positioningMethod READ positioningMethod NOTIFY positioningMethodChanged);
    Q_ENUMS(PositioningMethod);

public:

    enum PositioningMethod {
        NoPositioningMethod = 0,
        SatellitePositioningMethod = QGeoPositionInfoSource::SatellitePositioningMethods,
        NonSatellitePositioningMethod = QGeoPositionInfoSource::NonSatellitePositioningMethods,
        AllPositioningMethods = QGeoPositionInfoSource::AllPositioningMethods
    };

    QDeclarativePosition();
    ~QDeclarativePosition();

    void setNmeaSource(const QUrl& nmeaSource);
    void setUpdateInterval(int updateInterval);

    QDateTime timestamp() const;
    QUrl nmeaSource() const;
    double latitude() const;
    double longtitude() const;
    double speed() const;
    double altitude() const;
    bool isPositionLatest() const;
    bool isAltitudeLatest() const;
    bool isSpeedLatest() const;
    int updateInterval() const;
    PositioningMethod positioningMethod() const;

public Q_SLOTS:
    void update();
    void startUpdates();
    void stopUpdates();

Q_SIGNALS:
    void positionUpdated();
    void timestampChanged(QDateTime timestamp);
    void latitudeChanged(double latitude);
    void longtitudeChanged(double longtitude);
    void speedChanged(double speed);
    void altitudeChanged(double altitude);
    void positionLatestChanged(bool isLatest);
    void altitudeLatestChanged(bool isLatest);
    void speedLatestChanged(bool isLatest);
    void positioningMethodChanged(PositioningMethod positioningMethod);

private Q_SLOTS:
    void positionUpdateReceived(const QGeoPositionInfo& update);

private:
    void updateTimestamp(const QGeoPositionInfo& update, bool& timestampChanged);
    bool updatePosition(const QGeoPositionInfo& update, bool& positionChanged);
    bool updateAltitude(const QGeoPositionInfo& update, bool& altitudeChanged);
    bool updateSpeed(const QGeoPositionInfo& update, bool& speedChanged);

private:
    QGeoPositionInfoSource* m_positionSource;
    QGeoPositionInfo m_positionInfo;
    QUrl m_nmeaSource;
    QDateTime m_timestamp;
    double m_latitude;
    double m_longtitude;
    double m_altitude;
    double m_speed;
    bool m_updatesOngoing;
    bool m_positionLatest;
    bool m_altitudeLatest;
    bool m_speedLatest;
    int m_updateInterval;
};


QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativePosition));
Q_DECLARE_INTERFACE(QtMobility::QDeclarativePosition, "QtMobility.location/1.0")

#endif
