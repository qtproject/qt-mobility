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

#include "qlocationfilter.h"
#include "qlocationfilter_p.h"

QTM_BEGIN_NAMESPACE

QLocationFilter::QLocationFilter()
    : d_ptr(new QLocationFilterPrivate)
{
}

QLocationFilter::~QLocationFilter()
{
}

QString QLocationFilter::LROId() const
{
    Q_D(const QLocationFilter);
    return d->m_LROId;
}

void QLocationFilter::setLROId(const QString& id)
{
    Q_D(QLocationFilter);
    d->m_LROId = id;
}

QString QLocationFilter::country() const
{
    Q_D(const QLocationFilter);
    return d->m_country;
}

void QLocationFilter::setCountry(const QString& country)
{
    Q_D(QLocationFilter);
    d->m_country=country;

}

QString QLocationFilter::county() const
{
    Q_D(const QLocationFilter);
    return d->m_county;

}

void QLocationFilter::setCounty(const QString& county)
{
    Q_D(QLocationFilter);
    d->m_county = county;
}

QString QLocationFilter::state() const
{
    Q_D(const QLocationFilter);
    return d->m_state;
}

void QLocationFilter::setState(const QString& state)
{
    Q_D(QLocationFilter);
    d->m_state = state;
}

QString QLocationFilter::city() const
{
    Q_D(const QLocationFilter);
    return d->m_city;
}

void QLocationFilter::setCity(const QString& city)
{
    Q_D(QLocationFilter);
    d->m_city = city;
}

QString QLocationFilter::postalCode() const
{
    Q_D(const QLocationFilter);
    return d->m_postalCode;
}

void QLocationFilter::setPostalCode(const QString& postalCode)
{
    Q_D(QLocationFilter);
    d->m_postalCode = postalCode;
}

QString QLocationFilter::district() const
{
    Q_D(const QLocationFilter);
    return d->m_district;
}

void QLocationFilter::setDistrict(const QString& district)
{
    Q_D(QLocationFilter);
    d->m_district = district;
}

QString QLocationFilter::street() const
{
    Q_D(const QLocationFilter);
    return d->m_street;
}

void QLocationFilter::setStreet(const QString& street)
{
    Q_D(QLocationFilter);
    d->m_street = street;
}

QString QLocationFilter::houseNumber() const
{
    Q_D(const QLocationFilter);
    return d->m_houseNumber;
}

void QLocationFilter::setHouseNumber(const QString& number)
{
    Q_D(QLocationFilter);
    d->m_houseNumber = number;
}
QGeoCoordinate QLocationFilter::coordinate() const
{
    Q_D(const QLocationFilter);
    return d->m_coord;
}

void QLocationFilter::setCoordinate(const QGeoCoordinate& coord)
{
    Q_D(QLocationFilter);
    d->m_coord = coord;
}

QTM_END_NAMESPACE
