/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOCOORDINATE_H
#define QGEOCOORDINATE_H

#include "qmobilityglobal.h"

#include <QString>

QT_BEGIN_NAMESPACE
class QDebug;
class QDataStream;
QT_END_NAMESPACE

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinatePrivate;
class Q_LOCATION_EXPORT QGeoCoordinate
{
public:
    enum CoordinateType {
        InvalidCoordinate,
        Coordinate2D,
        Coordinate3D
    };

    enum CoordinateFormat {
        Degrees,
        DegreesWithHemisphere,
        DegreesMinutes,
        DegreesMinutesWithHemisphere,
        DegreesMinutesSeconds,
        DegreesMinutesSecondsWithHemisphere
    };

    QGeoCoordinate();
    QGeoCoordinate(double latitude, double longitude);
    QGeoCoordinate(double latitude, double longitude, double altitude);
    QGeoCoordinate(const QGeoCoordinate &other);
    ~QGeoCoordinate();

    QGeoCoordinate &operator=(const QGeoCoordinate &other);

    bool operator==(const QGeoCoordinate &other) const;
    inline bool operator!=(const QGeoCoordinate &other) const {
        return !operator==(other);
    }

    bool isValid() const;
    CoordinateType type() const;

    void setLatitude(double latitude);
    double latitude() const;

    void setLongitude(double longitude);
    double longitude() const;

    void setAltitude(double altitude);
    double altitude() const;

    qreal distanceTo(const QGeoCoordinate &other) const;
    qreal azimuthTo(const QGeoCoordinate &other) const;

    QGeoCoordinate atDistanceAndAzimuth(qreal distance, qreal azimuth, qreal distanceUp = 0.0) const;

    QString toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const;

private:
    QGeoCoordinatePrivate *d;

    friend class QGeoCoordinatePrivate;
};

#ifndef QT_NO_DEBUG_STREAM
Q_LOCATION_EXPORT QDebug operator<<(QDebug, const QGeoCoordinate &);
#endif

#ifndef QT_NO_DATASTREAM
Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoCoordinate &coordinate);
Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoCoordinate &coordinate);
#endif

QTM_END_NAMESPACE

QT_END_HEADER

#endif
