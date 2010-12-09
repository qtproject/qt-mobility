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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qgeomappingmanager.h>
#include <qgeoroutingmanager.h>
#include <qgeosearchmanager.h>
#include <qgeosearchreply.h>
#include <qgeoserviceprovider.h>
#include <qgeomappixmapobject.h>

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPixmap>
#include <QList>
#include <QTime>
#include <qnetworksession.h>

class QSlider;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QToolButton;
class QResizeEvent;
class QShowEvent;

QTM_USE_NAMESPACE
class MapWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool controlsVisible READ controlsVisible WRITE setControlsVisible)

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setControlsVisible(bool visible);
    bool controlsVisible();

protected:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent *);

private:
    void setupUi();
    void setProvider(QString providerId);
    void createMenus();
    void createPixmapIcon();

private slots:
    void demo1(bool checked);
    void demo2(bool checked);
    void demo3(bool checked);
    void drawRect(bool checked);
    void drawPixmap(bool checked);
    void drawPolyline(bool checked);
    void drawPolygon(bool checked);
    void drawCircle(bool checked);
    void drawText(bool checked);
    void calcRoute(bool checked);
    void customContextMenuRequest(const QPoint&);
    void routeFinished();
    void removePixmaps();
    void removeMapObject();
    void selectObjects();

    void searchClicked();
    void searchReplyFinished();
    void resultsError(QGeoSearchReply::Error errorCode, QString errorString);

    void sliderValueChanged(int zoomLevel);
    void mapZoomLevelChanged(qreal zoomLevel);
    void mapTypeToggled(int type);
    void mapTypeChanged(QGraphicsGeoMap::MapType type);
    void setCoordsClicked();
    void updateCoords(const QGeoCoordinate &coords);
    void sceneSelectionChanged();

    void networkSessionOpened();
    void error(QNetworkSession::SessionError error);

private:
    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    QGeoRoutingManager *m_routingManager;
    QGeoSearchManager *m_searchManager;

    MapWidget *m_mapWidget;
    QGraphicsPathItem* m_fullScreenButton;
    QMenu* m_popupMenu;
    QMenu* m_popupMenuMapObject;
    QPixmap m_markerIcon;
    QPoint m_markerOffset;
    QPoint m_lastClicked;
    QGeoMapObject* m_lastClickedMapObject;
    QList<QGeoMapPixmapObject*> m_markerObjects;

    QGraphicsView* m_qgv;
    QSlider* m_slider;
    QList<QAction*> m_mapControlActions;
    QList<QGraphicsGeoMap::MapType> m_mapControlTypes;
    QLineEdit *m_latitudeEdit;
    QLineEdit *m_longitudeEdit;
    QAction *m_captureCoordsAction;
    QDialog *m_coordControlDialog;
    QPushButton *m_setCoordsButton;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;

    QLayout *m_layout;
    bool m_controlsVisible;

    QNetworkSession *m_session;
};

#endif // MAINWINDOW_H
