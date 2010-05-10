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

#include "qgeocodingservice.h"
#include "qgeocodingservice_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoCodingService
    \brief The QGeoCodingService class manages requests to and replies from a
    geocoding service.
    \ingroup maps

    The request functions return a QGeoCodeReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoCodeReply objects and the QGeoCodingService instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoCodeReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

     \sa QGeoCodeReply
*/

/*!
    \enum QGeoCodingService::ErrorCode

    Describes the type of error which occurred when a QGeoCodingService instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    \enum QGeoCodingService::SupportLevel

    Desribes the level of support for a feature of QGeoCodingService.

    The feature may be unsupport

    \value NoSupport
        The feature is not supported at all.
    \value NoSupportWithCurrentAuthorization
        The feature is not supported due to the current authorization.  This
        also implies that the feature is supported but requries authorization
        to access.
    \value SupportedWithCurrentAuthorization
        The feature is supported due to the current authorization.  This also
        implies that the feature is supported but requries authorization to
        access.
    \value Supported
        The feature is supported.  This also implies that the feature does not
        requrie authorization to access.

    \sa QGeoCodingService::RequestOption
    \sa QGeoCodingService::supportedRequestOption()
*/

/*!
    \enum QGeoCodingService::RequestOption

    Describes options which can be specified in a request to a QGeoCodingService
    instance via a QGeoCodeRequestionOptions object.

    Support for particular options can be determined using
    QGeocodingService::supportedRequesOption().

    \value ViewportBiasing
        The results can be limited to those which occur within a particular
        QGeoBoundingBox.
    \value MaximumResults
        The maximum number of results returned can be set in the request.

    \sa QGeoCodingService::SupportLevel
    \sa QGeoCodingService::supportedRequestOption()
*/

/*!
    Constructs a QGeoCodingService object.
*/
QGeoCodingService::QGeoCodingService()
        : d_ptr(new QGeoCodingServicePrivate()) {}

/*!
    Destroys this QGeoCodingService object.
*/
QGeoCodingService::~QGeoCodingService()
{
    Q_D(QGeoCodingService);
    delete d;
}

/*!
\fn QGeoCodeReply* QGeoCodingService::reverseGeocode(const QGeoCoordinate &coord,
                                                     const QGeoBoundingBox &bounds,
                                                     const QGeoCodeRequestOptions &options)

Request a conversion of the coordinate \a coord to an address or list of
addresses.  The eventual result will be a list of addresses or an error.

The return value is a QGeoCodeReply object, which manages the result of the
request.  If the request completes successfully then the QGeoCodeReply object
will emit the QGeoCodeReply::finished() signal and this QGeoCodingService
instance will emit the QGeoCodingService::finished() signal.

The results can then be retreived with QGeoCodeReply::locations(), which will
contain the address or addresses and will be ordered from the least specific
address information to the most specific address information.

If an error occurs then the QGeoCodeReply object will emit QGeoCodeReply::error()
and this geocoding service object will emit QGeoCodingService::error().

The results may be limited to those within a particular geographical bounds by
setting \a bounds to a valid QGeoBoundingBox.  To determine if the service
supports the biasing of the results, use
QGeoCodingService::supportedRequestOption(QGeoCodingService::ViewportBiasing).
If the service does not support viewport biasing, \a bounds will be ignored.

Additional options can also be set using \a options, and support for the
various options can also be determined using
QGeoCodingService::supportedRequestOption().

\sa QGeoCodeRequestOptions
*/

/*!
\fn QGeoCodeReply* QGeoCodingService::geocode(const QGeoAddress &address,
                                              const QGeoBoundingBox &bounds,
                                              const QGeoCodeRequestOptions &options)

Request the conversion of the address \a address to a coordinate or list of
coordinates.  The eventual result will be a list of coordinates or an error.

The return value is a QGeoCodeReply object, which manages the result of the
request.  If the request completes successfully then the QGeoCodeReply object
will emit the QGeoCodeReply::finished() signal and this geocoding service will
emit the QGeoCodingService::finished() signal.

The results can then be retreived with QGeoCodeReply::locations(), which will
contain the coordinate or coordinates.

If an error occurs then the QGeoCodeReply object will emit QGeoCodeReply::error()
and this geocoding service object will emit QGeoCodingService::error().

The results may be limited to those within a particular geographical bounds by
setting \a bounds to a valid QGeoBoundingBox.  To determine if the service
supports the biasing of the results, use
QGeoCodingService::supportedRequestOption(QGeoCodingService::ViewportBiasing).
If the service does not support viewport biasing, \a bounds will be ignored.

Additional options can also be set using \a options, and support for the
various options can also be determined using
QGeoCodingService::supportedRequestOption().

\sa QGeoCodeRequestOptions
*/

