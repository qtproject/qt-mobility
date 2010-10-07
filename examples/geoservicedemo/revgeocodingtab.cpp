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

#include "revgeocodingtab.h"
#include "placepresenter.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>

GeocodingCoordinateInputDialog::GeocodingCoordinateInputDialog(QGeoCoordinate& location, QWidget *parent)
        : QDialog(parent), m_location(location)
{
    setWindowTitle(tr("Reverse Geocoding Coordinates"));

    QLabel *locationlbl = new QLabel(tr("Location (lat,long):"));
    m_locLong = new QLineEdit(QString::number(location.longitude()));
    m_locLong->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_locLat = new QLineEdit(QString::number(location.latitude()));
    m_locLat->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_locLat);
    firstrow->addWidget(m_locLong);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(locationlbl);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void GeocodingCoordinateInputDialog::accept()
{
    m_location.setLatitude(m_locLat->text().toDouble());
    m_location.setLongitude(m_locLong->text().toDouble());
    QDialog::accept();
}

ReverseGeocodingTab::ReverseGeocodingTab(QWidget *parent) :
        QWidget(parent),
        m_searchManager(NULL)
{
    m_location.setLatitude(52.51);
    m_location.setLongitude(13.377);

    m_requestBtn = new QPushButton(tr("Search By Coordinates"));
    m_requestBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_requestBtn->setDisabled(true);
    QObject::connect(m_requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Elements") << tr("Value");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 240);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_requestBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(m_resultTree);
    setLayout(mainLayout);
}

ReverseGeocodingTab::~ReverseGeocodingTab()
{
}

void ReverseGeocodingTab::initialize(QGeoSearchManager *searchManager)
{
    m_resultTree->clear();
    m_searchManager = searchManager;
    if (m_searchManager) {
        QObject::connect(m_searchManager, SIGNAL(finished(QGeoSearchReply*)), this,
                         SLOT(replyFinished(QGeoSearchReply*)));
        QObject::connect(m_searchManager,
                         SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)), this,
                         SLOT(resultsError(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
        if (m_searchManager->supportsGeocoding())
            m_requestBtn->setDisabled(false);
    } else
        m_requestBtn->setDisabled(true);

}

void ReverseGeocodingTab::on_btnRequest_clicked()
{
    if (m_searchManager) {
        GeocodingCoordinateInputDialog dlg(m_location, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_resultTree->clear();
            QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
            top->setText(0, tr("Reverse Geocode"));
            top->setText(1, tr("Requesting data"));
            m_searchManager->reverseGeocode(m_location);
        }
    } else {
        QMessageBox::warning(this, tr("Reverse Geocoding"), tr("No search manager available."));
    }
}

void ReverseGeocodingTab::replyFinished(QGeoSearchReply* reply)
{
    if (!isHidden() && reply->error() == QGeoSearchReply::NoError) {
        m_resultTree->clear();
        PlacePresenter presenter(m_resultTree, reply);
        presenter.show();
        reply->deleteLater();
    }
}

void ReverseGeocodingTab::resultsError(QGeoSearchReply* reply, QGeoSearchReply::Error errorCode, QString errorString)
{
    Q_UNUSED(errorCode)

    if (!isHidden()) {
        m_resultTree->clear();
        QTreeWidgetItem* errorResultItem = new QTreeWidgetItem(m_resultTree);
        errorResultItem->setText(0, tr("Error"));
        errorResultItem->setText(1, errorString);
        reply->deleteLater();
    }
}
