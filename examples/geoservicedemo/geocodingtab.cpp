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

#include "geocodingtab.h"
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

#include <qgeoaddress.h>

GeoCodingInputDialog::GeoCodingInputDialog(QString &obloc, QGeoAddress &address, QWidget *parent)
        : QDialog(parent), m_oblocStr(obloc), m_address(address)
{
    setWindowTitle(tr("Geocoding Parameters"));
    QLabel *obloclbl = new QLabel(tr("OneBox-Search:"));
    m_obloc = new QLineEdit(m_oblocStr);
    m_obloc->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QLabel *addressSearchlbl = new QLabel(tr("Search by address (Leave OneBox empty):"));
    QLabel *countrylbl = new QLabel(tr("Country:"));
    m_country = new QLineEdit(address.country());
    m_country->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *statelbl = new QLabel(tr("State:"));
    m_state = new QLineEdit(address.state());
    m_state->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *citylbl = new QLabel(tr("City:"));
    m_city = new QLineEdit(address.city());
    m_city->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *ziplbl = new QLabel(tr("Zip:"));
    m_zip = new QLineEdit(address.postcode());
    m_zip->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *streetlbl = new QLabel(tr("Street:"));
    m_street = new QLineEdit(address.street());
    m_street->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
    firstrow->addWidget(m_obloc);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *gridLayout = new QGridLayout ;
    gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(2, 1, 2, 1);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    gridLayout->addWidget(streetlbl, 1, 0);
    gridLayout->addWidget(m_street, 1, 1, 1, 3);

    gridLayout->addWidget(ziplbl, 2, 0);
    gridLayout->addWidget(citylbl, 2, 2);
    gridLayout->addWidget(m_zip, 2, 1);
    gridLayout->addWidget(m_city, 2, 3);

    gridLayout->addWidget(statelbl, 3, 0);
    gridLayout->addWidget(countrylbl, 3, 2);
    gridLayout->addWidget(m_state, 3, 1);
    gridLayout->addWidget(m_country, 3, 3);
#else
    gridLayout->addWidget(streetlbl, 1, 0);
    gridLayout->addWidget(m_street, 2, 0, 1, 2);

    gridLayout->addWidget(ziplbl, 3, 0);
    gridLayout->addWidget(citylbl, 3, 1);
    gridLayout->addWidget(m_zip, 4, 0);
    gridLayout->addWidget(m_city, 4, 1);

    gridLayout->addWidget(statelbl, 5, 0);
    gridLayout->addWidget(countrylbl, 5, 1);
    gridLayout->addWidget(m_state, 6, 0);
    gridLayout->addWidget(m_country, 6, 1);
#endif
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addWidget(obloclbl);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(addressSearchlbl);
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

}

void GeoCodingInputDialog::accept()
{
    m_oblocStr = m_obloc->text();

    m_address.setCountry(m_country->text());
    m_address.setState(m_state->text());
    m_address.setCity(m_city->text());
    m_address.setPostcode(m_zip->text());
    m_address.setStreet(m_street->text());

    QDialog::accept();
}


GeocodingTab::GeocodingTab(QWidget *parent) :
        QWidget(parent),
        m_searchManager(NULL)
{

    m_oblocStr = "Deutschland, München";

    m_requestBtn = new QPushButton(tr("Search By Address"));
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

GeocodingTab::~GeocodingTab()
{
}

void GeocodingTab::initialize(QGeoSearchManager *searchManager)
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

void GeocodingTab::on_btnRequest_clicked()
{
    if (m_searchManager) {
        GeoCodingInputDialog dlg(m_oblocStr, m_address, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_resultTree->clear();
            QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
            top->setText(0, tr("Geocode"));
            top->setText(1, tr("Requesting data"));

            if (!m_oblocStr.isEmpty()) {
                m_searchManager->search(m_oblocStr, QGeoSearchManager::SearchGeocode);
            } else {
                m_searchManager->geocode(m_address);
            }
        }
    } else {
        QMessageBox::warning(this, tr("Geocoding"), tr("No search manager available."));
    }
}

void GeocodingTab::replyFinished(QGeoSearchReply* reply)
{
    if (!isHidden() && reply->error() == QGeoSearchReply::NoError) {
        PlacePresenter presenter(m_resultTree, reply);
        presenter.show();
        reply->deleteLater();
    }
}

void GeocodingTab::resultsError(QGeoSearchReply* reply, QGeoSearchReply::Error errorCode, QString errorString)
{
    Q_UNUSED(errorCode)

    if (!isHidden()) {
        QTreeWidgetItem* errorResultItem = new QTreeWidgetItem(m_resultTree);
        errorResultItem->setText(0, tr("Error"));
        errorResultItem->setText(1, errorString);
        reply->deleteLater();
    }
}
