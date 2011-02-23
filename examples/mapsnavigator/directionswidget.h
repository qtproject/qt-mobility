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

#ifndef DIRECTIONSWIDGET_H_
#define DIRECTIONSWIDGET_H_

#include "markerlist.h"
#include "mapswidget.h"

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>

#include <qgeosearchmanager.h>
#include <qgeoroutingmanager.h>
#include <qgeorouterequest.h>
#include <qgeoroutesegment.h>
#include <qgeomaneuver.h>

class QListWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QColor;

QTM_USE_NAMESPACE

class Waypoint: public QHBoxLayout
{
    Q_OBJECT
public:
    Waypoint(const QString &address, int number, QWidget *parent = 0);
    QString searchAddress() const;
    void updateText();

private slots:
    void selectMapPoint();

signals:
    void selectWaypoint(MarkerObject *object);

public:
    int markerNumber;
    MarkerObject *marker;
    QLineEdit *waypoint;
    QPushButton *markerButton;
};

class DirectionsInputDialog: public QDialog
{
    Q_OBJECT
public:
    DirectionsInputDialog(QString& start, QString& end, QGeoRouteRequest::TravelModes& travel, QWidget *parent = 0);
    ~DirectionsInputDialog();

public slots:
    void setWaypointList(QList<Waypoint*> list);

private slots:
    void accept();

private:
    QList<Waypoint*> m_waypointList;

    QString& m_start;
    QString& m_end;

    Waypoint *waypointStart;
    Waypoint *waypointEnd;

    QComboBox* m_transport;
    QGeoRouteRequest::TravelModes& m_travel;

    QVBoxLayout *mainLayout;
};

class DirectionsWidget: public QWidget
{
    Q_OBJECT

public:
    DirectionsWidget(QWidget *parent = 0);
    ~DirectionsWidget();

    void addWaypoint(Waypoint *waypoint);

public slots:
    void initialize(QGeoRoutingManager *routingManager, QGeoSearchManager *searchManager, MapsWidget *mapswidget);
    void waypointSelected();

private slots:
    void on_btnUpdate_clicked();
    void routeReplyFinished(QGeoRouteReply* reply);
    void routeResultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString);
    void searchResultsError(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString);

    void searchStartFinished();
    void searchEndFinished();

    void showDirectionsDialog();

    void setCurrentWaypoint(MarkerObject* marker);
    void setWaypoint(MarkerObject* marker);

signals:
    void selectWaypoint(MarkerObject *object);
    void updateWaypoint(MarkerObject *object);

private:
    void getDirections();
    void addDirection(const QString& icon, const QString& text, const QColor& color);
    QString address(const QGeoPlace& place) const;

    QGeoRoutingManager *m_routingManager;
    QGeoSearchManager *m_searchManager;

    QGeoSearchReply *m_startReply;
    QGeoSearchReply *m_endReply;

    QString m_startpoint;
    QString m_endpoint;
    QGeoPlace m_start;
    QGeoPlace m_end;
    QGeoRouteRequest::TravelModes m_requestTravelModes;
    QGeoRouteRequest::RouteOptimizations m_requestRouteOptimizations;

    bool directionsRequest;

    QListWidget *m_resultList;
    QPushButton *m_updateBtn;
    QList<QGeoRoute> m_routes;

    QList<Waypoint*> waypointList;
    MapsWidget *m_mapsWidget;

    DirectionsInputDialog *m_dialog;

    Waypoint *currentWaypoint;
};

#endif /* DIRECTIONSWIDGET_H_ */
