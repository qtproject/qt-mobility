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

#ifndef MODESTENGINE_MAEMO_H
#define MODESTENGINE_MAEMO_H

#include <QMap>
#include <QString>
#include <QDBusArgument>

#include "qmessagemanager.h"
#include "qmessagefilter_p.h"
#include "gconf/gconf-client.h"
#include "libosso.h"
#include <QDBusPendingCallWatcher>
#include <QFileInfoList>

class QDBusInterface;

QTM_BEGIN_NAMESPACE

typedef QMap< QString, QString > ModestStringMap;
typedef QList< ModestStringMap > ModestStringMapList;

class QMessageService;

class ModestEngine : public QObject
{
    Q_OBJECT

public:
    enum EmailProtocol
    {
        EmailProtocolUnknown = -1,
        EmailProtocolPop3 = 1,
        EmailProtocolIMAP,
    };

    static ModestEngine* instance();

    ModestEngine();
    ~ModestEngine();

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount() const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder,
                                      uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countFolders(const QMessageFolderFilter &filter) const;
    QMessageFolder folder(const QMessageFolderId &id) const;

    QMessage message(const QMessageId &id) const;
    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool countMessages(QMessageService& messageService, const QMessageFilter &filter);

    bool sendEmail(QMessage &message);
    bool composeEmail(const QMessage &message);

private:
    QFileInfoList localFolders() const;
    void appendLocalSubFolders(QFileInfoList& fileInfoList, int startIndex) const;
    void appendIMAPSubFolders(QFileInfoList& fileInfoList, int startIndex) const;
    QFileInfoList accountFolders(QMessageAccountId& accountId) const;
    QString localRootFolder() const;
    QString accountRootFolder(QMessageAccountId& accountId) const;
    EmailProtocol accountEmailProtocol(QMessageAccountId& accountId) const;

    void updateEmailAccounts() const;

private slots:

    // Async D-BUS call ended
    void sendEmailCallEnded(QDBusPendingCallWatcher *watcher);

private: //Data
    GConfClient *m_gconfclient;
    QDBusInterface *m_ModestDBusInterface;

    mutable QHash<QString, QMessageAccount> iAccounts;
    mutable QMessageAccountId iDefaultEmailAccountId;
};

QTM_END_NAMESPACE

// Marshall the MyStructure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::ModestStringMap &map);

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::ModestStringMap &map);

Q_DECLARE_METATYPE (QtMobility::ModestStringMap);
Q_DECLARE_METATYPE (QtMobility::ModestStringMapList);

#endif // MODESTENGINE_MAEMO_H

