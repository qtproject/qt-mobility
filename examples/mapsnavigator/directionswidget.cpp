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

#include "directionswidget.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QComboBox>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QColor>
#include <QBrush>
#include <QPixmap>

#include <QDialogButtonBox>

//TODO: get rid of this when finalised
#include <QDebug>

Waypoint::Waypoint(const QString& address, int number, QWidget *parent)
    : QHBoxLayout(parent), markerNumber(number), marker(0)
{
    qDebug() << "MY NUMBER" << markerNumber;
    waypoint = new QLineEdit(address);
    waypoint->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    waypoint->setCursorPosition(0);

    markerButton = new QPushButton("M");
    markerButton->setMaximumWidth(25);
    connect(markerButton, SIGNAL(clicked()), this, SLOT(selectMapPoint()));

    addWidget(waypoint);
    addWidget(markerButton);

    if (!marker) {
        QString iconPath;
        if (markerNumber == 0) {
            iconPath = ":/icons/startmarker.png";
            marker = new MarkerObject(MarkerObject::Startpoint);
        } else if (markerNumber < 0) {
            iconPath = ":/icons/endmarker.png";
            marker = new MarkerObject(MarkerObject::Endpoint);
        } else {
            iconPath = ":/icons/waypointmarker" + QString::number(markerNumber) + ".png";
            marker = new MarkerObject(MarkerObject::Waypoint);
        }

        // sets waypoint on a higher map layer
        marker->setZValue(1);
        marker->setIconPath(iconPath);
    }
}

void Waypoint::selectMapPoint()
{
    emit selectWaypoint(marker);
}

QString Waypoint::searchAddress() const
{
    return waypoint->text();
}

void Waypoint::updateText()
{
    /* TODO: UPDATE TEXT FIELDS
    if (marker)
        qDebug() << "NOTHING";
    else
        qDebug() << "SOMETHING";
    */
}

DirectionsInputDialog::DirectionsInputDialog(QString& start, QString& end, QGeoRouteRequest::TravelModes& travel, QWidget *parent)
        : QDialog(parent), m_start(start), m_end(end), m_travel(travel)
{
    setWindowTitle(tr("Directions"));

    m_transport = new QComboBox();
    m_transport->addItem("Car");
    m_transport->setItemData(0, QVariant(QGeoRouteRequest::CarTravel));
    m_transport->addItem("Pedestrian");
    m_transport->setItemData(1, QVariant(QGeoRouteRequest::PedestrianTravel));
    m_transport->addItem("Bicycle");
    m_transport->setItemData(2, QVariant(QGeoRouteRequest::BicycleTravel));
    m_transport->addItem("Public Transport");
    m_transport->setItemData(3, QVariant(QGeoRouteRequest::PublicTransitTravel));
    m_transport->addItem("Truck");
    m_transport->setItemData(4, QVariant(QGeoRouteRequest::TruckTravel));
    m_transport->setCurrentIndex(m_transport->findData(QVariant(m_travel)));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(m_transport);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

DirectionsInputDialog::~DirectionsInputDialog()
{
}

void DirectionsInputDialog::setWaypointList(QList<Waypoint*> list)
{
    m_waypointList = list;
    waypointStart = m_waypointList[0];
    waypointEnd = m_waypointList[m_waypointList.size() - 1];
    waypointStart->updateText();
    waypointEnd->updateText();

    if (!waypointStart->parent())
        mainLayout->insertLayout(0, waypointStart);

    if (!waypointEnd->parent())
        mainLayout->insertLayout(1, waypointEnd);
}

void DirectionsInputDialog::accept()
{
    qDebug() << "ACCEPTING ROUTES";
    //m_start = waypointStart->searchAddress();
    //m_end = waypointEnd->searchAddress();
    switch (m_transport->itemData(m_transport->currentIndex()).toInt()) {
        case 1:
            m_travel = QGeoRouteRequest::CarTravel;
            break;
        case 2:
            m_travel = QGeoRouteRequest::PedestrianTravel;
            break;
        case 4:
            m_travel = QGeoRouteRequest::BicycleTravel;
            break;
        case 8:
            m_travel = QGeoRouteRequest::PublicTransitTravel;
            break;
        case 16:
            m_travel = QGeoRouteRequest::TruckTravel;
            break;
        default:
            m_travel = QGeoRouteRequest::CarTravel;
            break;
    }

    /*
    if (m_start.isEmpty() || m_start.startsWith("Start Point:")) {
        QMessageBox::warning(this, tr("Directions"), tr("Please provide a starting point"));
        return;
    }

    if (m_end.isEmpty() || m_end.startsWith("End Point:")) {
        QMessageBox::warning(this, tr("Directions"), tr("Please provide an end point"));
        return;
    }
    */

    QDialog::accept();
}

DirectionsWidget::DirectionsWidget(QWidget *parent) :
        QWidget(parent),
        m_routingManager(0),
        m_searchManager(0),
        m_startReply(0),
        m_endReply(0),
        currentWaypoint(0)
{
    m_startpoint = "53 Brandl St, Eight Mile Plains, Australia";
    //m_startpoint = "Start Point:";
    m_endpoint = "End Point:";

    m_requestTravelModes = QGeoRouteRequest::CarTravel;
    m_requestRouteOptimizations = QGeoRouteRequest::FastestRoute;

    m_updateBtn = new QPushButton(tr("Update Route"));
    m_updateBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_updateBtn->setDisabled(true);
    QObject::connect(m_updateBtn, SIGNAL(clicked(bool)),
                     this, SLOT(on_btnUpdate_clicked()));

    m_resultList = new QListWidget();
    m_resultList->setWordWrap(true);
    m_resultList->setIconSize(QSize(30, 30));

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_updateBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(m_resultList);
    setLayout(mainLayout);
}

DirectionsWidget::~DirectionsWidget()
{
}

void DirectionsWidget::initialize(QGeoRoutingManager *routingManager, QGeoSearchManager *searchManager, MapsWidget *mapsWidget)
{
    m_resultList->clear();

    m_mapsWidget = mapsWidget;

    m_dialog = new DirectionsInputDialog(m_startpoint, m_endpoint, m_requestTravelModes, this);

    Waypoint *waypointStart = new Waypoint(m_startpoint, 0);
    Waypoint *waypointEnd = new Waypoint(m_endpoint, -1);
    addWaypoint(waypointStart);
    addWaypoint(waypointEnd);

    m_routingManager = routingManager;
    if (m_routingManager) {
        QObject::connect(m_routingManager, SIGNAL(finished(QGeoRouteReply*)), this,
                         SLOT(routeReplyFinished(QGeoRouteReply*)));
        QObject::connect(m_routingManager,
                         SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString)), this,
                         SLOT(routeResultsError(QGeoRouteReply*, QGeoRouteReply::Error, QString)));
    } else {
        m_resultList->addItem("\nNo routing manager available.\n");
        return;
    }

    m_searchManager = searchManager;
    if (m_searchManager) {
        QObject::connect(m_searchManager,
                         SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)), this,
                         SLOT(searchResultsError(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
        if (m_searchManager->supportsGeocoding())
            qDebug() << "WAS REQUEST_BUTTON BUT MEH";
    } else {
        m_resultList->addItem("\nNo search manager available.\n");
    }
}

