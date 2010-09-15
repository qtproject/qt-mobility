/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef XQCENTRALREPOSITORYUTILS_H
#define XQCENTRALREPOSITORYUTILS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include "xqsettingskey.h"
#include "settingsmanager_global.h"

class XQSettingsManager;
class XQCentralRepositoryUtilsPrivate;
class XQCentralRepositorySearchCriteria;

class XQSETTINGSMANAGER_EXPORT XQCentralRepositoryUtils : public QObject
{
public:

    enum TransactionMode
    {
        ConcurrentReadWriteTransaction,
        ReadTransaction,
        ReadWriteTransaction
    };

    enum TransactionState
    {
        NotInTransaction,
        ActiveTransaction,
        FailedTransaction,
        UnknownTransaction
    };
    
    explicit XQCentralRepositoryUtils(XQSettingsManager& settingsManager, QObject* parent = 0);
    ~XQCentralRepositoryUtils();
    bool createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value);
    bool deleteKey(const XQCentralRepositorySettingsKey& key);
    bool resetKey(const XQCentralRepositorySettingsKey& key);
    bool resetRepository(long int repositoryUid);
    QList<XQCentralRepositorySettingsKey> findKeys(const XQCentralRepositorySearchCriteria& criteria);

    bool startTransaction(long int repositoryUid, TransactionMode transactionMode);
    bool commitTransaction(long int repositoryUid);
    void cancelTransaction(long int repositoryUid);
    void failTransaction(long int repositoryUid);
    TransactionState transactionState(long int repositoryUid) const;

private:
    XQCentralRepositoryUtilsPrivate* d;
};

#endif //XQCENTRALREPOSITORYUTILS_H
