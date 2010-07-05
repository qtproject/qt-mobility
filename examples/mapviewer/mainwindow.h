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

#include <qgeomappingmanager.h>
#include <qgeoroutingmanager.h>
#include <qgeomapwidget.h>
#include <qgeoserviceprovider.h>
#include <qgeomapmarkerobject.h>
#include <QMainWindow>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPixmap>
#include <QList>

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

class MapWidget : public QGeoMapWidget
{
    Q_OBJECT
public:
    MapWidget(QGeoMappingManager *manager);
    ~MapWidget();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

private:
    bool panActive;
};

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
    Ui::MainWindow *ui;

    void setProvider(QString providerId);
    void createMenus();
    void createMarkerIcon();
    void removeMarkers();

private slots:
    void delayedInit();
    void drawRect(bool checked);
    void drawMarker(bool checked);
    void drawPolyline(bool checked);
    void drawPolygon(bool checked);
    void calcRoute(bool checked);
    void customContextMenuRequest(const QPoint&);
    void routeFinished();

private:
    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    QGeoRoutingManager *m_routingManager;
    MapWidget *m_mapWidget;
    QMenu* m_popupMenu;
    QPixmap m_markerIcon;
    QPoint lastClicked;
    QList<QPoint> markers;
    QList<QGeoMapMarkerObject*> markerObjects;

    QGraphicsView* qgv;

#ifdef Q_OS_SYMBIAN
    QNetworkSession *session;
#endif

};

#endif // MAINWINDOW_H
