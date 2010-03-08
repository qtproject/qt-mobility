/*
* ==============================================================================
*  Name        : GeoPresenter.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef GEOPRESENTER_H
#define GEOPRESENTER_H

#include <QTreeWidget>
#include <QRectF>
#include <QString>

#include "QGeoCoordinateMaps.h"

QTM_USE_NAMESPACE

class GeoPresenter
{
protected:
    GeoPresenter(QTreeWidget* treeWidget) : treeWidget(treeWidget) {}

    QString formatGeoCoordinate(const QGeoCoordinateMaps& coord);
    void showBoundingBox(QTreeWidgetItem* routeItem, const QRectF& box);

public:
    virtual void show() = 0;

private:
    Q_DISABLE_COPY(GeoPresenter)

protected:
    QTreeWidget* treeWidget;
};

#endif
