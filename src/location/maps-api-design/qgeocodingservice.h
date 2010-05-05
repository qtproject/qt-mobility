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

#ifndef QGEOCODINGSERVICE_H
#define QGEOCODINGSERVICE_H

#include "qgeocoordinate.h"
#include "qgeoaddress.h"

#include "qgeoboundingbox.h"
#include "qgeocoderequestoptions.h"

#include <QObject>
#include <QString>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCodeReply;

class QGeoCodingServicePrivate;

class Q_LOCATION_EXPORT QGeoCodingService : public QObject {

    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

    enum SupportLevel {
        NoSupport,
        NoSupportWithCurrentAuthorization,
        SupportedWithCurrentAuthorization,
        Supported
    };

    enum RequestOption {
        ViewportBiasing,
        MaximumResults
    };

    virtual ~QGeoCodingService();

    virtual QGeoCodeReply* reverseGeocode(const QGeoCoordinate &coord,
                                    const QGeoBoundingBox &bounds = QGeoBoundingBox(),
                                    const QGeoCodeRequestOptions &options = QGeoCodeRequestOptions()) = 0;
    virtual QGeoCodeReply* geocode(const QGeoAddress &address,
                                    const QGeoBoundingBox &bounds = QGeoBoundingBox(),
                                    const QGeoCodeRequestOptions &options = QGeoCodeRequestOptions()) = 0;
    virtual QGeoCodeReply* geocode(const QString &locationString,
                                    const QGeoBoundingBox &bounds = QGeoBoundingBox(),
                                    const QGeoCodeRequestOptions &options = QGeoCodeRequestOptions()) = 0;

    SupportLevel supportedRequestOption(RequestOption option) const;

signals:
    void replyFinished(QGeoCodeReply *reply);
    void replyError(QGeoCodeReply *reply,
                    QGeoCodingService::ErrorCode errorCode,
                    QString errorString);

protected:
    QGeoCodingService();

    void setSupportedRequestOption(RequestOption option, SupportLevel level);

    QGeoCodingServicePrivate* d_ptr;

private:
    Q_DISABLE_COPY(QGeoCodingService)
    Q_DECLARE_PRIVATE(QGeoCodingService)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
