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

class Q_LOCATION_EXPORT QGeoCodingService : public QObject {

    Q_OBJECT

public:
    enum ServiceProvider {
        NokiaServices
    };

    // Option - add a serviceoptions object, possible sublcassed per service provider
    // ie createService(new QGeoCodingServiceOptionsNokia(... nokia service specific options...)
    // means one public class per service / provider combination
    //    already going to have 2 private classes per service / provider combination
    //    good opening for service specific documentation
    //    means more API to maintain
    //    easier to document than string maps
    //    can move the supported feature information to these classes
    //        auth information is in here, so can be more nuanced in the reporting

    static QGeoCodingService* createService(QGeoCodingService::ServiceProvider provider,
                                            QString versionString,
                                            QString token = QString());

    // Option - add a servicerequest object, possible sublcassed per service provider

    enum ErrorCode {
        NoError
    };

    enum SupportLevel {
        NoSupport,
        NoSupportWithCurrentToken,
        SupportedWithCurrentToken,
        Supported
    };

    enum GeocodingFeature {
        GeocodeFromAddress,
        GeocodeFromString,
        ReverseGeocodeFromCoordinate
    };

    virtual SupportLevel featureSupported(QGeoCodingService::GeocodingFeature feature) = 0;

    QGeoCodingService(QObject *parent = 0);
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

signals:
    void replyFinished(QGeoCodeReply *reply);
    void replyError(QGeoCodeReply *reply,
                    QGeoCodingService::ErrorCode errorCode,
                    QString errorString);

private:
    Q_DISABLE_COPY(QGeoCodingService)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
