/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECONTACTSORTORDER_P_H
#define QDECLARATIVECONTACTSORTORDER_P_H

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>

#include "qcontactsortorder.h"
#include "qdeclarativecontactdetail_p.h"

QTM_USE_NAMESPACE

class QDeclarativeContactSortOrder :public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeContactDetail::ContactDetailType detail READ detail WRITE setDetail NOTIFY sortOrderChanged)
    Q_PROPERTY(int field READ field WRITE setField NOTIFY sortOrderChanged)
    Q_PROPERTY(SortOrder direction READ direction WRITE setDirection NOTIFY sortOrderChanged)
    Q_PROPERTY(BlankPolicy blankPolicy READ blankPolicy WRITE setBlankPolicy NOTIFY sortOrderChanged)
    Q_PROPERTY(CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY sortOrderChanged)
    Q_ENUMS(BlankPolicy)
    Q_ENUMS(SortOrder)
    Q_ENUMS(CaseSensitivity)
public:

    enum BlankPolicy {
        BlanksFirst,
        BlanksLast
    };

    enum SortOrder {
        AscendingOrder,
        DescendingOrder
    };

    enum CaseSensitivity {
        CaseInsensitive,
        CaseSensitive
    };

    QDeclarativeContactSortOrder(QObject* parent = 0);

    void setDetail(QDeclarativeContactDetail::ContactDetailType detailType);

    QDeclarativeContactDetail::ContactDetailType detail() const;

    void setField(int fieldType);

    int field() const;

    BlankPolicy blankPolicy() const;
    void setBlankPolicy(BlankPolicy blankPolicy);
    SortOrder direction() const;
    void setDirection(SortOrder direction);
    CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(CaseSensitivity sensitivity);
    QContactSortOrder sortOrder();
    void setSortOrder(const QContactSortOrder& sortOrder);

signals:
    void sortOrderChanged();
private:
    QDeclarativeContactDetail::ContactDetailType m_detailType;
    int m_fieldType;
    QContactSortOrder m_sortOrder;
};

QML_DECLARE_TYPE(QDeclarativeContactSortOrder)

#endif
