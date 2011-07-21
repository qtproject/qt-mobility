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

#ifndef ROUTETAB_H_
#define ROUTETAB_H_

#include <QWidget>
#include <QDialog>

#include <qgeoroutingmanager.h>
#include <qgeorouterequest.h>

class QTreeWidget;
class QLineEdit;
class QPushButton;

QTM_USE_NAMESPACE


class RouteCoordinateInputDialog: public QDialog
{
    Q_OBJECT
public:
    RouteCoordinateInputDialog(QGeoCoordinate& src, QGeoCoordinate& dst, QWidget *parent = 0);
private slots:
    void accept();
private:
    QGeoCoordinate& m_src;
    QGeoCoordinate& m_dst;
    QLineEdit* m_srcLong;
    QLineEdit* m_srcLat;
    QLineEdit* m_destLong;
    QLineEdit* m_destLat;
};

class RouteTab: public QWidget
{
    Q_OBJECT

public:
    RouteTab(QWidget *parent = 0);
    ~RouteTab();

public slots:
    void initialize(QGeoRoutingManager *routingManager);

private slots:
    void on_btnRequest_clicked();
    void on_btnUpdate_clicked();
    void replyFinished(QGeoRouteReply* reply);
    void resultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString);

private:
    QGeoRoutingManager *m_routingManager;
    QGeoCoordinate m_src;
    QGeoCoordinate m_dst;
    QTreeWidget *m_resultTree;
    QPushButton *m_requestBtn;
    QPushButton *m_updateBtn;
    QList<QGeoRoute> m_routes;
    QGeoRouteRequest::TravelModes m_requestTravelModes;
    QGeoRouteRequest::RouteOptimizations m_requestRouteOptimizations;
};
#endif /* ROUTETAB_H_ */
