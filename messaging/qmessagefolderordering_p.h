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
#ifndef QMESSAGEFOLDERORDERINGPRIVATE_H
#define QMESSAGEFOLDERORDERINGPRIVATE_H
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderordering.h"
#include "qmessagefolder.h"
#include "qpair.h"

class QMessageFolderOrderingPrivate
{
    Q_DECLARE_PUBLIC(QMessageFolderOrdering)

public:
    enum Field { Name = 0, Path };

    QMessageFolderOrderingPrivate(QMessageFolderOrdering *ordering)
        :q_ptr(ordering)
    {
    }

    QMessageFolderOrdering *q_ptr;
    QList<QPair<Field, Qt::SortOrder> > _fieldOrderList;
    
#if defined(Q_OS_WIN)
    static bool lessthan(const QMessageFolderOrdering &ordering, const QMessageFolder &left, const QMessageFolder &right);
#endif
};
#endif
#endif
