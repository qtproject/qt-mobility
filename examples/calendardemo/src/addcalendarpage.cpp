/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
#include "addcalendarpage.h"

#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

AddCalendarPage::AddCalendarPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0)
{
#if defined(Q_WS_MAEMO_5)
    // Maemo5 calendar features
    QLabel *nameLabel = new QLabel("Name:", this);
    m_nameEdit = new QLineEdit(this);

    QLabel *colorLabel = new QLabel("Color:", this);
    m_colorComboBox = new QComboBox(this); // must be filled later
    connect(m_colorComboBox, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(colorChanged(const QString&)));

    QLabel *typeLabel = new QLabel("Type:", this);
    m_typeComboBox = new QComboBox(this); // must be filled later
    connect(m_typeComboBox, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(typeChanged(const QString&)));

    m_visibleCheckBox = new QCheckBox("Visible", this);
    connect(m_visibleCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(visibilityChanged(int)));

    m_readonlyCheckBox = new QCheckBox("Readonly", this);
    connect(m_readonlyCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(readonlyChanged(int)));
#endif

#ifndef Q_OS_SYMBIAN
    // Add push buttons for non-Symbian platforms as they do not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Save", this);
    connect(okButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
    hbLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    hbLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *scrollAreaLayout = new QVBoxLayout();

#if defined(Q_WS_MAEMO_5)
    scrollAreaLayout->addWidget(nameLabel);
    scrollAreaLayout->addWidget(m_nameEdit);
    scrollAreaLayout->addWidget(colorLabel);
    scrollAreaLayout->addWidget(m_colorComboBox);
    scrollAreaLayout->addWidget(typeLabel);
    scrollAreaLayout->addWidget(m_typeComboBox);

    QHBoxLayout *checkBoxLayout = new QHBoxLayout();
    checkBoxLayout->addWidget(m_visibleCheckBox);
    checkBoxLayout->addWidget(m_readonlyCheckBox);
    scrollAreaLayout->addLayout(checkBoxLayout);
#endif

    scrollAreaLayout->addStretch();

#ifndef Q_OS_SYMBIAN
    scrollAreaLayout->addLayout(hbLayout);
#endif

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *formContainer = new QWidget(scrollArea);
    formContainer->setLayout(scrollAreaLayout);
    scrollArea->setWidget(formContainer);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    // Add softkeys
    QAction* cancelSoftKey = new QAction("Cancel", this);
    cancelSoftKey->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(cancelSoftKey);
    connect(cancelSoftKey, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));

    QAction* saveSoftKey = new QAction("Save",this);
    saveSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(saveSoftKey);
    connect(saveSoftKey, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
}

AddCalendarPage::~AddCalendarPage()
{

}

void AddCalendarPage::cancelClicked()
{
    emit showPreviousPage();
}

void AddCalendarPage::saveClicked()
{
#if defined(Q_WS_MAEMO_5)
    m_collection.setMetaData("Name", m_nameEdit->text());
#endif

    m_manager->saveCollection(&m_collection);
    if (m_manager->error())
        QMessageBox::warning(this, "Failed!", QString("Failed to save calendar!\n(error code %1)").arg(m_manager->error()));
    else
        emit showPreviousPage();
}

void AddCalendarPage::calendarChanged(QOrganizerItemManager *manager, QOrganizerCollection& calendar)
{
    m_manager = manager;
    m_collection = calendar;
    window()->setWindowTitle(!calendar.id().localId().isNull() ? "Edit calendar" : "Add calendar");

#if defined(Q_WS_MAEMO_5)
    // Maemo5 calendar features
    QVariant calendarName = calendar.metaData("Name");
    if (calendarName.canConvert(QVariant::String))
        m_nameEdit->setText(calendarName.toString());

    QVariant colorList = calendar.metaData("Available colors");
    QStringList availableColors;
    if (colorList.canConvert(QVariant::StringList))
        availableColors = colorList.toStringList();

    int index = 0;
    foreach(QString color, availableColors) {
        m_colorComboBox->addItem(color);
        if (calendar.metaData("Color").toString() == color)
            m_colorComboBox->setCurrentIndex(index);
        ++index;
    }

    QVariant typeList = calendar.metaData("Available types");
    QStringList availableTypes;
    if (typeList.canConvert(QVariant::StringList))
        availableTypes = typeList.toStringList();

    index = 0;
    foreach(QString type, availableTypes) {
        m_typeComboBox->addItem(type);
        if (calendar.metaData("Type").toString() == type)
            m_typeComboBox->setCurrentIndex(index);
        ++index;
    }

    QVariant visible = calendar.metaData("Visible");
    if (visible.canConvert(QVariant::Bool))
        m_visibleCheckBox->setCheckState(visible.toBool() ? Qt::Checked : Qt::Unchecked);

    QVariant readonly = calendar.metaData("Readonly");
    if (readonly.canConvert(QVariant::Bool))
        m_readonlyCheckBox->setCheckState(readonly.toBool() ? Qt::Checked : Qt::Unchecked);
#endif
}

#if defined(Q_WS_MAEMO_5)
void AddCalendarPage::colorChanged(const QString& color)
{
    m_collection.setMetaData("Color", color);
}

void AddCalendarPage::typeChanged(const QString& type)
{
    m_collection.setMetaData("Type", type);
}

void AddCalendarPage::visibilityChanged(int visibility)
{
    m_collection.setMetaData("Visible", (visibility == Qt::Checked));
}

void AddCalendarPage::readonlyChanged(int readonly)
{
    m_collection.setMetaData("Readonly", (readonly == Qt::Checked));
}

#endif
