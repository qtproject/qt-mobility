/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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

    s = address.postcode();

    if (!s.isEmpty()) {
        QTreeWidgetItem* postCodeItem = new QTreeWidgetItem(addrItem);
        postCodeItem->setText(0, "post code");
        postCodeItem->setText(1, s);
    }
}
