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

#include "qgeocodingreply.h"
#include "qgeocodingreply_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeocodingReply
    \brief The QGeocodingReply class represents a response to a QReverseGeocodingRequest.
    \ingroup location
    
    This class represents a geododing reply in response to a
    previous (reverse) geocoding request.

    \sa QGeocodingRequest
    \sa QReverseGeocodingRequest
*/

/*!
    \enum QGeocodingReply::ResultCode
    
    Possible result codes as reported by the geo engine.

    \value OK Request succeeded.
    \value Failed RequestFailed.
*/

QGeocodingReply::QGeocodingReply() 
    : d_ptr(new QGeocodingReplyPrivate())
{
}

/*!
    Returns the result code as reported by the geo engine.
*/
QGeocodingReply::ResultCode QGeocodingReply::resultCode() const
{
    Q_D(const QGeocodingReply);
    return d->code;
}

/*!
    Sets the result code.
*/
void QGeocodingReply::setResultCode(QGeocodingReply::ResultCode result)
{
    Q_D(QGeocodingReply);
    d->code = result;
}

/*!
    Returns a textual description of the result.
*/
QString QGeocodingReply::resultDescription() const
{
    Q_D(const QGeocodingReply);
    return d->descr;
}

/*!
    Sets a textual description of the result.
*/
void QGeocodingReply::setResultDescription(QString resultDescription)
{
    Q_D(QGeocodingReply);
    d->descr = resultDescription;    
}
/*!
    Returns the number of places found.
*/
quint32 QGeocodingReply::count() const
{
    Q_D(const QGeocodingReply);
    return d->plcs.length();
}
/*!
    Returns a list of all places found.
*/
QList<QGeoLocation> QGeocodingReply::places() const
{
    Q_D(const QGeocodingReply);
    return d->plcs;
}
void QGeocodingReply::addPlace(QGeoLocation place)
{
    Q_D(QGeocodingReply);
    d->plcs.append(place);
}
    
#include "moc_qgeocodingreply.cpp"

QTM_END_NAMESPACE
