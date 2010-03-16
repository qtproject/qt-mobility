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

#ifndef QLOCATION_ADDRESS_H
#define QLOCATION_ADDRESS_H

#include "qmobilityglobal.h"

#include <QString>

QTM_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QAddress
{
    friend class QGeocodingXmlHandler;
    friend class QGeocodingXmlParser;

public:
    QAddress();

    QString country() const;
    QString countryCode() const;
    QString state() const;
    QString county() const;
    QString city() const;
    QString district() const;
    QString thoroughfareName() const;
    QString thoroughfareNumber() const;
    QString postCode() const;

    bool isEmpty() const;

private:
    QString sCountry; //!< country field
    QString sCountryCode; //!< country code field
    QString sState; //!< state field
    QString sCounty; //!< county field
    QString sCity; //!< city field
    QString sDistrict; //!< district field
    QString sThoroughfareName; //!< thoroughfare name field
    QString sThoroughfareNumber; //!< thoroughfare number field
    QString sPostCode; //!< post code field
};

QTM_END_NAMESPACE

#endif
