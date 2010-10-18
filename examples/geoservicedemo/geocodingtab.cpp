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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
