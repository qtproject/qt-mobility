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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECONTACTDETAILFILTER_H
#define QDECLARATIVECONTACTDETAILFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactdetailfilter.h"
#include "qdeclarativecontactdetail_p.h"

class QDeclarativeContactDetailFilter : public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeContactDetail::ContactDetailType detail READ detail WRITE setDetail NOTIFY valueChanged)
    Q_PROPERTY(int field READ field WRITE setField NOTIFY valueChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged)
public:

    QDeclarativeContactDetailFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent),
          m_detailType(QDeclarativeContactDetail::Customized),
          m_fieldType(-1)
    {
    }
    void setDetail(QDeclarativeContactDetail::ContactDetailType detailType)
    {
        m_detailType = detailType;
    }

    QDeclarativeContactDetail::ContactDetailType detail() const
    {
        return m_detailType;
    }

    void setField(int fieldType)
    {
        m_fieldType = fieldType;
    }

    int field() const
    {
        return m_fieldType;
    }

    QDeclarativeContactFilter::MatchFlags matchFlags() const
    {
        QDeclarativeContactFilter::MatchFlags matchFlags;
        matchFlags &= 0xFFFFFFFF;
        matchFlags &= (int)d.matchFlags();
        return matchFlags;
    }
    void setMatchFlags(QDeclarativeContactFilter::MatchFlags flags)
    {
        QContactFilter::MatchFlags matchFlags;
        matchFlags &= 0xFFFFFFFF;
        matchFlags &= (int)flags;
        d.setMatchFlags(matchFlags);
    }

    QVariant value() const
    {
        return d.value();
    }
    void setValue(const QVariant& value)
    {
        d.setValue(value);
    }

    QContactFilter filter() const
    {
        QString detailName = QDeclarativeContactDetail::definitionName(m_detailType);
        QString fieldName = QDeclarativeContactDetail::fieldName(m_detailType, m_fieldType);
        QContactDetailFilter filter(d);
        filter.setDetailDefinitionName(detailName, fieldName);
        return filter;
    }
signals:
    void valueChanged();


private:
    QDeclarativeContactDetail::ContactDetailType m_detailType;
    int m_fieldType;
    QContactDetailFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactDetailFilter)

#endif
