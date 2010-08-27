/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef QMESSAGEFOLDERORDERING_H
#define QMESSAGEFOLDERORDERING_H
#include <Qt>
#include <qmessageglobal.h>

QTM_BEGIN_NAMESPACE

class QMessageFolderSortOrderPrivate;

class Q_MESSAGING_EXPORT QMessageFolderSortOrder
{
    friend class QMessageFolderSortOrderPrivate;

public:
    QMessageFolderSortOrder();
    QMessageFolderSortOrder(const QMessageFolderSortOrder &other);
    virtual ~QMessageFolderSortOrder();

    QMessageFolderSortOrder& operator=(const QMessageFolderSortOrder &other);

    bool isEmpty() const;
    bool isSupported() const;

    QMessageFolderSortOrder operator+(const QMessageFolderSortOrder &other) const;
    QMessageFolderSortOrder& operator+=(const QMessageFolderSortOrder &other);

    bool operator==(const QMessageFolderSortOrder &other) const;
    bool operator!=(const QMessageFolderSortOrder &other) const;

    static QMessageFolderSortOrder byName(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageFolderSortOrder byPath(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QMessageFolderSortOrderPrivate *d_ptr;
};

QTM_END_NAMESPACE
#endif
