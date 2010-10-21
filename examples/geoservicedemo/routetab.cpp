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

#include "routetab.h"
#include "routepresenter.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include <QDialogButtonBox>

RouteCoordinateInputDialog::RouteCoordinateInputDialog(QGeoCoordinate& src, QGeoCoordinate& dst, QWidget *parent)
        : QDialog(parent), m_src(src), m_dst(dst)
{
    setWindowTitle(tr("Route End Points"));

    QLabel *source = new QLabel(tr("Source (lat,long):"));
    m_srcLong = new QLineEdit(QString::number(src.longitude()));
    m_srcLong->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_srcLat = new QLineEdit(QString::number(src.latitude()));
    m_srcLat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *destination = new QLabel(tr("Destination (lat,long):"));
    m_destLong = new QLineEdit(QString::number(dst.longitude()));
    m_destLong->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_destLat = new QLineEdit(QString::number(dst.latitude()));
    m_destLat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_srcLat);
    firstrow->addWidget(m_srcLong);

    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->setSpacing(2);
    secondrow->setContentsMargins(2, 1, 2, 1);
    secondrow->addWidget(m_destLat);
    secondrow->addWidget(m_destLong);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(source);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(destination);
    mainLayout->addLayout(secondrow);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void RouteCoordinateInputDialog::accept()
{
    m_src.setLatitude(m_srcLat->text().toDouble());
    m_src.setLongitude(m_srcLong->text().toDouble());
    m_dst.setLatitude(m_destLat->text().toDouble());
    m_dst.setLongitude(m_destLong->text().toDouble());
    QDialog::accept();
}

RouteTab::RouteTab(QWidget *parent) :
        QWidget(parent),
        m_routingManager(0)
{
    m_src.setLatitude(53.546836);
    m_src.setLongitude(9.986572);
    m_dst.setLatitude(52.382306);
    m_dst.setLongitude(9.733887);

    m_requestTravelModes = QGeoRouteRequest::CarTravel;
    m_requestRouteOptimizations = QGeoRouteRequest::FastestRoute;

    m_requestBtn = new QPushButton(tr("Request Route"));
    m_requestBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_requestBtn->setDisabled(true);
    QObject::connect(m_requestBtn, SIGNAL(clicked(bool)),
                     this, SLOT(on_btnRequest_clicked()));
    m_updateBtn = new QPushButton(tr("Update Route"));
    m_updateBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_updateBtn->setDisabled(true);
    QObject::connect(m_updateBtn, SIGNAL(clicked(bool)),
                     this, SLOT(on_btnUpdate_clicked()));
    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Elements") << tr("Value");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setColumnWidth(0, 240);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_requestBtn);
    firstrow->addWidget(m_updateBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(m_resultTree);
    setLayout(mainLayout);
}

RouteTab::~RouteTab()
{
}

void RouteTab::initialize(QGeoRoutingManager *routingManager)
{
    m_resultTree->clear();
    m_routingManager = routingManager;
    if (m_routingManager) {
        QObject::connect(m_routingManager, SIGNAL(finished(QGeoRouteReply*)), this,
                         SLOT(replyFinished(QGeoRouteReply*)));
        QObject::connect(m_routingManager,
                         SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString)), this,
                         SLOT(resultsError(QGeoRouteReply*, QGeoRouteReply::Error, QString)));
        m_requestBtn->setDisabled(false);
    } else
        m_requestBtn->setDisabled(true);
}

void RouteTab::on_btnRequest_clicked()
{
    if (m_routingManager) {
        RouteCoordinateInputDialog dlg(m_src, m_dst, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_resultTree->clear();
            QTreeWidgetItem* waitInfoItem = new QTreeWidgetItem(m_resultTree);
            waitInfoItem->setText(0, tr("Route"));
            waitInfoItem->setText(1, tr("Calculating"));
            m_updateBtn->setDisabled(true);

            QGeoRouteRequest request(m_src, m_dst);
            if ((m_routingManager->supportedSegmentDetails() & QGeoRouteRequest::BasicSegmentData) != 0)
                request.setSegmentDetail(QGeoRouteRequest::BasicSegmentData);
            if ((m_routingManager->supportedManeuverDetails() & QGeoRouteRequest::BasicManeuvers) != 0)
                request.setManeuverDetail(QGeoRouteRequest::BasicManeuvers);
            request.setTravelModes(m_requestTravelModes);
            request.setRouteOptimization(m_requestRouteOptimizations);

            m_routingManager->calculateRoute(request);
        }
    } else {
        QMessageBox::warning(this, tr("Routing"), tr("No routing manager available."));
    }
}

void RouteTab::on_btnUpdate_clicked()
{
    if (m_routingManager && m_routes.count() > 0) {
        m_resultTree->clear();
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, tr("Route"));
        top->setText(1, tr("Updating"));
        m_updateBtn->setDisabled(true);
        m_routingManager->updateRoute(m_routes[0], m_src);
    } else {
        QMessageBox::warning(this, tr("Routing"), tr("Route update not available."));
    }
}

void RouteTab::replyFinished(QGeoRouteReply* reply)
{
    if (reply->error() == QGeoRouteReply::NoError) {
        m_routes = reply->routes();
        RoutePresenter presenter(m_resultTree, reply);
        presenter.show();
        reply->deleteLater();
        if (m_routes.count() > 0 && m_routingManager->supportsRouteUpdates())
            m_updateBtn->setDisabled(false);
    }
}

void RouteTab::resultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString)
{
    Q_UNUSED(error)

    QTreeWidgetItem* errorResultItem = new QTreeWidgetItem(m_resultTree);
    errorResultItem->setText(0, tr("Error"));
    errorResultItem->setText(1, errorString);
    reply->deleteLater();
}
