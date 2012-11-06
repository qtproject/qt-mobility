/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERITEMSORTORDER_H
#define QORGANIZERITEMSORTORDER_H

#include "qtorganizerglobal.h"

#include <QString>
#include <QSharedData>
#include <QList>

class QDataStream;

QTM_BEGIN_NAMESPACE

class QOrganizerItemSortOrderPrivate;
class Q_ORGANIZER_EXPORT QOrganizerItemSortOrder
{
public:
    QOrganizerItemSortOrder();
    ~QOrganizerItemSortOrder();

    QOrganizerItemSortOrder(const QOrganizerItemSortOrder& other);
    QOrganizerItemSortOrder& operator=(const QOrganizerItemSortOrder& other);

    enum BlankPolicy {
        BlanksFirst,
        BlanksLast,
    };

    /* Mutators */
    void setDetailDefinitionName(const QString& definitionName, const QString& fieldName);
    void setBlankPolicy(BlankPolicy blankPolicy);
    void setDirection(Qt::SortOrder direction);
    void setCaseSensitivity(Qt::CaseSensitivity sensitivity);

    /* Accessors */
    QString detailDefinitionName() const;
    QString detailFieldName() const;
    BlankPolicy blankPolicy() const;
    Qt::SortOrder direction() const;
    Qt::CaseSensitivity caseSensitivity() const;

    bool isValid() const;

    bool operator==(const QOrganizerItemSortOrder& other) const;
    bool operator!=(const QOrganizerItemSortOrder& other) const {return !operator==(other);}

    /* Convenience cast */
    operator QList<QOrganizerItemSortOrder>() const {return QList<QOrganizerItemSortOrder>() << *this;}

private:
    QSharedDataPointer<QOrganizerItemSortOrderPrivate> d;
};

#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItemSortOrder& sortOrder);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerItemSortOrder& sortOrder);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItemSortOrder& sortOrder);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemSortOrder), Q_MOVABLE_TYPE);


#endif
