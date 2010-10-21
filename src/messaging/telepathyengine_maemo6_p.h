/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef TELEPATHYENGINE_MAEMO_H
#define TELEPATHYENGINE_MAEMO_H

#include <TelepathyQt4/Types>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/AccountManager>

#include "tpsessionaccount_p.h"

#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessage.h"
#include "qmessageservice.h"

QTM_USE_NAMESPACE


#define CM_NAME_DEFAULT "ring"
#define RING_ACCOUNT "/org/freedesktop/Telepathy/Account/ring/tel/ring"

class  TelepathyEngine : public QObject
{
    Q_OBJECT
public:
    static TelepathyEngine* instance();

    TelepathyEngine();
    ~TelepathyEngine();

    //bool initialize();


    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted);
    int countAccounts(const QMessageAccountFilter &filter);
    QMessageAccount account(const QMessageAccountId &id);
    QMessageAccountId defaultAccount(QMessage::Type type);

    bool sendMessage(QMessage &message, QMessageService *service);
    QMessageManager::Error error();
    static QMessageManager::Error convertError(const Tp::PendingOperation *op);

signals:
    //void amReady(TpSession *);
    void accountReady(TpSessionAccount *);
    void channeReady(TpSessionAccount *);
    void messageReceived(const Tp::ReceivedMessage &, TpSessionAccount *);

private slots:
    void onAMReady(Tp::PendingOperation *);
    void onAccountCreated(const QString &);
    void onReady(Tp::PendingOperation *);
    void onAccountReady(TpSessionAccount *tpacc);
    void onMessageReceived(const Tp::ReceivedMessage &, TpSessionAccount *);

private:
    //static TelepathyEngine *m_inst;

    void _updateImAccounts() const;
    TpSessionAccount *getTpSessionAccount(const QString &cm, const QString &protocol = QString());

    mutable QMessageAccountId m_defaultSmsAccountId;
    mutable QHash<QString, QMessageAccount> m_iAccounts;
    QEventLoop m_loop;

    Tp::AccountManagerPtr m_AM;
    QVector<TpSessionAccount*> m_accounts;
    QString m_CMName;
    QString m_reqMsg;
    QString m_reqAddress;
    bool m_sync;  // Synchronous initialization
    QMessageManager::Error m_error;
};

class SendRequest : public QObject
{
    Q_OBJECT
public:
    SendRequest(const QMessage &message, QMessageService *parent);
    ~SendRequest();

    QStringList to() const;
    QString text() const;	  

    void setFinished(const QString &address, bool success);

public slots:
    void finished(Tp::PendingOperation *operation, bool processLater = false);

private slots:
    friend class QTimer;
    void down();

private:
    QMessage _message;
    int _pendingRequestCount;
    int _failCount;

};

#endif // TELEPATHYENGINE_MAEMO_P_H
