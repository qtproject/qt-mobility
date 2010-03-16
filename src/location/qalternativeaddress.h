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

#ifndef QLOCATION_ALTERNATIVEADDRESS_H
#define QLOCATION_ALTERNATIVEADDRESS_H

#include "qmobilityglobal.h"

#include <QString>
#include <QList>

QTM_BEGIN_NAMESPACE

class QAlternativeAddress
{
    friend class QGeocodingXmlParser;

public:
    struct AltValue {
        QString value; //!< The string value of a given alternative.
        QString language; //!< The MARC language code
    };

public:
    QAlternativeAddress();

    QList<AltValue> country() const;
    QList<AltValue> countryCode() const;
    QList<AltValue> state() const;
    QList<AltValue> county() const;
    QList<AltValue> city() const;
    QList<AltValue> district() const;
    QList<AltValue> thoroughfareName() const;
    QList<AltValue> thoroughfareNumber() const;
    QList<AltValue> postCode() const;

private:
    QList<AltValue> lCountry;
    QList<AltValue> lCountryCode;
    QList<AltValue> lState;
    QList<AltValue> lCounty;
    QList<AltValue> lCity;
    QList<AltValue> lDistrict;
    QList<AltValue> lThoroughfareName;
    QList<AltValue> lThoroughfareNumber;
    QList<AltValue> lPostCode;
};

QTM_END_NAMESPACE

#endif
