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

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "routetab.h"

#include "routepresenter.h"
#include "qgeoroutingservice_nokia_p.h"

RouteTab::RouteTab(QWidget *parent) :
    QWidget(parent)
{
    QGeoRoutingServiceNokia *service = new QGeoRoutingServiceNokia();    
    //service->setHost("route.maps.svc.ovi.com");
    
    routingService=service;
    QObject::connect(routingService, SIGNAL(finished(QGeoRouteReply*)),
                     this, SLOT(replyFinished(QGeoRouteReply*)));

    QLabel *source = new QLabel(tr("Source:"));
    srcLong = new QLineEdit("10");
    srcLat = new QLineEdit("50");
    
    QLabel *destination = new QLabel(tr("Destination:"));
    destLong = new QLineEdit("1");
    destLat = new QLineEdit("48");
    QPushButton *requestBtn = new QPushButton(tr("Request Route"));
    QObject::connect(requestBtn, SIGNAL(clicked(bool)),
                     this, SLOT(on_btnRequest_clicked()));
    resultTree = new QTreeWidget();
    QStringList labels;
    labels << "Elements" << "Value";
    resultTree->setHeaderLabels(labels);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->addWidget(source);
    firstrow->addWidget(srcLong);
    firstrow->addWidget(srcLat);
    firstrow->addWidget(requestBtn);
    firstrow->addStretch(1);
    
    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->addWidget(destination);
    secondrow->addWidget(destLong);
    secondrow->addWidget(destLat);
    secondrow->addStretch(1);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstrow);
    mainLayout->addLayout(secondrow);
    mainLayout->addWidget(resultTree);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

RouteTab::~RouteTab()
{
    delete routingService;
}

void RouteTab::on_btnRequest_clicked()
{
    QGeoCoordinate src(srcLat->text().toDouble(), srcLong->text().toDouble());
    QGeoCoordinate dst(destLat->text().toDouble(), destLong->text().toDouble());
    QGeoRouteRequest request;

    request.setSource(src);
    request.setDestination(dst);

    resultTree->clear();
    routingService->getRoute(request);
}

void RouteTab::replyFinished(QGeoRouteReply* reply)
{
    RoutePresenter presenter(resultTree, reply);
    presenter.show();
    reply->deleteLater();
}
