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
#include "qgeoaddress.h"

PlacePresenter::PlacePresenter(QTreeWidget *treeWidget, const QGeoPlacesReply* codingReply)
        : GeoPresenter(treeWidget), codingReply(codingReply)
{
}

void PlacePresenter::show()
{
    treeWidget->clear();
    QTreeWidgetItem* top = showPlaces();
    QList<QGeoPlace> places = codingReply->places();

    for (int i = 0; i < places.length(); i++) {
        showPlace(top, places[i]);
    }
}

QTreeWidgetItem* PlacePresenter::showPlaces()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(treeWidget);
    top->setText(0, "places");

    /*
    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "result");
    prop->setText(1, QString().setNum((quint16) codingReply->resultCode()));
    */

    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "count");
    prop->setText(1, QString().setNum(codingReply->places().size()));

    return top;
}

void PlacePresenter::showPlace(QTreeWidgetItem* top, const QGeoPlace& place)
{
    QTreeWidgetItem* prop;
    QTreeWidgetItem* placeItem = new QTreeWidgetItem(top);
    placeItem->setText(0, "place");

    if (!place.boundingBox().isValid())
        showBoundingBox(placeItem, place.boundingBox());

    prop = new QTreeWidgetItem(placeItem);
    prop->setText(0, "position");
    prop->setText(1, formatGeoCoordinate(place.coordinate()));

    if (!place.address().isEmpty()) {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText(0, "address");

        showAddress(prop, place.address());
    }

    /*
    if ( !place->alternatives().isEmpty() )
    {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText( 0, "alternatives" );

        showAddress(prop, place->alternatives());
    }
    */
}

void PlacePresenter::showAddress(QTreeWidgetItem* addrItem, const QGeoAddress& address)
{
    QTreeWidgetItem* prop;
    QString s = address.country();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "country");
        prop->setText(1, s);
    }

    s = address.countryCode();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "country code");
        prop->setText(1, s);
    }

    s = address.state();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "state");
        prop->setText(1, s);
    }

    s = address.county();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "county");
        prop->setText(1, s);
    }

    s = address.city();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "city");
        prop->setText(1, s);
    }

    s = address.district();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "district");
        prop->setText(1, s);
    }

    s = address.thoroughfareName();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "thoroughfare name");
        prop->setText(1, s);
    }

    s = address.thoroughfareNumber();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "thoroughfare number");
        prop->setText(1, s);
    }

    s = address.postCode();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "post code");
        prop->setText(1, s);
    }
}
