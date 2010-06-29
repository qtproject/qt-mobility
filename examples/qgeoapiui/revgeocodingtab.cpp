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

ReverseGeocodingTab::ReverseGeocodingTab(QWidget *parent) :
        QWidget(parent),
        m_searchManager(NULL)
{
    QLabel *locationlbl = new QLabel(tr("Location (lat,long):"));
    m_locLong = new QLineEdit("13.377");
    m_locLong->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_locLat = new QLineEdit("52.51");
    m_locLat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *requestBtn = new QPushButton(tr("Reverse Geocoding"));
    requestBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Elements") << tr("Value");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 150);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(0);
    firstrow->setContentsMargins(0, 0, 0, 0);
    firstrow->addWidget(locationlbl);
    firstrow->addWidget(m_locLat);
    firstrow->addWidget(m_locLong);
    firstrow->addWidget(requestBtn);

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
    m_searchManager = searchManager;
    if (m_searchManager) {
        QObject::connect(m_searchManager, SIGNAL(finished(QGeoSearchReply*)), this,
                         SLOT(replyFinished(QGeoSearchReply*)));
        QObject::connect(m_searchManager,
                         SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)), this,
                         SLOT(resultsError(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
    }
}

void ReverseGeocodingTab::on_btnRequest_clicked()
{
    if (m_searchManager) {
        QGeoCoordinate coord(m_locLat->text().toDouble(), m_locLong->text().toDouble());

        m_resultTree->clear();

        m_searchManager->geocode(coord);
    } else {
        QMessageBox::warning(this, tr("Reverse GeoCoding"), tr("No geocoding service available."));
    }
}

void ReverseGeocodingTab::replyFinished(QGeoSearchReply* reply)
{
    if (!isHidden() && reply->error() == QGeoSearchReply::NoError) {
        PlacePresenter presenter(m_resultTree, reply);
        presenter.show();
        reply->deleteLater();
    }
}

void ReverseGeocodingTab::resultsError(QGeoSearchReply* reply, QGeoSearchReply::Error errorCode, QString errorString)
{
    Q_UNUSED(errorCode)

    if (!isHidden()) {
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, tr("Error"));
        top->setText(1, errorString);
        reply->deleteLater();
    }
}
