/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVECONTACTDETAILRANGEFILTER_H
#define QDECLARATIVECONTACTDETAILRANGEFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactdetailrangefilter.h"
#include "qdeclarativecontactdetail_p.h"
#include <QDeclarativeParserStatus>

class QDeclarativeContactDetailRangeFilter : public QDeclarativeContactFilter, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY valueChanged())
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY valueChanged())
    Q_PROPERTY(QVariant min READ minValue WRITE setMinValue NOTIFY valueChanged())
    Q_PROPERTY(QVariant max READ maxValue WRITE setMaxValue NOTIFY valueChanged())
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged())
    Q_PROPERTY(RangeFlags rangeFlags READ rangeFlags WRITE setRangeFlags NOTIFY valueChanged())
    Q_FLAGS(RangeFlags)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    enum RangeFlag {
        IncludeLower = QContactDetailRangeFilter::IncludeLower,
        IncludeUpper = QContactDetailRangeFilter::IncludeUpper,
        ExcludeLower = QContactDetailRangeFilter::ExcludeLower,
        ExcludeUpper = QContactDetailRangeFilter::ExcludeUpper
    };
    Q_DECLARE_FLAGS(RangeFlags, RangeFlag)

    QDeclarativeContactDetailRangeFilter(QObject* parent = 0)
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

    QDeclarativeContactDetailRangeFilter::RangeFlags rangeFlags() const
    {
        QDeclarativeContactDetailRangeFilter::RangeFlags flags;
        flags = ~flags & (int)d.rangeFlags();
        return flags;
    }
    void setRangeFlags(QDeclarativeContactDetailRangeFilter::RangeFlags flags)
    {
        QContactDetailRangeFilter::RangeFlags newFlags;
        newFlags = ~newFlags & (int)flags;
        if (newFlags != d.rangeFlags()) {
            d.setRange(d.minValue(), d.maxValue(), newFlags);
            emit valueChanged();
        }
    }

    QVariant minValue() const
    {
        return d.minValue();
    }
    void setMinValue(const QVariant& value)
    {
        if (value != d.minValue()) {
            d.setRange(value, d.maxValue(), d.rangeFlags());
            emit valueChanged();
        }
    }

    QVariant maxValue() const
    {
        return d.maxValue();
    }
    void setMaxValue(const QVariant& value)
    {
        if (value != d.maxValue()) {
            d.setRange(d.minValue(), value, d.rangeFlags());
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
    QContactDetailRangeFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactDetailRangeFilter)

#endif
