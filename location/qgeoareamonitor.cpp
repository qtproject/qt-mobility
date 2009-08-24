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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeoareamonitor.h"

/*!
    \class QGeoAreaMonitor
    \brief The QGeoAreaMonitor class enables the detection of proximity changes for a specified set of coordinates.

    A QGeoAreaMonitor emits signals when the current position is in
    range, or has moved out of range, of a specified area.

    For example:

    \code
        public:
            MyClass::MyClass()
            {
                QGeoAreaMonitor *monitor = QGeoAreaMonitor::createMonitor();
                connect(monitor, SIGNAL(areaEntered(QGeoPositionInfo)),
                        this, SLOT(areaEntered(QGeoPositionInfo)));
                connect(monitor, SIGNAL(areaExited(QGeoPositionInfo)),
                        this, SLOT(areaExited(QGeoPositionInfo)));

                QGeoCoordinate bigBenLocation(51.50104, -0.124632);
                monitor->setMonitoredArea(bigBenLocation, 100);
            }

        public slots:
            void areaEntered(const QGeoPositionInfo &update)
            {
                qDebug() << "Now within 100 meters, current position is" << update.coordinate();
            }

            void areaExited(const QGeoPositionInfo &update)
            {
                qDebug() << "No longer within 100 meters, current position is" << update.coordinate();
            }
    \endcode
*/


class QGeoAreaMonitorPrivate
{
public:
    QGeoCoordinate coord;
    int radius;
};


/*!
    Creates a monitor with the given \a parent.
*/
QGeoAreaMonitor::QGeoAreaMonitor(QObject *parent)
    : QObject(parent),
      d(new QGeoAreaMonitorPrivate)
{
    d->radius = 0;
}

/*!
    Destroys the monitor.
*/
QGeoAreaMonitor::~QGeoAreaMonitor()
{
}

/*!
    Sets the area to be monitored to the area specified by \a coordinate
    with a radius of \a radius.

    If the current position is within the specified area, areaEntered()
    is emitted immediately.

    Note: Subclass implementations of this method should call the base
    implementation to ensure coordinate() and radius() return the correct
    values.
*/

void QGeoAreaMonitor::setMonitoredArea(const QGeoCoordinate &coordinate, int radius)
{
    d->coord = coordinate;
    d->radius = radius;
}

/*!
    Returns the coordinate set with setMonitoredArea(), or an invalid
    coordinate if no area has been set.
*/
QGeoCoordinate QGeoAreaMonitor::coordinate() const
{
    return d->coord;
}

/*!
    Returns the radius set with setMonitoredArea(), or 0 if no area
    has been set.
*/
int QGeoAreaMonitor::radius() const
{
    return d->radius;
}

/*!
    Creates and returns a monitor with the given \a parent that
    monitors areas using resources on the underlying system.

    Returns 0 if the system has no support for position monitoring.
*/
QGeoAreaMonitor *QGeoAreaMonitor::createMonitor(QObject *parent)
{
    return 0;
}

/*!
    \fn void QGeoAreaMonitor::areaEntered(const QGeoPositionInfo &update);

    Emitted when the current position has moved from a position outside the
    monitored area to a position within the monitored area.

    The \a update holds the new position.
*/

/*!
    \fn void QGeoAreaMonitor::areaExited(const QGeoPositionInfo &update);

    Emitted when the current position has moved from a position within the
    monitored area to a position outside the monitored area.

    The \a update holds the new position.
*/
