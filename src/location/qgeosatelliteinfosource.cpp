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
#include <qgeosatelliteinfosource.h>

#if defined(Q_OS_SYMBIAN)
#   include "qgeosatelliteinfosource_s60_p.h"
#elif defined(Q_OS_WINCE)
#   include "qgeosatelliteinfosource_wince_p.h"
#elif defined(Q_WS_MAEMO_6)
#   include "qgeosatelliteinfosource_maemo_p.h"
#elif defined(Q_WS_MAEMO_5)
#   include "qgeosatelliteinfosource_maemo5_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoSatelliteInfoSource
    \brief The QGeoSatelliteInfoSource class is an abstract base class for the distribution of satellite information updates.
    \ingroup location

    The static function QGeoSatelliteInfoSource::createDefaultSource() creates a default
    satellite data source that is appropriate for the platform, if one is
    available. Otherwise, QGeoSatelliteInfoSource can be subclassed to create an
    appropriate custom source of satellite data.

    Call startUpdates() and stopUpdates() to start and stop regular updates,
    or requestUpdate() to request a single update.
    When an update is available, satellitesInViewUpdated() and/or
    satellitesInUseUpdated() will be emitted.

    \warning On Windows CE it is not possible to detect if a device is GPS enabled.
    The default satellite source on a Windows CE device without GPS support will never provide any satellite data.
*/

/*!
    Creates a source with the specified \a parent.
*/
QGeoSatelliteInfoSource::QGeoSatelliteInfoSource(QObject *parent)
        : QObject(parent)
{
}

/*!
    Creates and returns a source with the specified \a parent that reads
    from the system's default source of satellite update information.

    Returns 0 if the system has no default source.
*/
QGeoSatelliteInfoSource *QGeoSatelliteInfoSource::createDefaultSource(QObject *parent)
{
#if defined(Q_OS_SYMBIAN)
    CQGeoSatelliteInfoSourceS60 *ret = NULL;
    TRAPD(error, ret = CQGeoSatelliteInfoSourceS60::NewL(parent));
    if (error != KErrNone)
        return 0;
    return ret;
#elif defined(Q_OS_WINCE)
    return new QGeoSatelliteInfoSourceWinCE(parent);
#elif (defined(Q_WS_MAEMO_6)) || (defined(Q_WS_MAEMO_5))
    QGeoSatelliteInfoSourceMaemo *source = new QGeoSatelliteInfoSourceMaemo(parent);
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
    \fn void QGeoSatelliteInfoSource::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the satellites that are
    currently in view.

    The \a satellites parameter holds the satellites currently in view.
*/

/*!
    \fn void QGeoSatelliteInfoSource::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the number of satellites that are
    currently in use.

    These are the satellites that are used to get a "fix" - that
    is, those used to determine the current position.

    The \a satellites parameter holds the satellites currently in use.
*/

/*!
    \fn virtual void QGeoSatelliteInfoSource::startUpdates() = 0;

    Starts emitting updates at regular intervals. The updates will be
    provided whenever new satellite information becomes available.

    \sa satellitesInViewUpdated(), satellitesInUseUpdated()
*/

/*!
    \fn virtual void QGeoSatelliteInfoSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QGeoSatelliteInfoSource::requestUpdate(int timeout = 0);

    Attempts to get the current satellite information and emit
    satellitesInViewUpdated() and satellitesInUseUpdated() with this
    information. If the current position cannot be found
    within the given \a timeout (in milliseconds), requestTimeout() is
    emitted.

    If the timeout is zero, the timeout defaults to a reasonable timeout
    period as appropriate for the source.

    This does nothing if another update request is in progress. However
    it can be called even if startUpdates() has already been called and
    regular updates are in progress.
*/

/*!
    \fn void QGeoSatelliteInfoSource::requestTimeout();

    Emitted if requestUpdate() was called and the current satellite
    information could not be retrieved within the specified timeout.
*/

#include "moc_qgeosatelliteinfosource.cpp"

QTM_END_NAMESPACE
