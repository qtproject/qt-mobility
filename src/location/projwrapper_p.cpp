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

#include "projwrapper_p.h"
#include <proj_api.h>
#include <QSharedData>
#include <QString>

QTM_BEGIN_NAMESPACE

class ProjCoordinateSystemPrivate : public QSharedData
{
public:
    ProjCoordinateSystemPrivate(const QString &projection);
    ProjCoordinateSystemPrivate(const ProjCoordinateSystemPrivate &other);
    ~ProjCoordinateSystemPrivate();

    projPJ projection;
};

ProjCoordinateSystemPrivate::ProjCoordinateSystemPrivate(const QString &projStr)
{
    projection = pj_init_plus(projStr.toLatin1().constData());
}

ProjCoordinateSystemPrivate::ProjCoordinateSystemPrivate(const ProjCoordinateSystemPrivate &other) :
    QSharedData(other), projection(other.projection)
{}

ProjCoordinateSystemPrivate::~ProjCoordinateSystemPrivate()
{}

ProjCoordinateSystem::ProjCoordinateSystem(const QString &projection) :
    d(new ProjCoordinateSystem(projection))
{}

ProjCoordinateSystem::ProjCoordinateSystem(const ProjCoordinateSystem &other) :
    d(other.d)
{}

ProjCoordinateSystem::~ProjCoordinateSystem()
{}

class ProjCoordinatePrivate
{
public:
    ProjCoordinateSystem currentSystem;
    double x;
    double y;
    double z;
};

ProjCoordinate::ProjCoordinate(double x, double y, double z, const ProjCoordinateSystem &system) :
    d(new ProjCoordinatePrivate)
{
    d->x = x;
    d->y = y;
    d->z = z;
    d->currentSystem = system;
}

ProjCoordinate::ProjCoordinate(const ProjCoordinate &other) :
    d(new ProjCoordinatePrivate)
{
    d->x = other.d->x;
    d->y = other.d->y;
    d->z = other.d->z;
    d->currentSystem = other.d->currentSystem;
}

ProjCoordinate::~ProjCoordinate()
{}

double ProjCoordinate::x() const
{
    return d->x;
}

double ProjCoordinate::y() const
{
    return d->y;
}

double ProjCoordinate::z() const
{
    return d->z;
}

bool ProjCoordinate::convert(const ProjCoordinateSystem &system)
{
    int result;
    double x = d->x, y = d->y, z = d->z;
    result = pj_transform(d->currentSystem.d->projection,
                          system.d->projection,
                          1, 1, &x, &y, &z);
    if (result) {
        return false;
    } else {
        d->x = x;
        d->y = y;
        d->z = z;
        d->currentSystem = system;
        return true;
    }
}

QTM_END_NAMESPACE
