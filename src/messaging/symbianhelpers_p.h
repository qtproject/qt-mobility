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

#ifndef _SYMBIANHELPERS_H_
#define _SYMBIANHELPERS_H_

#include <QtCore>
#include "qmessage.h"

QTM_BEGIN_NAMESPACE

class QMessageId;
class QMessageAccountId;
class QMessageFolderId;

namespace SymbianHelpers {

    bool isFreestyleMessage(const QMessageId& msgId);
    bool isFreestyleMessage(const QString& msgId);
    bool isFreestyleAccount(const QMessageAccountId &accountId);
    bool isFreestyleFolder(const QMessageFolderId &folderId);
    QMessageId addFreestylePrefix(const QMessageId& msgId);
    QMessageAccountId addFreestylePrefix(const QMessageAccountId &accountId);
    QMessageFolderId addFreestylePrefix(const QMessageFolderId &folderId);
    QString addFreestylePrefix(const QString& msgId);
    QMessageId removeFreestylePrefix(const QMessageId& msgId);
    QMessageAccountId removeFreestylePrefix(const QMessageAccountId &accountId);
    QMessageFolderId removeFreestylePrefix(const QMessageFolderId &folderId);
    QString removeFreestylePrefix(const QString& msgId);

    QString addIdPrefix(const QString& id);
    QString stripIdPrefix(const QString& id);

    static const char* mtmPrefix = "MTM_";
    static const char* freestylePrefix = "FS_";
};

QTM_END_NAMESPACE

#endif
