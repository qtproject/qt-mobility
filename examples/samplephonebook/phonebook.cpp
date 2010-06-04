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

#include "phonebook.h"
#include "contactlistpage.h"
#include "contacteditor.h"
#include "filterpage.h"

#include <QtGui>

PhoneBook::PhoneBook(QWidget *parent)
        : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
   
    m_editorPage = new ContactEditor(centralWidget);
    connect(m_editorPage, SIGNAL(showListPage()), this, SLOT(activateList()));

    m_filterPage = new FilterPage(centralWidget);
    connect(m_filterPage, SIGNAL(showListPage(QContactFilter)), this, SLOT(activateList(QContactFilter)));

    m_listPage = new ContactListPage(this, centralWidget);
    connect(m_listPage, SIGNAL(showEditorPage(QContactLocalId)), this, SLOT(activateEditor(QContactLocalId)));
    connect(m_listPage, SIGNAL(showFilterPage(QContactFilter)), this, SLOT(activateFind()));
    connect(m_listPage, SIGNAL(managerChanged(QContactManager*)), this, SLOT(managerChanged(QContactManager*)));
    connect(m_listPage, SIGNAL(clearFilter()), m_filterPage, SLOT(clearFilter()));

    m_stackedWidget = new QStackedWidget(centralWidget);
    m_stackedWidget->addWidget(m_listPage);
    m_stackedWidget->addWidget(m_editorPage);
    m_stackedWidget->addWidget(m_filterPage);
    m_stackedWidget->setCurrentIndex(0);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_stackedWidget);
    centralWidget->setLayout(centralLayout);
    
    setCentralWidget(centralWidget);
}

PhoneBook::~PhoneBook()
{
}

void PhoneBook::activateEditor(QContactLocalId contactId)
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(false);
#endif
    m_editorPage->setCurrentContact(m_manager, contactId);
    m_stackedWidget->setCurrentIndex(1); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateList(const QContactFilter& filter)
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(true);
#endif
    m_currentFilter = filter;
    activateList(); // call base now.
}

void PhoneBook::activateList()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(true);
#endif
    m_listPage->rebuildList(m_currentFilter);
    m_stackedWidget->setCurrentIndex(0); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateFind()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(false);
#endif
    m_stackedWidget->setCurrentIndex(2); // list = 0, editor = 1, find = 2.
}

void PhoneBook::managerChanged(QContactManager *manager)
{
    m_manager = manager;
    m_editorPage->setCurrentContact(m_manager, 0); // must reset the manager of the editor.
}
