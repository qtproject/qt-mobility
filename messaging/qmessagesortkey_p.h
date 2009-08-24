/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagesortkey.h"
#if defined(Q_OS_WIN)
#include "qmessagestore.h"
#include <qpair.h>
#include "winhelpers_p.h"
#endif

class QMessageSortKeyPrivate
{
    Q_DECLARE_PUBLIC(QMessageSortKey)

public:
    QMessageSortKeyPrivate(QMessageSortKey *messageSortKey);

    QMessageSortKey *q_ptr;
#if defined(Q_OS_WIN)
    enum Field { Type = 0, Sender, Recipients, Subject, TimeStamp, ReceptionTimeStamp, Read, HasAttachments, Incoming, Removed, Priority, Size };
    QList<QPair<Field, Qt::SortOrder> > _fieldOrderList;
    static bool compare(const QMessageSortKey &key, const QMessage &left, const QMessage &right);
    static void sortTable(QMessageStore::ErrorCode *lastError, const QMessageSortKey &key, LPMAPITABLE);
    static QMessageSortKey from(QMessageSortKeyPrivate::Field field, Qt::SortOrder order);
#endif
};
