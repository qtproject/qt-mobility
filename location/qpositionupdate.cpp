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
#include "qpositionupdate.h"

#include <QHash>
#include <QDebug>
#include <QDataStream>

class QPositionUpdatePrivate
{
public:
    QDateTime updateTime;
    QCoordinate coord;
    QHash<int, qreal> doubleProps;
};

/*!
    \class QPositionUpdate
    \brief The QPositionUpdate class contains information gathered on one's global position, direction and velocity at a particular point in time.

    A QPositionUpdate contains, at a minimum, a geographical coordinate and
    a timestamp. It may also have heading and speed measurements as well as
    estimates of the accuracy of the provided data.

    \sa QPositionSource
*/

/*!
    \enum QPositionUpdate::Property
    Defines the properties for a position update.

    \value Heading The bearing to true north, in degrees.
    \value GroundSpeed The ground speed, in metres/sec.
    \value VerticalSpeed The vertical speed, in metres/sec.
    \value MagneticVariation The angle between the horizontal component of the magnetic field and true north, in degrees. Also known as magnetic declination.
    \value HorizontalAccuracy The accuracy of the provided latitude-longitude value, in metres.
    \value VerticalAccuracy The accuracy of the provided altitude value, in metres.
*/

/*!
    Creates a null update.

    \sa isValid()
*/
QPositionUpdate::QPositionUpdate()
    : d(new QPositionUpdatePrivate)
{
}

/*!
    Creates an update with the given \a coordinate and \a updateTime.
*/
QPositionUpdate::QPositionUpdate(const QCoordinate &coordinate, const QDateTime &updateTime)
    : d(new QPositionUpdatePrivate)
{
    d->updateTime = updateTime;
    d->coord = coordinate;
}

/*!
    Creates an update with the values of \a other.
*/
QPositionUpdate::QPositionUpdate(const QPositionUpdate &other)
    : d(new QPositionUpdatePrivate)
{
    operator=(other);
}

/*!
    Destroys an update.
*/
QPositionUpdate::~QPositionUpdate()
{
    delete d;
}

/*!
    Assigns the values from \a other to this update.
*/
QPositionUpdate &QPositionUpdate::operator=(const QPositionUpdate &other)
{
    if (this == &other)
        return *this;

    d->updateTime = other.d->updateTime;
    d->coord = other.d->coord;
    d->doubleProps = other.d->doubleProps;

    return *this;
}

/*!
    Returns true if all of this update's values are the same as those of
    \a other.
*/
bool QPositionUpdate::operator==(const QPositionUpdate &other) const
{
    return d->updateTime == other.d->updateTime
            && d->coord == other.d->coord
            && d->doubleProps == other.d->doubleProps;
}

/*!
    \fn bool QPositionUpdate::operator!=(const QPositionUpdate &other) const

    Returns true if any of this update's values are not the same as those of
    \a other.
*/

/*!
    Returns true if the update's updateTime() and coordinate() values are both valid.

    \sa QCoordinate::isValid(), QDateTime::isValid()
*/
bool QPositionUpdate::isValid() const
{
    return d->updateTime.isValid() && d->coord.isValid();
}

/*!
    Sets the date and time at which this update was received to \a updateTime.

    The \a updateTime must be in UTC time.

    \sa updateTime()
*/
void QPositionUpdate::setUpdateTime(const QDateTime &updateTime)
{
    d->updateTime = updateTime;
}

/*!
    Returns the date and time at which this update was received, in UTC time.

    Returns an invalid QDateTime if no date/time value has been set.

    \sa setUpdateTime()
*/
QDateTime QPositionUpdate::updateTime() const
{
    return d->updateTime;
}

/*!
    Sets the coordinate for this update to \a coordinate.

    \sa coordinate()
*/
void QPositionUpdate::setCoordinate(const QCoordinate &coordinate)
{
    d->coord = coordinate;
}

/*!
    Returns the coordinate for this update.

    Returns an invalid coordinate if no coordinate has been set.

    \sa setCoordinate()
*/
QCoordinate QPositionUpdate::coordinate() const
{
    return d->coord;
}

/*!
    Sets the value for \a property to \a value.

    \sa doubleProperty()
*/
void QPositionUpdate::setDoubleProperty(Property property, qreal value)
{
    d->doubleProps[int(property)] = value;
}

/*!
    Returns the value of the specified \a property as a qreal value.

    Returns -1 if the value has not been set.

    \sa hasProperty(), setDoubleProperty()
*/
qreal QPositionUpdate::doubleProperty(Property property) const
{
    if (d->doubleProps.contains(int(property)))
        return d->doubleProps[int(property)];
    return -1;
}

/*!
    Removes the specified \a property and its value.
*/
void QPositionUpdate::removeProperty(Property property)
{
    d->doubleProps.remove(int(property));
}

/*!
    Returns true if the specified \a property is present in this update.
*/
bool QPositionUpdate::hasProperty(Property property) const
{
    return d->doubleProps.contains(int(property));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QPositionUpdate &update)
{
    dbg.nospace() << "QPositionUpdate(" << update.d->updateTime;
    dbg.nospace() << ", ";
    dbg.nospace() << update.d->coord;

    QList<int> props = update.d->doubleProps.keys();
    for (int i=0; i<props.count(); i++) {
        dbg.nospace() << ", ";
        switch (props[i]) {
            case QPositionUpdate::Heading:
                dbg.nospace() << "Heading=";
                break;
            case QPositionUpdate::GroundSpeed:
                dbg.nospace() << "GroundSpeed=";
                break;
            case QPositionUpdate::VerticalSpeed:
                dbg.nospace() << "VerticalSpeed=";
                break;
            case QPositionUpdate::MagneticVariation:
                dbg.nospace() << "MagneticVariation=";
                break;
            case QPositionUpdate::HorizontalAccuracy:
                dbg.nospace() << "HorizontalAccuracy=";
                break;
            case QPositionUpdate::VerticalAccuracy:
                dbg.nospace() << "VerticalAccuracy=";
                break;
        }
        dbg.nospace() << update.d->doubleProps[props[i]];
    }
    dbg.nospace() << ')';
    return dbg;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const QPositionUpdate &update)
{
    stream << update.d->updateTime;
    stream << update.d->coord;
    stream << update.d->doubleProps;
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &stream, QPositionUpdate &update)
{
    stream >> update.d->updateTime;
    stream >> update.d->coord;
    stream >> update.d->doubleProps;
    return stream;
}
#endif
