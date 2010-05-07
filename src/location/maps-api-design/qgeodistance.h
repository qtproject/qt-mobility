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
****************************************************************************/

#ifndef QGEODISTANCE_H
#define QGEODISTANCE_H

#include "qmobilityglobal.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoDistancePrivate;

class Q_LOCATION_EXPORT QGeoDistance
{
public:
    enum DistanceUnits {
        Metres,
        Kilometres,
        Miles
    };

    QGeoDistance(double magnitude = 0.0, DistanceUnits units = Metres);
    QGeoDistance(const QGeoDistance &other);
    ~QGeoDistance();

    QGeoDistance& operator= (const QGeoDistance &other);

    void setMagnitude(double magnitude);
    double magnitude() const;

    // convert value first
    void setUnits(DistanceUnits units, bool convert = false);
    DistanceUnits units() const;

    double convert(DistanceUnits units) const;

private:
    QGeoDistancePrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoDistance);
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
