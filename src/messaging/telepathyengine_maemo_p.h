/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef TELEPATHYENGINE_MAEMO_P_H
#define TELEPATHYENGINE_MAEMO_P_H
#include <tpsession.h>
#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessage.h"

QTM_BEGIN_NAMESPACE

class QMessageService;

class TelepathyEngine : public QObject
{
     Q_OBJECT
public:
    static TelepathyEngine* instance();

    TelepathyEngine();
    ~TelepathyEngine();

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount(QMessage::Type type) const;


    bool sendMessage(QMessage &message);
public slots:
    void onMessageSent(const Tp::Message &,TpSessionAccount *);
    void onMessageQueued(TpSessionAccount *,bool);
private:

    void updateImAccounts() const;


private: //Data
    TpSession *tpSession;
    mutable QMessageAccountId defaultSmsAccountId;
    mutable QHash<QString, QMessageAccount> iAccounts;
    QEventLoop loop; // For making send message synchronous
    bool opBusy;
};

QTM_END_NAMESPACE

#endif // TELEPATHYENGINE_MAEMO_P_H
