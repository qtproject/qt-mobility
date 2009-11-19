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
#ifndef QMESSAGEORDERING_H
#define QMESSAGEORDERING_H
#include <Qt>
#include <qmessageglobal.h>
#include <qmessage.h>

class QMessageOrderingPrivate;

class Q_MESSAGING_EXPORT QMessageOrdering
{
    friend class QMessageOrderingPrivate;

public:
    QMessageOrdering();
    QMessageOrdering(const QMessageOrdering &other);
    virtual ~QMessageOrdering();

    QMessageOrdering& operator=(const QMessageOrdering &other);

    bool isEmpty() const;
    bool isSupported() const;

    QMessageOrdering operator+(const QMessageOrdering &other) const;
    QMessageOrdering& operator+=(const QMessageOrdering &other);

    bool operator==(const QMessageOrdering &other) const;
    bool operator!=(const QMessageOrdering &other) const;

    static QMessageOrdering byType(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering bySender(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering byRecipients(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering bySubject(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering byTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering byReceptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering byStatus(QMessage::Status flag, Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering byPriority(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageOrdering bySize(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QMessageOrderingPrivate *d_ptr;
};
#endif
