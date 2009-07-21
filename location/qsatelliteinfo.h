/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QSATELLITEINFO_H
#define QSATELLITEINFO_H

#include "qlocationglobal.h"

class QDebug;
class QSatelliteInfoPrivate;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QSatelliteInfo
{
public:
    enum Property {
        Elevation,
        Azimuth
    };

    QSatelliteInfo();
    QSatelliteInfo(const QSatelliteInfo &other);
    ~QSatelliteInfo();

    QSatelliteInfo &operator=(const QSatelliteInfo &other);

    bool operator==(const QSatelliteInfo &other) const;
    inline bool operator!=(const QSatelliteInfo &other) const { return !operator==(other); }

    void setPrnNumber(int prn);
    int prnNumber() const;

    void setSignalStrength(int signalStrength);
    int signalStrength() const;

    void setProperty(Property property, qreal value);
    qreal property(Property property) const;
    void removeProperty(Property property);

    bool hasProperty(Property property) const;

private:
#ifndef QT_NO_DEBUG_STREAM
    friend Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QSatelliteInfo &info);;
#endif
    QSatelliteInfoPrivate *d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QSatelliteInfo &info);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
