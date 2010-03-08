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

#ifndef QLOCATION_GEOCODINGREPLY_H
#define QLOCATION_GEOCODINGREPLY_H

#include <QString>
#include <QObject>

#include "qgeolocation.h"
#include "qaddress.h"
#include "qgeoreply.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represents a geododing reply in response to a
* previous (reverse) geocoding request.
* @see QGeocodingRequest
* @see QReverseGeocodingRequest
*/
class QGeocodingReply : public QGeoReply
{
    Q_OBJECT

    friend class QGeoNetworkManager;
    friend class QGeocodingXmlHandler;

public:

    //! Possible result codes as reported by the geo engine.
    enum ResultCode {
        OK = 0, //!< request succeeded
        Failed //!< request failed
    };

public:
    /*!
    * @return The result code as reported by the geo engine.
    */
    ResultCode resultCode() const {
        return code;
    }
    /*!
    * @return A textual description of the result.
    */
    QString resultDescription() const {
        return descr;
    }
    /*!
    * @return The number of places found.
    */
    quint32 count() const {
        return plcs.length();
    }
    /*!
    * @return A list of all places found.
    */
    QList<QGeoLocation> places() const {
        return plcs;
    }

private:
    Q_DISABLE_COPY(QGeocodingReply)

    QGeocodingReply() {}

private:
    ResultCode code;
    QString descr;
    QList<QGeoLocation> plcs;
};


QTM_END_NAMESPACE

#endif
