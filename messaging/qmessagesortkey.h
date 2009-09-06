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
#ifndef QMESSAGESORTKEY_H
#define QMESSAGESORTKEY_H
#include <Qt>
#include <qmessageglobal.h>
#include <qmessage.h>

class QMessageSortKeyPrivate;

class Q_MESSAGING_EXPORT QMessageSortKey {
    Q_DECLARE_PRIVATE(QMessageSortKey)

public:
    QMessageSortKey();
    QMessageSortKey(const QMessageSortKey &other);
    virtual ~QMessageSortKey();

    bool isEmpty() const;
    bool isSupported() const;

    QMessageSortKey operator+(const QMessageSortKey &other) const;
    QMessageSortKey& operator+=(const QMessageSortKey &other);

    bool operator==(const QMessageSortKey &other) const;
    const QMessageSortKey& operator=(const QMessageSortKey &other);

    static QMessageSortKey byType(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey bySender(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey byRecipients(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey bySubject(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey byTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey byReceptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey byStatus(QMessage::Status flag, Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey byPriority(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey bySize(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QMessageSortKeyPrivate *d_ptr;
};
#endif
