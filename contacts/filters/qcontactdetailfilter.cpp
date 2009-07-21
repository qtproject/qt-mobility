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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetailfilter.h"
#include "qcontactdetailfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactDetailFilter);

QContactDetailFilter::QContactDetailFilter()
    : QContactFilter(new QContactDetailFilterPrivate)
{
}

void QContactDetailFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QContactDetailFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

void QContactDetailFilter::setValue(const QVariant& value)
{
    Q_D(QContactDetailFilter);
    d->m_exactValue = value;
}

void QContactDetailFilter::setMatchFlags(Qt::MatchFlags flags)
{
    Q_D(QContactDetailFilter);
    d->m_flags = flags;
}

Qt::MatchFlags QContactDetailFilter::matchFlags() const
{
    Q_D(const QContactDetailFilter);
    return d->m_flags;
}

QString QContactDetailFilter::detailDefinitionName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_defId;
}

QString QContactDetailFilter::detailFieldName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_fieldId;
}

QVariant QContactDetailFilter::value() const
{
    Q_D(const QContactDetailFilter);
    return d->m_exactValue;
}