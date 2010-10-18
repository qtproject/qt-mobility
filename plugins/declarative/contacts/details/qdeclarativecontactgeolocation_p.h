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


#ifndef QDECLARATIVECONTACTGEOLOCATION_H
#define QDECLARATIVECONTACTGEOLOCATION_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactgeolocation.h"

class QDeclarativeContactGeoLocation : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY fieldsChanged)
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY fieldsChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY fieldsChanged)
    Q_PROPERTY(double accuracy READ accuracy WRITE setAccuracy NOTIFY fieldsChanged)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude NOTIFY fieldsChanged)
    Q_PROPERTY(double altitudeAccuracy READ altitudeAccuracy WRITE setAltitudeAccuracy NOTIFY fieldsChanged)
    Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY fieldsChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY fieldsChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY fieldsChanged)
    Q_CLASSINFO("DefaultProperty", "label")
    Q_ENUMS(FieldType);
public:
    enum FieldType {
        Label = 0,
        Latitude,
        Longitude,
        Accuracy,
        Altitude,
        AltitudeAccuracy,
        Heading,
        Speed,
        Timestamp
    };

    QDeclarativeContactGeoLocation(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactGeoLocation());
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Geolocation;
    }

    void setLabel(const QString& label) {detail().setValue(QContactGeoLocation::FieldLabel, label);}
    QString label() const {return detail().value(QContactGeoLocation::FieldLabel);}
    void setLatitude(double latitude) {detail().setValue(QContactGeoLocation::FieldLatitude, latitude);}
    double latitude() const {return detail().variantValue(QContactGeoLocation::FieldLatitude).toDouble();}
    void setLongitude(double longitude) {detail().setValue(QContactGeoLocation::FieldLongitude, longitude);}
    double longitude() const {return detail().variantValue(QContactGeoLocation::FieldLongitude).toDouble();}
    void setAccuracy(double accuracy) {detail().setValue(QContactGeoLocation::FieldAccuracy, accuracy);}
    double accuracy() const {return detail().variantValue(QContactGeoLocation::FieldAccuracy).toDouble();}
    void setAltitude(double altitude) {detail().setValue(QContactGeoLocation::FieldAltitude, altitude);}
    double altitude() const {return detail().variantValue(QContactGeoLocation::FieldAltitude).toDouble();}
    void setAltitudeAccuracy(double altitudeAccuracy) {detail().setValue(QContactGeoLocation::FieldAltitudeAccuracy, altitudeAccuracy);}
    double altitudeAccuracy() const {return detail().variantValue(QContactGeoLocation::FieldAltitudeAccuracy).toDouble();}
    void setHeading(double heading) {detail().setValue(QContactGeoLocation::FieldHeading, heading);}
    double heading() const {return detail().variantValue(QContactGeoLocation::FieldHeading).toDouble();}
    void setSpeed(double speed) {detail().setValue(QContactGeoLocation::FieldSpeed, speed);}
    double speed() const {return detail().variantValue(QContactGeoLocation::FieldSpeed).toDouble();}
    void setTimestamp(const QDateTime& timestamp) {detail().setValue(QContactGeoLocation::FieldTimestamp, timestamp);}
    QDateTime timestamp() const {return detail().variantValue(QContactGeoLocation::FieldTimestamp).toDateTime();}
signals:
    void fieldsChanged();
};

#endif