void DirectionsWidget::showDirectionsDialog()
{
    m_dialog->setWaypointList(waypointList);

    int ret = m_dialog->exec();
    if (ret == QDialog::Accepted) {
        //TODO: implement routing

    } else if (ret == QDialog::Rejected) {
        if (currentWaypoint)
            return;

        for (int i=0; i<waypointList.size(); i++) {
            if (!waypointList[i]->marker)
                continue;

            if (waypointList[i]->marker->markerType() == MarkerObject::MyLocation) {
                qDebug() << "MY LOCATION DONT ERASE";
            } else {
                qDebug() << "ERASE THIS";
                m_mapsWidget->deleteMarker(waypointList[i]->marker);
                waypointList[i]->marker = 0;
            }
        }
    }
}

/*
void DirectionsWidget::on_btnRequest_clicked()
{
    if (!m_routingManager)
        QMessageBox::warning(this, tr("Directions"), tr("No routing manager available."));

    if (!m_searchManager)
        QMessageBox::warning(this, tr("Directions"), tr("No search manager available."));

    DirectionsInputDialog dialog(m_startpoint, m_endPoint, m_requestTravelModes, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_resultList->clear();
        m_updateBtn->setDisabled(true);

        m_startReply = m_searchManager->search(m_startpoint, QGeoSearchManager::SearchGeocode);
        QObject::connect(m_startReply, SIGNAL(finished()), this, SLOT(searchStartFinished()));

        m_endReply = m_searchManager->search(m_endPoint, QGeoSearchManager::SearchGeocode);
        QObject::connect(m_endReply, SIGNAL(finished()), this, SLOT(searchEndFinished()));

        m_resultList->addItem("\nRequesting directions...\n");
    }
}
*/

void DirectionsWidget::on_btnUpdate_clicked()
{
    if (m_routingManager && m_routes.count() > 0) {
        m_resultList->clear();
        m_resultList->addItem("\nUpdating directions...\n");
        m_updateBtn->setDisabled(true);
        m_routingManager->updateRoute(m_routes[0], m_start.coordinate());
    } else {
        QMessageBox::warning(this, tr("Directions"), tr("Route update not available."));
    }
}

void DirectionsWidget::getDirections()
{
    if (m_startReply)
        m_startReply->deleteLater();
    m_startReply = 0;

    if (m_endReply)
        m_endReply->deleteLater();
    m_endReply = 0;

    QGeoRouteRequest request(m_start.coordinate(), m_end.coordinate());
    request.setTravelModes(m_requestTravelModes);
    request.setRouteOptimization(m_requestRouteOptimizations);

    if ((m_routingManager->supportedSegmentDetails() & QGeoRouteRequest::BasicSegmentData) != 0)
        request.setSegmentDetail(QGeoRouteRequest::BasicSegmentData);

    if ((m_routingManager->supportedManeuverDetails() & QGeoRouteRequest::BasicManeuvers) != 0)
        request.setManeuverDetail(QGeoRouteRequest::BasicManeuvers);

    directionsRequest = true;
    m_routingManager->calculateRoute(request);
}


