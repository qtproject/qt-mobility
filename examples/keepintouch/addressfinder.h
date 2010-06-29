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
#ifndef ADDRESSFINDER_H
#define ADDRESSFINDER_H

#include "qmessageservice.h"

#include <QMap>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QWidget>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QListWidget;
class QPushButton;
class QTabWidget;
QT_END_NAMESPACE

#if !defined(Q_OS_WIN) || !defined(_WIN32_WCE)
// Don't use a 'Search' Button in CE
#define USE_SEARCH_BUTTON
#endif

#ifdef Q_WS_MAEMO_5
#define USE_CONTACTS_COMBOBOX
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

#ifdef USE_CONTACTS_COMBOBOX
    QComboBox *contactList;
#else
    QListWidget *contactList;
#endif
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
