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
#ifndef QMESSAGEFOLDERSORTORDERPRIVATE_H
#define QMESSAGEFOLDERSORTORDERPRIVATE_H
#include "qmessagefoldersortorder.h"
#include "qmessagefolder.h"
#include "qpair.h"

QTM_BEGIN_NAMESPACE

class QMessageFolderSortOrderPrivate
{
    Q_DECLARE_PUBLIC(QMessageFolderSortOrder)

public:
    enum Field { Name = 0, Path };

    QMessageFolderSortOrderPrivate(QMessageFolderSortOrder *sortOrder)
        :q_ptr(sortOrder)
    {
    }

    QMessageFolderSortOrder *q_ptr;
    QList<QPair<Field, Qt::SortOrder> > _fieldOrderList;
    
#if defined(Q_OS_WIN)
    static bool lessthan(const QMessageFolderSortOrder &sortOrder, const QMessageFolder &left, const QMessageFolder &right);
#endif
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    static bool lessThan(const QMessageFolderSortOrder &sortOrder, const QMessageFolder &folder1, const QMessageFolder &folder2);
#endif
#if defined(Q_WS_MAEMO_6)
    static QMessageFolderSortOrderPrivate *implementation(const QMessageFolderSortOrder &sortOrder);
#endif
};

QTM_END_NAMESPACE
#endif
