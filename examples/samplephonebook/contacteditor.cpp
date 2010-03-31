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

#include "contacteditor.h"

#include <QtGui>

ContactEditor::ContactEditor(QWidget *parent)
        :QWidget(parent)
{
    m_manager = 0;
    m_contactId = QContactLocalId(0);

    m_nameEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_addrEdit = new QLineEdit(this);
    m_avatarBtn = new QPushButton(tr("Add image"), this);
    m_avatarBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_avatarBtn, SIGNAL(clicked()), this, SLOT(avatarClicked()));

    QFormLayout *detailsLayout = new QFormLayout;
    detailsLayout->addRow(new QLabel(tr("Name"), this));
    detailsLayout->addRow(m_nameEdit);
    detailsLayout->addRow(new QLabel(tr("Phone"), this));
    detailsLayout->addRow(m_phoneEdit);
    detailsLayout->addRow(new QLabel(tr("Email"), this));
    detailsLayout->addRow(m_emailEdit);
    detailsLayout->addRow(new QLabel(tr("Address"), this));
    detailsLayout->addRow(m_addrEdit);
    detailsLayout->addRow(new QLabel(tr("Avatar"), this));
    detailsLayout->addRow(m_avatarBtn);
    detailsLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    detailsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QScrollArea *detailsScrollArea = new QScrollArea(this);
    detailsScrollArea->setWidgetResizable(true);
    QWidget *detailsContainer = new QWidget(detailsScrollArea);
    detailsContainer->setLayout(detailsLayout);
    detailsScrollArea->setWidget(detailsContainer);

    QVBoxLayout *editLayout = new QVBoxLayout;
    editLayout->addWidget(detailsScrollArea);

#ifdef Q_OS_SYMBIAN
    // In symbian "save" and "cancel" buttons are softkeys.
    m_saveBtn = new QAction(tr("Save"), this);
    m_saveBtn->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_saveBtn);
    connect(m_saveBtn, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
    m_cancelBtn = new QAction(tr("Cancel"), this);
    m_cancelBtn->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(m_cancelBtn);
    connect(m_cancelBtn, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));
#else
    m_saveBtn = new QPushButton(tr("&Save"), this);
    m_saveBtn->setDefault(true);
    connect(m_saveBtn, SIGNAL(clicked()), this, SLOT(saveClicked()));
    m_cancelBtn = new QPushButton(tr("&Cancel"), this);
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addWidget(m_cancelBtn);
    editLayout->addLayout(btnLayout);
#endif

    setLayout(editLayout);
}

ContactEditor::~ContactEditor()
{
}

void ContactEditor::setCurrentContact(QContactManager* manager, QContactLocalId currentId)
{
    m_manager = manager;
    m_contactId = currentId;
    m_newAvatarPath = QString();

    // Clear UI
    m_nameEdit->clear();
    m_phoneEdit->clear();
    m_emailEdit->clear();
    m_addrEdit->clear();
    m_avatarBtn->setText("Add image");
    m_avatarBtn->setIcon(QIcon());

    if (manager == 0) {
        m_saveBtn->setEnabled(false);
        return;
    }

    // enable the UI.
    m_saveBtn->setEnabled(true);

    // otherwise, build from the contact details.
    QContact curr;
    if (m_contactId != QContactLocalId(0))
        curr = manager->contact(m_contactId);

    // Disable fields & buttons according to what the backend supports
    QMap<QString, QContactDetailDefinition> defs = m_manager->detailDefinitions(QContactType::TypeContact);

    // name
    //QContactName nm = curr.detail(QContactName::DefinitionName);
    if (m_contactId != QContactLocalId(0))
        m_nameEdit->setText(manager->synthesizedDisplayLabel(curr));

    // phonenumber
    QContactPhoneNumber phn = curr.detail(QContactPhoneNumber::DefinitionName);
    m_phoneEdit->setText(phn.value(QContactPhoneNumber::FieldNumber));

    // email
    if (defs.contains(QContactEmailAddress::DefinitionName)) {
        QContactEmailAddress em = curr.detail(QContactEmailAddress::DefinitionName);
        m_emailEdit->setText(em.value(QContactEmailAddress::FieldEmailAddress));
        m_emailEdit->setReadOnly(false);
    } else {
        m_emailEdit->setText("<not supported>");
        m_emailEdit->setReadOnly(true);
    }

    // address
    if (defs.contains(QContactAddress::DefinitionName)) {
        QContactAddress adr = curr.detail(QContactAddress::DefinitionName);
        m_addrEdit->setText(adr.value(QContactAddress::FieldStreet)); // ugly hack.
        m_addrEdit->setReadOnly(false);
    } else {
        m_addrEdit->setText("<not supported>");
        m_addrEdit->setReadOnly(true);
    }

    // avatar button
    if (defs.contains(QContactAvatar::DefinitionName)) {
        QContactAvatar av = curr.detail(QContactAvatar::DefinitionName);
        QContactThumbnail thumb = curr.detail(QContactThumbnail::DefinitionName);
        m_avatarBtn->setText(QString());
        m_avatarBtn->setIcon(QIcon());
        if (thumb.thumbnail().isNull()) {
            if (av.imageUrl().isEmpty()) {
                m_avatarBtn->setText("Add image");
            } else {
                m_avatarBtn->setIcon(QIcon(QPixmap(av.imageUrl().toLocalFile())));
                m_thumbnail = QImage(av.imageUrl().toLocalFile());
            }
        } else {
            m_newAvatarPath = av.imageUrl().toLocalFile();
            m_thumbnail = thumb.thumbnail();
            m_avatarBtn->setIcon(QIcon(QPixmap::fromImage(thumb.thumbnail())));
        }
        m_avatarBtn->setDisabled(false);
    } else {
        m_avatarBtn->setIcon(QIcon());
        m_avatarBtn->setText("<not supported>");
        m_avatarBtn->setDisabled(true);
    }
}

