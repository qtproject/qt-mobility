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

#include "placepresenter.h"
#include <qgeoaddress.h>

PlacePresenter::PlacePresenter(QTreeWidget *treeWidget, const QGeoSearchReply* codingReply)
        : GeoPresenter(treeWidget), m_codingReply(codingReply)
{
}

void PlacePresenter::show()
{
    m_treeWidget->clear();
    QTreeWidgetItem* placesItem = new QTreeWidgetItem(m_treeWidget);
    placesItem->setText(0, "places");

    QTreeWidgetItem* countItem = new QTreeWidgetItem(placesItem);
    countItem->setText(0, "count");
    countItem->setText(1, QString().setNum(m_codingReply->places().size()));

    QList<QGeoPlace> places = m_codingReply->places();
    for (int i = 0; i < places.length(); i++) {
        showPlace(placesItem, places[i]);
    }
}

void PlacePresenter::showPlace(QTreeWidgetItem* top, const QGeoPlace& place)
{
    QTreeWidgetItem* placeItem = new QTreeWidgetItem(top);
    placeItem->setText(0, "place");

    if (place.viewport().isValid())
        showBoundingBox(placeItem, place.viewport());

    QTreeWidgetItem* positionItem = new QTreeWidgetItem(placeItem);
    positionItem->setText(0, "position");
    positionItem->setText(1, formatGeoCoordinate(place.coordinate()));

    if (!place.address().isEmpty()) {
        QTreeWidgetItem* addressItem = new QTreeWidgetItem(placeItem);
        addressItem->setText(0, "address");

        showAddress(addressItem, place.address());
    }
}

void PlacePresenter::showAddress(QTreeWidgetItem* addrItem, const QGeoAddress& address)
{
    QString s = address.country();

    if (!s.isEmpty()) {
        QTreeWidgetItem* countryItem = new QTreeWidgetItem(addrItem);
        countryItem->setText(0, "country");
        countryItem->setText(1, s);
    }

    s = address.countryCode();

    if (!s.isEmpty()) {
        QTreeWidgetItem* coutryCodeItem = new QTreeWidgetItem(addrItem);
        coutryCodeItem->setText(0, "country code");
        coutryCodeItem->setText(1, s);
    }

    s = address.state();

    if (!s.isEmpty()) {
        QTreeWidgetItem* stateItem = new QTreeWidgetItem(addrItem);
        stateItem->setText(0, "state");
        stateItem->setText(1, s);
    }

    s = address.county();

    if (!s.isEmpty()) {
        QTreeWidgetItem* countyItem = new QTreeWidgetItem(addrItem);
        countyItem->setText(0, "county");
        countyItem->setText(1, s);
    }

    s = address.city();

    if (!s.isEmpty()) {
        QTreeWidgetItem* cityItem = new QTreeWidgetItem(addrItem);
        cityItem->setText(0, "city");
        cityItem->setText(1, s);
    }

    s = address.district();

    if (!s.isEmpty()) {
        QTreeWidgetItem* districtItem = new QTreeWidgetItem(addrItem);
        districtItem->setText(0, "district");
        districtItem->setText(1, s);
    }

    s = address.street();

    if (!s.isEmpty()) {
        QTreeWidgetItem* streetNameItem = new QTreeWidgetItem(addrItem);
        streetNameItem->setText(0, "street name");
        streetNameItem->setText(1, s);
    }

    /* TODO: check on windows
        s = address.streetNumber();

        if (!s.isEmpty()) {
            QTreeWidgetItem* streetNumberItem = new QTreeWidgetItem(addrItem);
            streetNumberItem->setText(0, "street number");
            streetNumberItem->setText(1, s);
        }
    */

    s = address.postCode();

    if (!s.isEmpty()) {
        QTreeWidgetItem* postCodeItem = new QTreeWidgetItem(addrItem);
        postCodeItem->setText(0, "post code");
        postCodeItem->setText(1, s);
    }
}
