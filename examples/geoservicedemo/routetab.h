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
