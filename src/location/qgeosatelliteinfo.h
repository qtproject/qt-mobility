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
#ifndef QGEOSATELLITEINFO_H
#define QGEOSATELLITEINFO_H

#include "qmobilityglobal.h"

QT_BEGIN_NAMESPACE
class QDebug;
class QDataStream;
QT_END_NAMESPACE

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoPrivate;
class Q_LOCATION_EXPORT QGeoSatelliteInfo
{
public:
    enum Attribute {
        Elevation,
        Azimuth
    };

    QGeoSatelliteInfo();
    QGeoSatelliteInfo(const QGeoSatelliteInfo &other);
    ~QGeoSatelliteInfo();

    QGeoSatelliteInfo &operator=(const QGeoSatelliteInfo &other);

    bool operator==(const QGeoSatelliteInfo &other) const;
    inline bool operator!=(const QGeoSatelliteInfo &other) const {
        return !operator==(other);
    }

    void setPrnNumber(int prn);
    int prnNumber() const;

    void setSignalStrength(int signalStrength);
    int signalStrength() const;

    void setAttribute(Attribute attribute, qreal value);
    qreal attribute(Attribute attribute) const;
    void removeAttribute(Attribute attribute);

    bool hasAttribute(Attribute attribute) const;

private:
#ifndef QT_NO_DEBUG_STREAM
    friend Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info);
#endif
#ifndef QT_NO_DATASTREAM
    friend Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info);
    friend Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info);
#endif
    QGeoSatelliteInfoPrivate *d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info);
#endif

#ifndef QT_NO_DATASTREAM
Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info);
Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info);
#endif

QTM_END_NAMESPACE

QT_END_HEADER

#endif
