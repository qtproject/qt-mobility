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

#ifndef CONTACTLISTPAGE_H
#define CONTACTLISTPAGE_H

#include "qtcontacts.h"

#include <QWidget>
#include <QList>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
class QListWidget;
class QPushButton;
class QComboBox;
class QLabel;
class QLineEdit;
class QMainWindow;
QT_END_NAMESPACE

QTM_USE_NAMESPACE

class ContactListPage : public QWidget
{
    Q_OBJECT

public:
    ContactListPage(QMainWindow *mainWindow = 0, QWidget *parent = 0);
    ~ContactListPage();

    void rebuildList(const QContactFilter& filter);

signals:
    void showEditorPage(QContactLocalId contactId);
    void showFilterPage(const QContactFilter& filter);
    void managerChanged(QContactManager *manager);
    void clearFilter();

private slots:
    void backendSelected();
    void addClicked();
    void editClicked();
    void filterClicked();
    void deleteClicked();
    void importClicked();
    void exportClicked();

private:
    // elements of the contact list "page"
    QComboBox *m_backendsCombo;
    QLabel *m_filterActiveLabel;

    QListWidget *m_contactsList;

    // The main window that the page can add actions to
    QMainWindow *m_mainWindow;

    // data
    QContactManager *m_manager;
    QMap<QContactLocalId, int> m_idToListIndex;
    QMap<QString, QContactManager*> m_initialisedManagers;
    QList<QContact> m_contacts;
    QContactFilter m_currentFilter;
    QMap<QString, QString> m_availableManagers;
};

#endif // CONTACTLISTPAGE_H
