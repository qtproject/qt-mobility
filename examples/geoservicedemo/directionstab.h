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

#ifndef DIRECTIONSTAB_H_
#define DIRECTIONSTAB_H_

#include <QWidget>
#include <QDialog>

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


class DirectionsInputDialog: public QDialog
{
    Q_OBJECT
public:
    DirectionsInputDialog(QString& start, QString& end, QGeoRouteRequest::TravelModes& travel, QWidget *parent = 0);
private slots:
    void accept();
private:
    QLineEdit* m_startpoint;
    QLineEdit* m_endpoint;
    QComboBox* m_transport;
    QString& m_start;
    QString& m_end;
    QGeoRouteRequest::TravelModes& m_travel;
};

class DirectionsTab: public QWidget
{
    Q_OBJECT

public:
    DirectionsTab(QWidget *parent = 0);
    ~DirectionsTab();

public slots:
    void initialize(QGeoRoutingManager *routingManager, QGeoSearchManager *searchManager);

private slots:
    void on_btnRequest_clicked();
    void on_btnUpdate_clicked();
    void routeReplyFinished(QGeoRouteReply* reply);
    void routeResultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString);
    void searchResultsError(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString);
    
    void searchStartFinished();
    void searchEndFinished();

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
    QPushButton *m_requestBtn;
    QPushButton *m_updateBtn;
    QList<QGeoRoute> m_routes;
};

#endif /* DIRECTIONSTAB_H_ */
