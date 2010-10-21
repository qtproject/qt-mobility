/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    QPixmap m_markerIcon;
    QPoint m_markerOffset;
    QPoint m_lastClicked;
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
