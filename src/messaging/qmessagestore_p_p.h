/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#ifndef QMESSAGESTOREPRIVATE_H
#define QMESSAGESTOREPRIVATE_H
#include "qmessagestore_p.h"
#ifdef Q_OS_WIN
QT_BEGIN_NAMESPACE
class QMutex;
QT_END_NAMESPACE
#endif

QTM_BEGIN_NAMESPACE

class QMessageStorePrivatePlatform;

class QMessageStorePrivate
{
    Q_DECLARE_PUBLIC(QMessageStore)

public:
    QMessageStorePrivate();
    ~QMessageStorePrivate();

    void initialize(QMessageStore *store);

    QMessageStore *q_ptr;
    QMessageStorePrivatePlatform *p_ptr;

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    enum NotificationType
    {
        Added,
        Updated,
        Removed
    };
    void messageNotification(QMessageStorePrivate::NotificationType type, const QMessageId& id,
                             const QMessageManager::NotificationFilterIdSet &matchingFilters);
#endif
#ifdef Q_WS_MAEMO_6
    mutable QMessageManager::Error error;
#endif
#ifdef Q_OS_WIN
    static QMutex* mutex(QMessageStore*);
    static QMutex* mutex(QMessageManager&);
#endif

};

QTM_END_NAMESPACE
#endif
