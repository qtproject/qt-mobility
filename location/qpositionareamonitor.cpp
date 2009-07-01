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
#include "qpositionareamonitor.h"

/*!
    \class QPositionAreaMonitor
    \brief The QPositionAreaMonitor class enables the detection of proximity changes for a specified set of coordinates.

    A QPositionAreaMonitor emits signals when the current position is in
    range, or has moved out of range, of a specified area.

    For example:

    \code
        public:
            MyClass::MyClass()
            {
                QPositionAreaMonitor *monitor = QPositionAreaMonitor::createMonitor();
                connect(monitor, SIGNAL(areaEntered(QPositionUpdate)),
                        this, SLOT(areaEntered(QPositionUpdate)));
                connect(monitor, SIGNAL(areaExited(QPositionUpdate)),
                        this, SLOT(areaExited(QPositionUpdate)));

                QCoordinate bigBenLocation(51.50104, -0.124632);
                monitor->setMonitoredArea(bigBenLocation, 100);
            }

        public slots:
            void areaEntered(const QPositionUpdate &update)
            {
                qDebug() << "Now within 100 meters, current position is" << update.coordinate();
            }

            void areaExited(const QPositionUpdate &update)
            {
                qDebug() << "No longer within 100 meters, current position is" << update.coordinate();
            }
    \endcode
*/


class QPositionAreaMonitorPrivate
{
public:
    QCoordinate coord;
    int radius;
};


/*!
    Creates a monitor with the given \a parent.
*/
QPositionAreaMonitor::QPositionAreaMonitor(QObject *parent)
    : QObject(parent),
      d(new QPositionAreaMonitorPrivate)
{
    d->radius = 0;
}

/*!
    Destroys the monitor.
*/
QPositionAreaMonitor::~QPositionAreaMonitor()
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

void QPositionAreaMonitor::setMonitoredArea(const QCoordinate &coordinate, int radius)
{
    d->coord = coordinate;
    d->radius = radius;
}

/*!
    Returns the coordinate set with setMonitoredArea(), or an invalid
    coordinate if no area has been set.
*/
QCoordinate QPositionAreaMonitor::coordinate() const
{
    return d->coord;
}

/*!
    Returns the radius set with setMonitoredArea(), or 0 if no area
    has been set.
*/
int QPositionAreaMonitor::radius() const
{
    return d->radius;
}

/*!
    Creates and returns a monitor with the given \a parent that
    monitors areas using resources on the underlying system.

    Returns 0 if the system has no support for position monitoring.
*/
QPositionAreaMonitor *QPositionAreaMonitor::createMonitor(QObject *parent)
{
    return 0;
}

/*!
    \fn void QPositionAreaMonitor::areaEntered(const QPositionUpdate &update);

    Emitted when the current position has moved from a position outside the
    monitored area to a position within the monitored area.

    The \a update holds the new position.
*/

/*!
    \fn void QPositionAreaMonitor::areaExited(const QPositionUpdate &update);

    Emitted when the current position has moved from a position within the
    monitored area to a position outside the monitored area.

    The \a update holds the new position.
*/
