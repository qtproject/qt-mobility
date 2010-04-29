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
#ifndef QGEONAVIGATIONHELPER_H
#define QGEONAVIGATIONHELPER_H

#include "qgeomapwidget.h"

#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoPositionInfo;
class QGeoRoute;
class QGeoNavigationInstruction;

class QGeoNavigationHelperPrivate;

// TODO - name could be better

class Q_LOCATION_EXPORT QGeoNavigationHelper : public QObject {

    Q_OBJECT

public:
    enum NavigationStatus {
        OnRouteStatus,
        OffRouteStatus,
        ArrivedStatus
    };

    QGeoNavigationHelper();
    QGeoNavigationHelper(const QGeoNavigationHelper &other);
    QGeoNavigationHelper(const QGeoRoute &route);
    ~QGeoNavigationHelper();

    QGeoNavigationHelper& operator = (const QGeoNavigationHelper &other);

    // TODO - decide on using routes or route services
    // route services allow us to request updates when we're off track
    // candidate for merging with routing service - see how the JSR does things

    void setRoute(const QGeoRoute &route);

    void setPositionTolerance(double radius, QGeoMapWidget::DistanceUnits units = QGeoMapWidget::Metres);
    double positionTolerance(QGeoMapWidget::DistanceUnits units = QGeoMapWidget::Metres);

    QGeoRoute originalRoute() const;
    QGeoRoute completedRoute() const;
    QGeoRoute remainingRoute() const;

public slots:
    void depart();
    void updatePosition(const QGeoPositionInfo &positionInfo);

    void getNextNavigationInstruction();
    void navigationInstructionPerformed(QGeoNavigationInstruction *instruction);

    //need access to route service for this
    //void newRouteFromPosition(const QGeoPositionInfo &position);

signals:
    void nextNavigationInstruction(QGeoNavigationInstruction *instruction);
    void statusChanged(NavigationStatus status);
    void arrived();

private:
    QGeoNavigationHelperPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGeoNavigationHelper)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
