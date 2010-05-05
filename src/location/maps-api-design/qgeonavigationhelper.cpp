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

#include "qgeonavigationhelper.h"
#include "qgeonavigationhelper_p.h"

#include "qgeodistance.h"
#include "qgeoroute.h"
#include "qgeopositioninfo.h"

QTM_BEGIN_NAMESPACE

        /*
        enum NavigationStatus {
            OnRouteStatus,
            OffRouteStatus,
            ArrivedStatus
        };
        */

QGeoNavigationHelper::QGeoNavigationHelper()
{
}

QGeoNavigationHelper::QGeoNavigationHelper(const QGeoNavigationHelper &other)
{
    Q_UNUSED(other);
}

QGeoNavigationHelper::QGeoNavigationHelper(const QGeoRoute &route)
{
    Q_UNUSED(route);
}

QGeoNavigationHelper::~QGeoNavigationHelper()
{
}

QGeoNavigationHelper& QGeoNavigationHelper::operator= (const QGeoNavigationHelper &other)
{
    Q_UNUSED(other);
    return *this;
}

void QGeoNavigationHelper::setRoute(const QGeoRoute &route)
{
    Q_UNUSED(route);
}

void QGeoNavigationHelper::setPositionTolerance(const QGeoDistance &radius)
{
    Q_UNUSED(radius);
}

QGeoDistance QGeoNavigationHelper::positionTolerance() const
{
    return QGeoDistance();
}

QGeoRoute QGeoNavigationHelper::originalRoute() const
{
    return QGeoRoute();
}

QGeoRoute QGeoNavigationHelper::completedRoute() const
{
    return QGeoRoute();
}

QGeoRoute QGeoNavigationHelper::remainingRoute() const
{
    return QGeoRoute();
}

void QGeoNavigationHelper::depart()
{
}

void QGeoNavigationHelper::updatePosition(const QGeoPositionInfo &positionInfo)
{
    Q_UNUSED(positionInfo);
}

void QGeoNavigationHelper::getNextNavigationInstruction()
{
}

void QGeoNavigationHelper::navigationInstructionPerformed(QGeoNavigationInstruction *instruction)
{
    Q_UNUSED(instruction);
}

//need access to route service for this
//void QGeoNavigationHelper::newRouteFromPosition(const QGeoPositionInfo &position) {}
    /*
    signals:
        void nextNavigationInstruction(QGeoNavigationInstruction *instruction);
        void statusChanged(NavigationStatus status);
        void arrived();
        */

#include "moc_qgeonavigationhelper.cpp"

QTM_END_NAMESPACE
