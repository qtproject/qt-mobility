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
#ifndef QMESSAGEACCOUNTSORTORDERPRIVATE_H
#define QMESSAGEACCOUNTSORTORDERPRIVATE_H
#include "qmessageaccountsortorder.h"
#include "qstring.h"
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include <qmessageaccount.h>
#endif

QTM_BEGIN_NAMESPACE

class QMessageAccountSortOrderPrivate
{
public:
    QMessageAccountSortOrderPrivate(QMessageAccountSortOrder *sortOrder);

    bool _empty;
    Qt::SortOrder _order;
    QMessageAccountSortOrder *q_ptr;
    static Qt::SortOrder order(const QMessageAccountSortOrder &sortOrder) { return sortOrder.d_ptr->_order; }
    
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    static bool lessThan(const QMessageAccountSortOrder &sortOrder, const QMessageAccount &account1, const QMessageAccount &account2);
#endif    
};

QTM_END_NAMESPACE
#endif
