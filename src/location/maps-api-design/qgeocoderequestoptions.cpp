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

#include "qgeocoderequestoptions.h"
#include "qgeocoderequestoptions_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoCodeRequestOptions
    \brief The QGeoCodeRequestOptions class represents various options which
    may be specified when requesting geocoding information with an instance of
    QGeoCodingService.
    \ingroup maps

    The most common uses of the methods in QGeoCodingService will use a single
    QGeoCoordinate, QGeoAddress or QString as their argument.  A GeoBoundingBox
    can also be supplied to bias the results to a particular area.

    The QGeoCodeRequestOptions represents the options corresponding to the
    more advanced (or less common) options which can be passed to the
    QGeoCodingService methods.

    \sa QGeoCodingService
*/

/*!
    Constructs a QGeoCodingRequestOptions object.
*/
QGeoCodeRequestOptions::QGeoCodeRequestOptions()
    : d_ptr(new QGeoCodeRequestOptionsPrivate()) {}

/*!
    Constructs a QGeoCodingRequestOptions object from \a other.
*/
QGeoCodeRequestOptions::QGeoCodeRequestOptions(const QGeoCodeRequestOptions &other)
    : d_ptr(new QGeoCodeRequestOptionsPrivate(*(other.d_ptr))) {}

/*!
    Destroys this QGeoCodingRequestOptions object.
*/
QGeoCodeRequestOptions::~QGeoCodeRequestOptions()
{
    Q_D(QGeoCodeRequestOptions);
    delete d;
}

/*!
    Assigns the value of \a other to this QGeoCodingRequestOptions object and
    returns a reference to this QGeoCodingRequestOptions object.
*/
QGeoCodeRequestOptions& QGeoCodeRequestOptions::operator= (const QGeoCodeRequestOptions &other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
    Sets the maximum number of responses to request to \a maximumResponses.
*/
void QGeoCodeRequestOptions::setMaximumResponses(int maximumResponses)
{
    Q_D(QGeoCodeRequestOptions);
    d->maximumResponses = maximumResponses;
}

/*!
    Returns the maximum number of responses which will be requested.
*/
int QGeoCodeRequestOptions::maximumResponses() const
{
    Q_D(const QGeoCodeRequestOptions);
    return d->maximumResponses;
}

/*******************************************************************************
*******************************************************************************/

QGeoCodeRequestOptionsPrivate::QGeoCodeRequestOptionsPrivate() {
    maximumResponses = 1;
}

QGeoCodeRequestOptionsPrivate::QGeoCodeRequestOptionsPrivate(const QGeoCodeRequestOptionsPrivate &other)
    : maximumResponses(other.maximumResponses) {}

QGeoCodeRequestOptionsPrivate::~QGeoCodeRequestOptionsPrivate() {}

QGeoCodeRequestOptionsPrivate& QGeoCodeRequestOptionsPrivate::operator= (const QGeoCodeRequestOptionsPrivate &other)
{
    maximumResponses = other.maximumResponses;
    return *this;
}

QTM_END_NAMESPACE

