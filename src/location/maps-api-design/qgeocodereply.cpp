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

#include "qgeocodereply.h"

#include "qgeolocation.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoCodeReply
    \brief The QGeoCodeReply class represents the result of a request for geocoding information.
    \ingroup maps


*/

/*!
  Constructs a geocoding reply with parent \a parent.
*/
QGeoCodeReply::QGeoCodeReply(QObject *parent) : QObject(parent)
{
}

/*!
  Destroys the geocoding reply object.
*/
QGeoCodeReply::~QGeoCodeReply()
{
}

// ordered from most to least specific
/*!
*/
void QGeoCodeReply::setLocation(const QList<QGeoLocation> &locations)
{
    Q_UNUSED(locations);
}

/*!
*/
QList<QGeoLocation> QGeoCodeReply::locations() const
{
    return QList<QGeoLocation>();
}

/*!
*/
void QGeoCodeReply::cancel()
{
}

/*!
  \fn void QGeoCodeReply::finished();
*/

/*!
  \fn void QGeoCodeReply::error(QGeoCodingService::ErrorCode errorCode, QString errorString)
*/

#include "moc_qgeocodereply.cpp"

QTM_END_NAMESPACE
