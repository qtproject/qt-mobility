/*
* ==============================================================================
*  Name        : CodingPresenter.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef PLACEPRESENTER_H
#define PLACEPRESENTER_H

#include <QTreeWidget>

#include "qgeocodingreply.h"
#include "geopresenter.h"

QTM_USE_NAMESPACE

class PlacePresenter : GeoPresenter
{
public:
    PlacePresenter(QTreeWidget* treeWidget, const QGeocodingReply* codingReply);
    virtual void show();

private:
    QTreeWidgetItem* showPlaces();
    void showPlace(QTreeWidgetItem* top, const QGeoLocation& place);
    void showAddress(QTreeWidgetItem* addrItem, const QAddress& address);

private:
    const QGeocodingReply* codingReply;
};

#endif