QString ContactEditor::nameField()
{
    // return the field which the name data should be saved in.
    if (!m_manager)
        return QString();

    QMap<QString, QContactDetailDefinition> defs = m_manager->detailDefinitions(QContactType::TypeContact);
    QContactDetailDefinition nameDef = defs.value(QContactName::DefinitionName);
    if (nameDef.fields().keys().contains(QContactName::FieldCustomLabel)) {
        return QString(QLatin1String(QContactName::FieldCustomLabel));
    } else if (nameDef.fields().keys().contains(QContactName::FieldFirstName)) {
        return QString(QLatin1String(QContactName::FieldFirstName));
    } else {
        return QString();
    }
}

void ContactEditor::avatarClicked()
{
    // put up a file dialog, and update the new avatar path.
    QString fileName = QFileDialog::getOpenFileName(this,
       tr("Select Avatar Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        m_newAvatarPath = fileName;
        m_thumbnail = QImage(m_newAvatarPath);
        m_avatarBtn->setText(QString());
        m_avatarBtn->setIcon(QIcon(m_newAvatarPath));
    }
}

void ContactEditor::saveClicked()
{
    if (!m_manager) {
        qWarning() << "No manager selected; cannot save.";
    } else {
        QContact curr;
        if (m_contactId != QContactLocalId(0))
            curr = m_manager->contact(m_contactId);

        if (m_nameEdit->text().isEmpty()) {
            QMessageBox::information(this, "Failed!", "You must give a name for the contact!");
            return;
        }

        if (m_nameEdit->text() != m_manager->synthesizedDisplayLabel(curr)) {
            // if the name has changed (ie, is different to the synthed label) then save it as a custom label.
            QString saveNameField = nameField();
            if (!saveNameField.isEmpty()) {
                QContactName nm = curr.detail(QContactName::DefinitionName);
                nm.setValue(saveNameField, m_nameEdit->text());
                curr.saveDetail(&nm);
            }
        }

        QContactPhoneNumber phn = curr.detail(QContactPhoneNumber::DefinitionName);
        phn.setNumber(m_phoneEdit->text());
        curr.saveDetail(&phn);

        if (!m_emailEdit->isReadOnly()) {
            QContactEmailAddress em = curr.detail(QContactEmailAddress::DefinitionName);
            em.setEmailAddress(m_emailEdit->text());
            curr.saveDetail(&em);
        }

        if (!m_addrEdit->isReadOnly()) {
            QContactAddress adr = curr.detail(QContactAddress::DefinitionName);
            adr.setStreet(m_addrEdit->text());
            curr.saveDetail(&adr);
        }

        if (m_avatarBtn->isEnabled()) {
            QContactAvatar av = curr.detail(QContactAvatar::DefinitionName);
            av.setImageUrl(QUrl(m_newAvatarPath));
            curr.saveDetail(&av);

            QContactThumbnail thumb = curr.detail(QContactThumbnail::DefinitionName);
            QImage img(m_thumbnail);
            thumb.setThumbnail(img);
            curr.saveDetail(&thumb);
        }

        bool success = m_manager->saveContact(&curr);
        if (!success)
            QMessageBox::information(this, "Failed!", QString("Failed to save contact!\n(error code %1)").arg(m_manager->error()));
    }

    emit showListPage();
}

void ContactEditor::cancelClicked()
{
    emit showListPage();
}
