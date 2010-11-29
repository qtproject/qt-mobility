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

#ifndef QDECLARATIVEORGANIZERITEMSORTORDER_H
#define QDECLARATIVEORGANIZERITEMSORTORDER_H

#include "qdeclarative.h"

#include "qorganizeritemsortorder.h"
#include "qdeclarativeorganizeritemdetail_p.h"
#include <QDeclarativeParserStatus>

QTM_USE_NAMESPACE

class QDeclarativeOrganizerItemSortOrder : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY sortOrderChanged)
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY sortOrderChanged)
    Q_PROPERTY(BlankPolicy blankPolicy READ blankPolicy WRITE setBlankPolicy NOTIFY sortOrderChanged)
    Q_PROPERTY(Qt::SortOrder direction READ direction WRITE setDirection NOTIFY sortOrderChanged)
    Q_PROPERTY(Qt::CaseSensitivity sensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY sortOrderChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(BlankPolicy)
public:
    QDeclarativeOrganizerItemSortOrder(QObject* parent = 0)
        :QObject(parent)
    {}

    enum BlankPolicy {
        BlanksFirst = QOrganizerItemSortOrder::BlanksFirst,
        BlanksLast = QOrganizerItemSortOrder::BlanksLast,
    };

    //from QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete()
    {
         d = sortOrder();
    }

    void setDetail(const QVariant& v)
    {
        if (m_detail != v) {
            m_detail = v;
            emit sortOrderChanged();
        }

    }

    void setField(const QVariant& v)
    {
        if (m_field !=v) {
            m_field = v;
            emit sortOrderChanged();
        }
    }

    void setBlankPolicy(BlankPolicy policy)
    {
        if (policy != blankPolicy()) {
            d.setBlankPolicy(static_cast<QOrganizerItemSortOrder::BlankPolicy>(policy));
            emit sortOrderChanged();
        }
    }

    void setDirection(Qt::SortOrder newDirection)
    {
        if (newDirection != direction()) {
            d.setDirection(newDirection);
            emit sortOrderChanged();
        }
    }

    void setCaseSensitivity(Qt::CaseSensitivity newSensitivity)
    {
        if (newSensitivity != caseSensitivity()) {
            d.setCaseSensitivity(newSensitivity);
            emit sortOrderChanged();
        }
    }

    QVariant detail() const
    {
        return m_detail;
    }
    QVariant field() const
    {
        return m_field;
    }
    BlankPolicy blankPolicy() const
    {
        return static_cast<QDeclarativeOrganizerItemSortOrder::BlankPolicy>(d.blankPolicy());
    }
    Qt::SortOrder direction() const
    {
        return d.direction();
    }
    Qt::CaseSensitivity caseSensitivity() const
    {
        return d.caseSensitivity();
    }

    QOrganizerItemSortOrder sortOrder()
    {
        QString ddn;
        if (m_detail.type() != QVariant::String) {
            ddn = QDeclarativeOrganizerItemDetail::definitionName(static_cast<QDeclarativeOrganizerItemDetail::ItemDetailType>(m_detail.toInt()));
        } else {
            ddn = m_detail.toString();
        }

        QString dfn;
        if (m_field.type() != QVariant::String) {
           QDeclarativeOrganizerItemDetail::ItemDetailType dt = QDeclarativeOrganizerItemDetail::detailType(ddn);
           dfn = QDeclarativeOrganizerItemDetail::fieldName(dt, m_field.toInt());
        } else {
            dfn = m_field.toString();
        }

        d.setDetailDefinitionName(ddn, dfn);
        m_detail = ddn;
        m_field = dfn;
        return d;
    }

    void setSortOrder(const QOrganizerItemSortOrder& sortOrder)
    {
        d = sortOrder;
        m_field = d.detailFieldName();
        m_detail = d.detailDefinitionName();
        emit sortOrderChanged();
    }

signals:
    void sortOrderChanged();
private:
    QVariant m_field;
    QVariant m_detail;
    QOrganizerItemSortOrder d;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemSortOrder)


#endif
