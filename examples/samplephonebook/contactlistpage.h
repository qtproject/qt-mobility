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

    QContactFilter m_currentFilter;
    QMap<QString, QString> m_availableManagers;
};

#endif // CONTACTLISTPAGE_H
