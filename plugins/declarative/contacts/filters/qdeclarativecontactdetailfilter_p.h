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

#ifndef QDECLARATIVECONTACTDETAILFILTER_H
#define QDECLARATIVECONTACTDETAILFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactdetailfilter.h"
#include "qdeclarativecontactdetail_p.h"
#include <QDeclarativeParserStatus>

class QDeclarativeContactDetailFilter : public QDeclarativeContactFilter, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY valueChanged())
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY valueChanged())
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged())
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged())
    Q_INTERFACES(QDeclarativeParserStatus)
public:

    QDeclarativeContactDetailFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent),
          m_componentCompleted(false)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }
    //from QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete()
    {
        setDetailDefinitionName();
        m_componentCompleted = true;
    }

    void setDetailDefinitionName()
    {
        QString ddn;
        if (m_detail.type() != QVariant::String) {
            ddn = QDeclarativeContactDetail::definitionName(static_cast<QDeclarativeContactDetail::ContactDetailType>(m_detail.toInt()));
        } else {
            ddn = m_detail.toString();
        }

        QString dfn;
        if (m_field.type() != QVariant::String) {
           QDeclarativeContactDetail::ContactDetailType dt = static_cast<QDeclarativeContactDetail::ContactDetailType>(QDeclarativeContactDetail::detailType(ddn));
           dfn = QDeclarativeContactDetail::fieldName(dt, m_field.toInt());
        } else {
            dfn = m_field.toString();
        }
        d.setDetailDefinitionName(ddn, dfn);
        m_detail = ddn;
        m_field = dfn;
    }

    void setDetail(const QVariant& detailType)
    {
        if (detailType != m_detail || m_componentCompleted) {
            m_detail = detailType;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }

    QVariant detail() const
    {
        return m_detail;
    }

    void setField(const QVariant& fieldType)
    {
        if (fieldType != m_field || m_componentCompleted) {
            m_field = fieldType;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }

    QVariant field() const
    {
        return m_field;
    }


    QDeclarativeContactFilter::MatchFlags matchFlags() const
    {
        QDeclarativeContactFilter::MatchFlags flags;
        flags = ~flags & (int)d.matchFlags();
        return flags;
    }
    void setMatchFlags(QDeclarativeContactFilter::MatchFlags flags)
    {
        QContactFilter::MatchFlags newFlags;
        newFlags = ~newFlags & (int)flags;
        if (newFlags != d.matchFlags()) {
            d.setMatchFlags(newFlags);
            emit valueChanged();
        }
    }

    QVariant value() const
    {
        return d.value();
    }
    void setValue(const QVariant& value)
    {
        if (value != d.value()) {
            d.setValue(value);
            emit valueChanged();
        }
    }

    QContactFilter filter() const
    {
        return d;
    }
signals:
    void valueChanged();


private:
    bool m_componentCompleted;
    QVariant m_field;
    QVariant m_detail;
    QContactDetailFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactDetailFilter)

#endif
