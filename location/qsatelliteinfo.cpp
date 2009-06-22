/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qsatelliteinfo.h"

#include <QHash>
#include <QDebug>

class QSatelliteInfoPrivate
{
public:
    int prn;
    int signal;
    QHash<int, qreal> doubleProps;
};

QSatelliteInfo::QSatelliteInfo()
    : d(new QSatelliteInfoPrivate)
{
    d->prn = -1;
    d->signal = -1;
}

QSatelliteInfo::QSatelliteInfo(const QSatelliteInfo &other)
    : d(new QSatelliteInfoPrivate)
{
    operator=(other);
}

QSatelliteInfo::~QSatelliteInfo()
{
    delete d;
}

QSatelliteInfo &QSatelliteInfo::operator=(const QSatelliteInfo &other)
{
    if (this == &other)
        return *this;

    d->prn = other.d->prn;
    d->signal = other.d->signal;
    d->doubleProps = other.d->doubleProps;
    return *this;
}

bool QSatelliteInfo::operator==(const QSatelliteInfo &other) const
{
    return d->prn == other.d->prn
            && d->signal == other.d->signal
            && d->doubleProps == other.d->doubleProps;
}

void QSatelliteInfo::setPrnNumber(int prn)
{
    d->prn = prn;
}

int QSatelliteInfo::prnNumber() const
{
    return d->prn;
}

void QSatelliteInfo::setSignalStrength(int signalStrength)
{
    d->signal = signalStrength;
}

int QSatelliteInfo::signalStrength() const
{
    return d->signal;
}

void QSatelliteInfo::setDoubleProperty(Property property, qreal value)
{
    d->doubleProps[int(property)] = value;
}

qreal QSatelliteInfo::doubleProperty(Property property) const
{
    if (d->doubleProps.contains(int(property)))
        return d->doubleProps[int(property)];
    return -1;
}

void QSatelliteInfo::removeProperty(Property property)
{
    d->doubleProps.remove(int(property));
}

bool QSatelliteInfo::hasProperty(Property property) const
{
    return d->doubleProps.contains(int(property));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QSatelliteInfo &info)
{
    dbg.nospace() << "QSatelliteInfo(PRN=" << info.d->prn;
    dbg.nospace() << ", signal-strength=";
    dbg.nospace() << info.d->signal;

    QList<int> props = info.d->doubleProps.keys();
    for (int i=0; i<props.count(); i++) {
        dbg.nospace() << ", ";
        switch (props[i]) {
            case QSatelliteInfo::Elevation:
                dbg.nospace() << "Elevation=";
                break;
            case QSatelliteInfo::Azimuth:
                dbg.nospace() << "Azimuth=";
                break;
        }
        dbg.nospace() << info.d->doubleProps[props[i]];
    }
    dbg.nospace() << ')';
    return dbg;
}
#endif
