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

#include "searchwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

//TODO: get rid of debug when done
#include <QDebug>

SearchGeocodeDialog::SearchGeocodeDialog(QString& address, QWidget *parent)
    : QDialog(parent), m_address(address)
{
    setWindowTitle(tr("Search"));

    m_search = new QLineEdit("15 Googie Cr, Eight Mile Plains, Australia");
    m_search->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(new QLabel(tr("Address")));
    mainLayout->addWidget(m_search);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void SearchGeocodeDialog::accept()
{
    if (m_search->text().isEmpty()) {
        QMessageBox::warning(this, tr("Search"), tr("Please provide an address"));
        return;
    }

    m_address = m_search->text();
    QDialog::accept();
}

SearchRevGeocodeDialog::SearchRevGeocodeDialog(QGeoCoordinate& coord, QWidget *parent)
    : QDialog(parent), m_coord(coord)
{
    setWindowTitle(tr("Search"));

    m_longitude = new QLineEdit("-27.5734");
    m_longitude->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    m_latitude = new QLineEdit("153.098");
    m_latitude->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(new QLabel(tr("Longitude")));
    mainLayout->addWidget(m_longitude);
    mainLayout->addWidget(new QLabel(tr("Latitude")));
    mainLayout->addWidget(m_latitude);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void SearchRevGeocodeDialog::accept()
{
    if (m_longitude->text().isEmpty()) {
        QMessageBox::warning(this, tr("Search"), tr("Please provide a longitude value"));
        return;
    }

    if (m_latitude->text().isEmpty()) {
        QMessageBox::warning(this, tr("Search"), tr("Please provide a latitude value"));
        return;
    }

    //TODO: check for valid double input
    m_coord = QGeoCoordinate(m_longitude->text().toDouble(), m_latitude->text().toDouble());
    QDialog::accept();
}

SearchWidget::SearchWidget(QWidget *parent)
    : QDialog(parent), m_searchManager(0), m_searchReply(0), m_marker(0)
{
    setWindowTitle(tr("Marker"));

    m_name = new QLabel();
    m_name->setFont(QFont("Times", 14, QFont::Bold));

    m_street = new QLabel();
    m_street->setFont(QFont("Times", 12, QFont::Normal));

    m_city = new QLabel();
    m_city->setFont(QFont("Times", 12, QFont::Normal));

    m_distance = new QLabel();
    m_distance->setFont(QFont("Times", 10, true));

    m_latitude = new QLabel();
    m_longitude= new QLabel();

    //TODO: maybe change to icons
    deleteButton = new QPushButton(tr("Delete"));
    waypointButton = new QPushButton(tr("Set Waypoint"));
    okButton = new QPushButton(tr("OK"));

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteMarker()));
    connect(waypointButton, SIGNAL(clicked()), this, SLOT(setWaypoint()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(hide()));

    QHBoxLayout *buttonBox = new QHBoxLayout;
    buttonBox->addWidget(deleteButton);
    buttonBox->addWidget(waypointButton);
    buttonBox->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(m_name);
    mainLayout->addWidget(m_street);
    mainLayout->addWidget(m_city);
    mainLayout->addWidget(m_distance);
    mainLayout->addWidget(m_latitude);
    mainLayout->addWidget(m_longitude);
    mainLayout->addLayout(buttonBox);
    setLayout(mainLayout);
}

SearchWidget::~SearchWidget()
{
}

void SearchWidget::initialize(QGeoSearchManager *manager, MarkerObject *myLocationMarker)
{
    m_searchManager = manager;
    myLocation = myLocationMarker->coordinate();
}

void SearchWidget::clearWidget()
{
    m_street->clear();
    m_city->clear();
    m_distance->clear();
    m_latitude->clear();
    m_longitude->clear();

    deleteButton->setEnabled(false);
    waypointButton->setEnabled(false);
}

void SearchWidget::updateWidget()
{
    setupWidget(m_marker);
}

void SearchWidget::errorWidget()
{
    clearWidget();
    m_city->setText("Error! invalid location");
    emit deleteMarker(m_marker);
}


void SearchWidget::setupWidget(MarkerObject* marker)
{
    if (!marker)
        return;

    m_marker = marker;
    m_name->setText(marker->name());

    if (marker->markerState() == MarkerObject::Error) {
        errorWidget();
        return;
    }

    if (marker->markerState() == MarkerObject::Searching) {
        clearWidget();
        m_city->setText("Loading...");

        connect(marker->searchReply(), SIGNAL(finished()),
                this, SLOT(updateWidget()));

        return;
    }

    //TODO: maybe hide the buttons instead of disable
    if (m_marker->markerType() == MarkerObject::Search ||
            m_marker->markerType() == MarkerObject::Waypoint) {
        deleteButton->setEnabled(true);
        waypointButton->setEnabled(true);
    } else {
        deleteButton->setEnabled(false);
        waypointButton->setEnabled(false);
    }

    QGeoAddress address = m_marker->place().address();
    QGeoCoordinate coord = m_marker->place().coordinate();

    m_street->setText(address.street());
    m_city->setText(address.city() + ", " + address.postcode() + ", " + address.country());
    m_longitude->setText("Longitude: " + QString::number(coord.longitude()));
    m_latitude->setText("Latitude: " + QString::number(coord.latitude()));

    double distance = myLocation.distanceTo(coord);
    double degrees = myLocation.azimuthTo(coord);

    //TODO: functionalize the distance/bearing conversions
    QString bearing = "N";
    if (360-6 < degrees && degrees < 6)
        bearing = "N";
    else if (6 <= degrees && degrees <= 90-6)
        bearing = "NE";
    else if (90-6 < degrees && degrees < 90+6)
        bearing = "E";
    else if (90+6 <= degrees && degrees <= 180-6)
        bearing = "SE";
    else if (180-6 < degrees && degrees < 180+6)
        bearing = "S";
    else if (180+6 <= degrees && degrees <= 270-6)
        bearing = "SW";
    else if (270-6 < degrees && degrees < 270+6)
        bearing = "W";
    else if (270+6 <= degrees && degrees <= 360-6)
        bearing = "NW";

    if (distance > 1000) {
        distance = distance / 1000;
        distance = static_cast<int>(distance*10+0.5)/10.0;
        m_distance->setText(QString::number(distance) + "km " + bearing);
    } else {
        m_distance->setText(QString::number(qRound(distance)) + "m " + bearing);
    }

    if (marker->center()) {
        emit centerOnMarker(m_marker);
        marker->setCenter(false);
    }
}

void SearchWidget::deleteMarker()
{
    emit deleteMarker(m_marker);
    hide();
}

void SearchWidget::showGeocodeDialog()
{
    QString searchAddress;
    SearchGeocodeDialog dialog(searchAddress, this);
    if (dialog.exec() == QDialog::Accepted)
        emit addMarker(searchAddress);
}

void SearchWidget::showRevGeocodeDialog()
{
    QGeoCoordinate searchCoord;
    SearchRevGeocodeDialog dialog(searchCoord, this);
    if (dialog.exec() == QDialog::Accepted)
        emit addMarker(searchCoord);
}

void SearchWidget::setWaypoint()
{
    hide();
    emit setWaypoint(m_marker);
}
