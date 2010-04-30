/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qgeopositioninfosource.h>

#if defined(Q_OS_SYMBIAN)
#   include "qgeopositioninfosource_s60_p.h"
#elif defined(Q_OS_WINCE)
#   include "qgeopositioninfosource_wince_p.h"
#elif defined(Q_WS_MAEMO_6)
#   include "qgeopositioninfosource_maemo_p.h"
#elif defined(Q_WS_MAEMO_5)
#   include "qgeopositioninfosource_maemo5_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoPositionInfoSource
    \brief The QGeoPositionInfoSource class is an abstract base class for the distribution of positional updates.
    \ingroup location

    The static function QGeoPositionInfoSource::createDefaultSource() creates a default
    position source that is appropriate for the platform, if one is available.
    Otherwise, QGeoPositionInfoSource can be subclassed to create an appropriate
    custom source of position data.

    Users of a QGeoPositionInfoSource subclass can request the current position using
    requestUpdate(), or start and stop regular position updates using
    startUpdates() and stopUpdates(). When an update is available,
    positionUpdated() is emitted. The last known position can be accessed with
    lastKnownPosition().

    If regular position updates are required, setUpdateInterval() can be used
    to specify how often these updates should be emitted. If no interval is
    specified, updates are simply provided whenever they are available.
    For example:

    \code
        // Emit updates every 10 seconds if available
        QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource();
        if (source)
            source->setUpdateInterval(10000);
    \endcode

    To remove an update interval that was previously set, call
    setUpdateInterval() with a value of 0.

    Note that the position source may have a minimum value requirement for
    update intervals, as returned by minimumUpdateInterval().

    \warning On Windows CE it is not possible to detect if a device is GPS enabled.
    The default position source on a Windows CE device without GPS support will never provide any position data.
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
    \property QGeoPositionInfoSource::updateInterval
    \brief This property holds the requested interval in milliseconds between each update.

    If the update interval is not set (or is set to 0) the
    source will provide updates as often as necessary.

    If the update interval is set, the source will provide updates at an
    interval as close to the requested interval as possible. If the requested
    interval is less than the minimumUpdateInterval(),
    the minimum interval is used instead.

    Changes to the update interval will happen as soon as is practical, however the 
    time the change takes may vary between implementations.  Whether or not the elapsed 
    time from the previous interval is counted as part of the new interval is also 
    implementation dependent.

    The default value for this property is 0.

    Note: Subclass implementations must call the base implementation of
    setUpdateInterval() so that updateInterval() returns the correct value.
*/
void QGeoPositionInfoSource::setUpdateInterval(int msec)
{
    d->interval = msec;
}

int QGeoPositionInfoSource::updateInterval() const
{
    return d->interval;
}

/*!
    Sets the preferred positioning methods for this source to \a methods.

    If \a methods includes a method that is not supported by the source, the
    unsupported method will be ignored.

    If \a methods does not include any methods supported by the source, the
    preferred methods will be set to the set of methods which the source supports.

    \bold {Note:} When reimplementing this method, subclasses must call the
    base method implementation to ensure preferredPositioningMethods() returns the correct value.

    \sa supportedPositioningMethods()
*/
void QGeoPositionInfoSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    d->methods = methods & supportedPositioningMethods();
    if (d->methods == 0) {
        d->methods = supportedPositioningMethods();
    }
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

QGeoPositionInfoSource *QGeoPositionInfoSource::createDefaultSource(QObject *parent)
{
#if defined(Q_OS_SYMBIAN)
    QGeoPositionInfoSource *ret = NULL;
    TRAPD(error, ret = CQGeoPositionInfoSourceS60::NewL(parent));
    if (error != KErrNone)
        return 0;
    return ret;
#elif defined(Q_OS_WINCE)
    return new QGeoPositionInfoSourceWinCE(parent);
#elif (defined(Q_WS_MAEMO_6)) || (defined(Q_WS_MAEMO_5))
    QGeoPositionInfoSourceMaemo *source = new QGeoPositionInfoSourceMaemo(parent);

    int status = source->init();
    if (status == -1) {
        delete source;
        return 0;
    }

    return source;
#else
    Q_UNUSED(parent);
    return 0;
#endif
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
    \property QGeoPositionInfoSource::minimumUpdateInterval
    \brief This property holds the minimum time (in milliseconds) required to retrieve a position update.

    This is the minimum value accepted by setUpdateInterval() and
    requestUpdate().
*/


/*!
    \fn virtual void QGeoPositionInfoSource::startUpdates() = 0;

    Starts emitting updates at regular intervals as specified by setUpdateInterval().

    If setUpdateInterval() has not been called, the source will emit updates
    as soon as they become available.

    An updateTimout() signal will be emitted if this QGeoPositionInfoSource subclass determines
    that it will not be able to provide regular updates.  This could happen if a satellite fix is
    lost or if a hardware error is detected.  Position updates will recommence if the data becomes
    available later on.  The updateTimout() signal will not be emitted again until after the
    periodic updates resume.
*/

/*!
    \fn virtual void QGeoPositionInfoSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QGeoPositionInfoSource::requestUpdate(int timeout = 0);

    Attempts to get the current position and emit positionUpdated() with
    this information. If the current position cannot be found within the given \a timeout
    (in milliseconds) or if \a timeout is less than the value returned by
    minimumUpdateInterval(), updateTimeout() is emitted.

    If the timeout is zero, the timeout defaults to a reasonable timeout
    period as appropriate for the source.

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
    \fn void QGeoPositionInfoSource::updateTimeout();

    If requestUpdate() was called, this signal will be emitted if the current position could not
    be retrieved within the specified timeout.

    If startUpdates() has been called, this signal will be emitted if this QGeoPositionInfoSource
    subclass determines that it will not be able to provide further regular updates.  This signal
    will not be emitted again until after the regular updates resume.
*/

#include "moc_qgeopositioninfosource.cpp"

QTM_END_NAMESPACE
