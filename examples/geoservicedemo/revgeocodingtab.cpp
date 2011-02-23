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
