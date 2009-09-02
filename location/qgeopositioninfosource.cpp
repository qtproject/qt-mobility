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
#include "qgeopositioninfosource.h"

#if defined(Q_OS_SYMBIAN)
#   include "qgeopositioninfosource_s60_p.h"
#endif

/*!
    \class QGeoPositionInfoSource
    \brief The QGeoPositionInfoSource class is an abstract base class for the distribution of positional updates.

    The static function QGeoPositionInfoSource::createSource() creates a default
    position source that is appropriate for the platform, if one is available.
    Otherwise, QGeoPositionInfoSource can be subclassed to create an appropriate
    custom source of position data.

    Users of a QGeoPositionInfoSource subclass can request the current position using
    requestUpdate(), or start and stop regular position updates using
    startUpdates() and stopUpdates(). When an update is available,
    positionUpdate() is emitted. The last known position can be accessed with
    lastUpdate().

    If regular position updates are required, setUpdateInterval() can be used
    to specify how often these updates should be emitted. (If no interval is
    specified, updates are simply provided whenever they are available.)
    For example:

    \code
        // Emit updates every 10 seconds if available
        QGeoPositionInfoSource *source = QGeoPositionInfoSource::createSource();
        source->setUpdateInterval(10000);
    \endcode

    To remove an update interval that was previously set, call
    setUpdateInterval() with a value of 0.

    Note that the position source may have a minimum value requirement for
    update intervals, as returned by minimumIntervalForType().
*/

/*!
    \enum QGeoPositionInfoSource::PositioningMethod
    Defines the types of positioning methods.

    \value SatellitePositioningMethods Satellite-based positioning methods such as GPS.
    \value NonSatellitePositioningMethods Other positioning methods.
    \value AllPositioningMethods A flag that matches all positioning methods.
*/

class QGeoPositionInfoSourcePrivate
{
public:
    int interval;
    QGeoPositionInfoSource::PositioningMethods methods;
};


/*!
    Creates a position source with the specified \a parent.
*/

QGeoPositionInfoSource::QGeoPositionInfoSource(QObject *parent)
    : QObject(parent),
      d(new QGeoPositionInfoSourcePrivate)
{
    d->interval = 0;
    d->methods = 0;
}

/*!
    Destroys the position source.
*/
QGeoPositionInfoSource::~QGeoPositionInfoSource()
{
    delete d;
}

/*!
    Sets the source to emit updates every \a msec milliseconds.
    If \a msec is 0, the source will provide updates using a default interval
    or some internal logic that determines when an update should be provided.

    If \a msec is not 0 and is less than the value returned by
    minimumUpdateInterval(), the interval will be set to the minimum interval.

    Note that implementations may not provide updates at the exact
    interval specified. For example, a given millisecond interval value may be
    rounded to the nearest second if the implementation does not support
    intervals specified to millisecond precision.

    \bold {Note:} When reimplementing this method, subclasses must call the
    base method implementation to ensure updateInterval() returns the correct
    value.
*/
void QGeoPositionInfoSource::setUpdateInterval(int msec)
{
    d->interval = msec;
}

/*!
    Returns the interval value set by setUpdateInterval().

    Returns 0 if no interval has been set.
*/
int QGeoPositionInfoSource::updateInterval() const
{
    return d->interval;
}

/*!
    Sets the preferred positioning methods for this source to \a methods.

    If \a methods includes a method that is not supported by the source, the
    unsupported method will be ignored.

    \bold {Note:} When reimplementing this method, subclasses must call the
    base method implementation to ensure preferredPositioningMethods() returns the correct value.

    \sa supportedPositioningMethods()
*/
void QGeoPositionInfoSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    d->methods = methods;
}

/*!
    Returns the positioning methods set by setPreferredPositioningMethods().
*/
QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSource::preferredPositioningMethods() const
{
    return d->methods;
}

/*!
    Creates and returns a position source with the given \a parent that
    reads from the system's default sources of location data.

    Returns 0 if the system has no default position source.
*/

QGeoPositionInfoSource *QGeoPositionInfoSource::createSource(QObject *parent)
{
#if defined(Q_OS_SYMBIAN)
    return CQGeoPositionInfoSourceS60::NewL(parent);
#else
    Q_UNUSED(parent);
#endif
    return 0;
}

/*!
    \fn QGeoPositionInfo QGeoPositionInfoSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const = 0;

    Returns an update containing the last known position, or a null update
    if none is available.

    If \a fromSatellitePositioningMethodsOnly is true, this returns the last
    known position received from a satellite positioning method; if none
    is available, a null update is returned.
*/

/*!
    \fn virtual PositioningMethods QGeoPositionInfoSource::supportedPositioningMethods() const = 0;

    Returns the positioning methods available to this source.

    \sa setPreferredPositioningMethods()
*/

/*!
    \fn virtual int QGeoPositionInfoSource::minimumUpdateInterval() const = 0;

    Returns the minimum time (in milliseconds) required to retrieve a
    position update.

    This is the minimum value accepted by setUpdateInterval() and 
    requestUpdate().
*/


/*!
    \fn virtual void QGeoPositionInfoSource::startUpdates() = 0;

    Starts emitting updates at regular intervals as specified by setUpdateInterval().

    If setUpdateInterval() has not been called, the source will emit updates
    as soon as they become available.
*/

/*!
    \fn virtual void QGeoPositionInfoSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QGeoPositionInfoSource::requestUpdate(int timeout = 5000);

    Attempts to get the current position and emit positionUpdated() with
    this information. This is useful if you do not need the regular updates
    provided by startUpdates().

    If the current position cannot be found within the given \a timeout
    (in milliseconds) or if \a timeout is less than the value returned by
    minimumUpdateInterval(), requestTimeout() is emitted.

    This does nothing if another update request is in progress. However
    it can be called even if startUpdates() has already been called and
    regular updates are in progress.

    If the source uses multiple positioning methods, it tries to gets the
    current position from the most accurate positioning method within the
    given timeout.
*/

/*!
    \fn void QGeoPositionInfoSource::positionUpdated(const QGeoPositionInfo &update);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update becomes available.

    The \a update value holds the value of the new update.
*/

/*!
    \fn void QGeoPositionInfoSource::requestTimeout();

    Emitted if requestUpdate() was called and the current position could
    not be retrieved within the specified timeout.
*/
