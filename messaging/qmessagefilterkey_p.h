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
#include "qmessagefilterkey.h"
#if defined(Q_OS_WIN)
#include "qmessagestore.h"
#include "winhelpers_p.h"
#include <qvariant.h>
#endif

class QMessageFilterKeyPrivate
{
    Q_DECLARE_PUBLIC(QMessageFilterKey)

public:
    QMessageFilterKeyPrivate(QMessageFilterKey *messageFilterKey);

    QMessageFilterKey *q_ptr;
    QMessageDataComparator::Options _options;
#if defined(Q_OS_WIN)
    enum Field { None = 0, Id, Type, Sender, Recipients, Subject, TimeStamp, ReceptionTimeStamp, Status, Priority, Size, CustomField, ParentAccountId, ParentFolderId, AncestorFolderIds };
    enum Comparator { Equality = 0, Relation, Inclusion };
    enum Operator { Identity = 0, And, Or, Not, Nand, Nor, OperatorEnd };
    QMessageFilterKeyPrivate::Field _field;
    QVariant _value;
    Comparator _comparatorType;
    int _comparatorValue;
    Operator _operator;
    QMessageFilterKey *_left;
    QMessageFilterKey *_right;

    static void filterTable(QMessageStore::ErrorCode *lastError, const QMessageFilterKey &key, LPMAPITABLE);
    static QMessageFilterKey from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp);
    static QMessageFilterKey from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp);
#endif
};