void DirectionsWidget::searchStartFinished()
{
    if (m_startReply->error() == QGeoSearchReply::NoError) {
        QList<QGeoPlace> places = m_startReply->places();
        m_start = places[0];

        if (m_endReply && m_endReply->isFinished())
            getDirections();
    } else {
        m_startReply->deleteLater();
        m_startReply = 0;
    }
}

void DirectionsWidget::searchEndFinished()
{
    if (m_endReply->error() == QGeoSearchReply::NoError) {
        QList<QGeoPlace> places = m_endReply->places();
        m_end = places[0];

        if (m_startReply && m_startReply->isFinished())
            getDirections();
    } else {
        m_endReply->deleteLater();
        m_endReply = 0;
    }

}

void DirectionsWidget::addDirection(const QString& icon, const QString& text, const QColor& color)
{
   QListWidgetItem *item = new QListWidgetItem('\n' + text + '\n');
   item->setBackground(QBrush(color));

   if (icon.isEmpty()) {
       QPixmap blank(23,1);
       blank.fill(Qt::transparent);
       item->setIcon(QIcon(blank));
   } else {
       item->setIcon(QIcon(icon));
   }

   m_resultList->addItem(item);
}

void DirectionsWidget::routeReplyFinished(QGeoRouteReply* reply)
{
    if (!directionsRequest)
        return;

    if (reply->error() == QGeoRouteReply::NoError) {
        m_resultList->clear();

        m_routes = reply->routes();
        for (int i=0; i<m_routes.size(); i++) {
            const QString& startAddress = address(m_start);
            const QString& endAddress = address(m_end);
            const QString& totalDistance = QString::number(m_routes[i].distance()/1000.00) + "km";
            const QString& totalTime = QString::number(qRound(m_routes[i].travelTime()/60.00)) + "min";

            addDirection(":/icons/startmarker.png", startAddress, QColor("lightblue"));
            addDirection(":/icons/endmarker.png", endAddress, QColor("lightgreen"));
            addDirection("", totalDistance + " - approx " + totalTime, QColor("lightgreen"));

            int count = 0;
            QGeoRouteSegment segment = m_routes[i].firstRouteSegment();
            while (segment.isValid()) {
                count++;
                const QString& instruction = segment.maneuver().instructionText();
                if (count%2 > 0)
                    addDirection(":/icons/pathmarker.png", instruction, QColor("white"));
                else
                    addDirection(":/icons/pathmarker.png", instruction, QColor("lightgray"));
                segment = segment.nextRouteSegment();
            }

            if (++count%2 > 0)
                addDirection(":/icons/endmarker.png", endAddress, QColor("white"));
            else
                addDirection(":/icons/endmarker.png", endAddress, QColor("lightgray"));
        }

        reply->deleteLater();

        if (m_routes.count() > 0 && m_routingManager->supportsRouteUpdates())
            m_updateBtn->setDisabled(false);
    }
}

void DirectionsWidget::routeResultsError(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString)
{
    Q_UNUSED(error)

    m_resultList->clear();
    m_resultList->addItem("\nError! " + errorString + "\n");
    reply->deleteLater();
}

void DirectionsWidget::searchResultsError(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString)
{
    Q_UNUSED(error)

    m_resultList->clear();
    m_resultList->addItem("\nError! " + errorString + "\n");
    reply->deleteLater();
}

QString DirectionsWidget::address(const QGeoPlace& place) const
{
    QGeoAddress addr = place.address();
    QString addrStr = addr.street() + ",\n"
                    + addr.city() + " " + addr.postcode() + ",\n"
                    + addr.country();
    return addrStr;
}

void DirectionsWidget::addWaypoint(Waypoint *waypoint)
{
    waypointList.append(waypoint);
    m_mapsWidget->createMarker(waypoint->marker);

    connect(waypoint, SIGNAL(selectWaypoint(MarkerObject*)),
            this, SLOT(setCurrentWaypoint(MarkerObject*)));
}

void DirectionsWidget::setCurrentWaypoint(MarkerObject *marker)
{
    currentWaypoint = qobject_cast<Waypoint*>(sender());
    emit selectWaypoint(marker);
    m_dialog->close();
}

void DirectionsWidget::waypointSelected()
{
    emit updateWaypoint(currentWaypoint->marker);
    currentWaypoint = 0;
    showDirectionsDialog();
}

void DirectionsWidget::setWaypoint(MarkerObject* marker)
{
    /*
    if (currentWaypoint->marker->markerType() != MarkerObject::Search) {
        marker->setIconPath(currentWaypoint->marker->iconPath());
        m_mapsWidget->deleteMarker(currentWaypoint->marker);
    } else {
        currentWaypoint->marker->setIconPath(":/icons/searchmarker.png");
    }
    */

    //marker->setIconPath(currentWaypoint->marker->iconPath());
    //m_mapsWidget->deleteMarker(currentWaypoint->marker);

    currentWaypoint->marker->setCoordinate(marker->coordinate());
    waypointSelected();
}