/*!
\fn QGeoCodeReply* QGeoCodingService::geocode(const QString &locationString,
                                              const QGeoBoundingBox &bounds,
                                              const QGeoCodeRequestOptions &options)

Request the conversion of the arbitrary location string \a locationString to
a coordinate or list of coordinates.  The eventual result will be a list of
coordinates or an error.

The value of \a locationString is expected to be partial address information,
although the provider of the geocoding service may return results for more
general inputs.

The return value is a QGeoCodeReply object, which manages the result of the
request.  If the request completes successfully then the QGeoCodeReply object
will emit the QGeoCodeReply::finished() signal and this geocoding service will
emit the QGeoCodingService::finished() signal.

The results can then be retreived with QGeoCodeReply::locations(), which will
contain the coordinate or coordinates.

If an error occurs then the QGeoCodeReply object will emit QGeoCodeReply::error()
and this geocoding service object will emit QGeoCodingService::error().

The results may be limited to those within a particular geographical bounds by
setting \a bounds to a valid QGeoBoundingBox.  To determine if the service
supports the biasing of the results, use
QGeoCodingService::supportedRequestOption(QGeoCodingService::ViewportBiasing).
If the service does not support viewport biasing, \a bounds will be ignored.

Additional options can also be set using \a options, and support for the
various options can also be determined using
QGeoCodingService::supportedRequestOption().

\sa QGeoCodeRequestOptions
*/

/*!
    Returns the level of support for the request option \a option.

    The level of support will be determined by the subclasses of
    QGeoCodingService and the current authorization against the service.

    \sa QGeoCodingService::RequestOption
    \sa QGeoCodingService::SupportLevel
*/
QGeoCodingService::SupportLevel QGeoCodingService::supportedRequestOption(QGeoCodingService::RequestOption option) const
{
    Q_D(const QGeoCodingService);
    return d->supportedRequestOption[option];
}

/*!
    Sets the level of support for the request option \a option to \a level.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedRequestOption() provides accurate information.

    \sa QGeoCodingService::RequestOption
    \sa QGeoCodingService::SupportLevel
*/
void QGeoCodingService::setSupportedRequestOption(QGeoCodingService::RequestOption option,
        QGeoCodingService::SupportLevel level)
{
    Q_D(QGeoCodingService);
    d->supportedRequestOption.insert(option, level);
}

/*!
    \fn void QGeoCodingService::replyFinished(QGeoCodeReply *reply)

    Indicates that a request handled by this QGeoCodingService object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this QGeoCodingService
    instance when the request was issued, and that the QGeoCodeReply::finished()
    signal can be used instead of this signal if it is more convinient to do so.

    Do not delete the QGeoCodeReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoCodeReply::finished()
*/

/*!
    \fn void QGeoCodingService::replyError(QGeoCodeReply *reply,
                                           QGeoCodingService::ErrorCode errorCode,
                                           QString errorString)

    Indicates that a request handled by this QGeoCodingService object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoCodeReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoCodingService instance when the request was issued, and that the
    QGeoCodeReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoCodeReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoCodeReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoCodingServicePrivate::QGeoCodingServicePrivate() {}

QGeoCodingServicePrivate::QGeoCodingServicePrivate(const QGeoCodingServicePrivate &other)
        : supportedRequestOption(other.supportedRequestOption) {}

QGeoCodingServicePrivate::~QGeoCodingServicePrivate() {}

QGeoCodingServicePrivate& QGeoCodingServicePrivate::operator= (const QGeoCodingServicePrivate & other)
{
    supportedRequestOption = other.supportedRequestOption;

    return *this;
}

#include "moc_qgeocodingservice.cpp"

QTM_END_NAMESPACE
