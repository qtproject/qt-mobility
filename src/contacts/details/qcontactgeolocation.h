/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTGEOLOCATION_H
#define QCONTACTGEOLOCATION_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
// replaces the below
class Q_CONTACTS_EXPORT QContactGeoLocation : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldLabel;
    static const QLatin1Constant FieldLatitude;
    static const QLatin1Constant FieldLongitude;
    static const QLatin1Constant FieldAccuracy;
    static const QLatin1Constant FieldAltitude;
    static const QLatin1Constant FieldAltitudeAccuracy;
    static const QLatin1Constant FieldHeading;
    static const QLatin1Constant FieldSpeed;
    static const QLatin1Constant FieldTimestamp;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactGeoLocation, "GeoLocation")
    Q_DECLARE_LATIN1_CONSTANT(FieldLabel, "Label");
    Q_DECLARE_LATIN1_CONSTANT(FieldLatitude, "Latitude");
    Q_DECLARE_LATIN1_CONSTANT(FieldLongitude, "Longitude");
    Q_DECLARE_LATIN1_CONSTANT(FieldAccuracy, "Accuracy");
    Q_DECLARE_LATIN1_CONSTANT(FieldAltitude, "Altitude");
    Q_DECLARE_LATIN1_CONSTANT(FieldAltitudeAccuracy, "AltitudeAccuracy");
    Q_DECLARE_LATIN1_CONSTANT(FieldHeading, "Heading");
    Q_DECLARE_LATIN1_CONSTANT(FieldSpeed, "Speed");
    Q_DECLARE_LATIN1_CONSTANT(FieldTimestamp, "Timestamp");
#endif

    void setLabel(const QString& label) {setValue(FieldLabel, label);}
    QString label() const {return value(FieldLabel);}
    void setLatitude(double latitude) {setValue(FieldLatitude, latitude);}
    double latitude() const {return variantValue(FieldLatitude).toDouble();}
    void setLongitude(double longitude) {setValue(FieldLongitude, longitude);}
    double longitude() const {return variantValue(FieldLongitude).toDouble();}
    void setAccuracy(double accuracy) {setValue(FieldAccuracy, accuracy);}
    double accuracy() const {return variantValue(FieldAccuracy).toDouble();}
    void setAltitude(double altitude) {setValue(FieldAltitude, altitude);}
    double altitude() const {return variantValue(FieldAltitude).toDouble();}
    void setAltitudeAccuracy(double altitudeAccuracy) {setValue(FieldAltitudeAccuracy, altitudeAccuracy);}
    double altitudeAccuracy() const {return variantValue(FieldAltitudeAccuracy).toDouble();}
    void setHeading(double heading) {setValue(FieldHeading, heading);}
    double heading() const {return variantValue(FieldHeading).toDouble();}
    void setSpeed(double speed) {setValue(FieldSpeed, speed);}
    double speed() const {return variantValue(FieldSpeed).toDouble();}
    void setTimestamp(const QDateTime& timestamp) {setValue(FieldTimestamp, timestamp);}
    QDateTime timestamp() const {return variantValue(FieldTimestamp).toDateTime();}
};

QTM_END_NAMESPACE

#endif

