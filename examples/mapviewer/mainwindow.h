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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QSlider>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include "qmapview.h"
#include "qgeoroutereply.h"
#include "qgeoroutingservice.h"
#include "qmaptileservice.h"

namespace Ui
{
class MainWindow;
}
#ifdef Q_OS_SYMBIAN
QTM_BEGIN_NAMESPACE
class QNetworkSession;
QTM_END_NAMESPACE
#endif

QTM_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    virtual void resizeEvent(QResizeEvent* event);

private:
    void createMenus();

private slots:
    void mapClicked(QGeoCoordinate geoCoord, QGraphicsSceneMouseEvent* mouseEvent);
    void mapObjectSelected(QMapObject* mapObject);
    void routeReplyFinished(QGeoRouteReply* reply);
    void setRtFromTo(bool checked);
    void zoomLevelChanged(quint16 oldZoomLevel, quint16 newZoomLevel);
    void setScheme(bool checked);
    void addMarker(bool checked);
    void addIconMarker(bool checked);
    void drawLine(bool checked);
    void drawRect(bool checked);
    void drawEllipse(bool checked);
    void drawPolygon(bool checked);
    void drawPixmap(bool checked);
    void customContextMenuRequest(const QPoint&);
    void delayedInit();

private:
    Ui::MainWindow *ui;

private:
    QGraphicsView* qgv;
    QMapTileService *mapService;
    QGeoRoutingService *routingService;
    QMapView* mapView;
    QSlider* slider;
    QMenu* popupMenu;
    QAction* mnDay;
    QAction* mnSat;
    QAction* mnTer;

    QGeoCoordinate lastClicked;

    QList<const QMapMarker*> selectedMarkers;
#ifdef Q_OS_SYMBIAN
    QNetworkSession *session;
#endif

};

#endif // MAINWINDOW_H
