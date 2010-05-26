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

#ifndef ROUTETAB_H_
#define ROUTETAB_H_

#include <QWidget>

#include <QGeoRoutingManager>

class QTreeWidget;
class QLineEdit;

QTM_USE_NAMESPACE

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
    void replyFinished(QGeoRouteReply* reply);
    void resultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString);

private:
    QGeoRoutingManager *m_routingManager;
    QLineEdit *m_srcLong;
    QLineEdit *m_srcLat;
    QLineEdit *m_destLong;
    QLineEdit *m_destLat;
    QTreeWidget *m_resultTree;
};
#endif /* ROUTETAB_H_ */
