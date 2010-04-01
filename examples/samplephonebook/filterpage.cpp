/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "filterpage.h"

#include <QtGui>

FilterPage::FilterPage(QWidget* parent)
        : QWidget(parent)
{
    m_nameEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_addressEdit = new QLineEdit(this);
    QLabel *nameLabel = new QLabel(tr("Name"), this);
    QLabel *phoneLabel = new QLabel(tr("Phone"), this);
    QLabel *emailLabel = new QLabel(tr("Email"), this);
    QLabel *addressLabel = new QLabel(tr("Address"), this);

    QFormLayout *formLayout = new QFormLayout;
    if (QApplication::desktop()->availableGeometry().width() < 360) {
        // Narrow screen: put label on separate line to textbox
        formLayout->addRow(nameLabel);
        formLayout->addRow(m_nameEdit);
        formLayout->addRow(phoneLabel);
        formLayout->addRow(m_phoneEdit);
        formLayout->addRow(emailLabel);
        formLayout->addRow(m_emailEdit);
        formLayout->addRow(addressLabel);
        formLayout->addRow(m_addressEdit);
    } else {
        // Wide screen: put label on same line as textbox
        formLayout->addRow(nameLabel, m_nameEdit);
        formLayout->addRow(phoneLabel, m_phoneEdit);
        formLayout->addRow(emailLabel, m_emailEdit);
        formLayout->addRow(addressLabel, m_addressEdit);
    }

    QVBoxLayout *pageLayout = new QVBoxLayout;

    QScrollArea *formScrollArea = new QScrollArea(this);
    formScrollArea->setWidgetResizable(true);
    QWidget *formContainer = new QWidget(formScrollArea);
    formContainer->setLayout(formLayout);
    formScrollArea->setWidget(formContainer);
    pageLayout->addWidget(formScrollArea);

#ifdef Q_OS_SYMBIAN
    m_filterBtn = new QAction(tr("Filter"), this);
    m_filterBtn->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_filterBtn);
    connect(m_filterBtn, SIGNAL(triggered(bool)), this, SLOT(filterClicked()));
    m_cancelBtn = new QAction(tr("Cancel"), this);
    m_cancelBtn->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(m_cancelBtn);
    connect(m_cancelBtn, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));
#else
    m_filterBtn = new QPushButton(tr("&Filter"), this);
    m_filterBtn->setDefault(true);
    connect(m_filterBtn, SIGNAL(clicked()), this, SLOT(filterClicked()));
    m_cancelBtn = new QPushButton(tr("&Cancel"), this);
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_filterBtn);
    btnLayout->addWidget(m_cancelBtn);
    pageLayout->addLayout(btnLayout);
#endif

    setLayout(pageLayout);
}

FilterPage::~FilterPage()
{
}

void FilterPage::clearFilter()
{
    m_name.clear();
    m_phone.clear();
    m_email.clear();
    m_address.clear();
    m_nameEdit->clear();
    m_phoneEdit->clear();
    m_emailEdit->clear();
    m_addressEdit->clear();
    m_currentFilter = QContactIntersectionFilter();
    emit showListPage(m_currentFilter);
}

void FilterPage::filterClicked()
{
    m_name = m_nameEdit->text();
    m_phone = m_phoneEdit->text();
    m_email = m_emailEdit->text();
    m_address = m_addressEdit->text();
    // The intersection filter ensures that non-empty field value must be found in the contact.
    m_currentFilter = QContactIntersectionFilter();
    if (!m_nameEdit->text().isEmpty()) {
        // Search all fields of the name by building a union filter
        QContactUnionFilter nameFilter;
        QStringList nameFields;
        nameFields << QContactName::FieldCustomLabel;
        nameFields << QContactName::FieldFirstName;
        nameFields << QContactName::FieldLastName;
        nameFields << QContactName::FieldMiddleName;
        nameFields << QContactName::FieldPrefix;
        nameFields << QContactName::FieldSuffix;
        foreach (const QString& fieldName, nameFields) {
            QContactDetailFilter subFilter;
            subFilter.setDetailDefinitionName(QContactName::DefinitionName, fieldName);
            subFilter.setValue(m_nameEdit->text());
            subFilter.setMatchFlags(QContactFilter::MatchContains);
            nameFilter.append(subFilter);
        }
        m_currentFilter.append(nameFilter);
    }
    if (!m_phoneEdit->text().isEmpty()) {
        QContactDetailFilter phoneFilter;
        phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
                                            QContactPhoneNumber::FieldNumber);
        phoneFilter.setValue(m_phoneEdit->text());
        phoneFilter.setMatchFlags(QContactFilter::MatchContains);
        m_currentFilter.append(phoneFilter);
    }
    if (!m_emailEdit->text().isEmpty()) {
        QContactDetailFilter emailFilter;
        emailFilter.setDetailDefinitionName(QContactEmailAddress::DefinitionName,
                                            QContactEmailAddress::FieldEmailAddress);
        emailFilter.setValue(m_emailEdit->text());
        emailFilter.setMatchFlags(QContactFilter::MatchContains);
        m_currentFilter.append(emailFilter);
    }
    if (!m_addressEdit->text().isEmpty()) {
        // Search all fields of the address by building a union filter
        QContactUnionFilter addressFilter;
        QStringList addressFields;
        addressFields << QContactAddress::FieldCountry;
        addressFields << QContactAddress::FieldLocality;
        addressFields << QContactAddress::FieldPostcode;
        addressFields << QContactAddress::FieldPostOfficeBox;
        addressFields << QContactAddress::FieldRegion;
        addressFields << QContactAddress::FieldStreet;
        foreach (const QString& fieldName, addressFields) {
            QContactDetailFilter subFilter;
            subFilter.setDetailDefinitionName(QContactAddress::DefinitionName, fieldName);
            subFilter.setValue(m_addressEdit->text());
            subFilter.setMatchFlags(QContactFilter::MatchContains);
            addressFilter.append(subFilter);
        }
        m_currentFilter.append(addressFilter);
    }
    emit showListPage(m_currentFilter);
}

void FilterPage::cancelClicked()
{
    m_nameEdit->setText(m_name);
    m_phoneEdit->setText(m_phone);
    m_emailEdit->setText(m_email);
    m_addressEdit->setText(m_address);
    emit showListPage(m_currentFilter);
}
