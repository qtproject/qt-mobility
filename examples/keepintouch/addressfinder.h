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
#ifndef ADDRESSFINDER_H
#define ADDRESSFINDER_H

#include "qtmessaging.h"

#include <QMap>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QWidget>
#include <QMainWindow>

class QCheckBox;
class QComboBox;
class QListWidget;
class QPushButton;
class QTabWidget;

#if !defined(Q_OS_WIN) || !defined(_WIN32_WCE)
// Don't use a 'Search' Button in CE
#define USE_SEARCH_BUTTON
#endif

QTM_USE_NAMESPACE

class AddressFinder : public QMainWindow
{
    Q_OBJECT

public:
    AddressFinder(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~AddressFinder();

private slots:
    void includePeriodChanged(int);
    void excludePeriodEnabled(int);
    void addressSelected(const QString&);
    void searchMessages();
    void stateChanged(QMessageService::State a);
    void messagesFound(const QMessageIdList &ids);
    void continueSearch();
#ifndef USE_SEARCH_BUTTON
    void tabChanged(int index);
#endif
    void messageIndexChanged(int index);
    void showMessage();
    void forwardMessage();

private:
    void setupUi();
    void setSearchActionEnabled(bool val);

private:
    QTabWidget *tabWidget;
    QComboBox *includePeriod;
    QComboBox *excludePeriod;
    QCheckBox *excludeCheckBox;

    QAction *searchAction;
    QPushButton *searchButton;

    QListWidget *contactList;
    QComboBox *messageCombo;
    QPushButton *showButton;
    QPushButton *forwardButton;

    QMessageService service;

    QMessageFilter inclusionFilter;

    QMessageIdList inclusionMessages;
    QMessageIdList exclusionMessages;

    QSet<QString> excludedAddresses;
    
    QStringList addressList;
    QMap<QString, QList<QPair<QString, QMessageId> > > addressMessages;
};

#endif
