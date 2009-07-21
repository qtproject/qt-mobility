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

#include "qcontactactionfilter.h"
#include "qcontactactionfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactActionFilter);

QContactActionFilter::QContactActionFilter()
    : QContactFilter(new QContactActionFilterPrivate)
{
}

void QContactActionFilter::setActionName(const QString& action)
{
    Q_D(QContactActionFilter);
    d->m_action = action;
}
void QContactActionFilter::setValue(const QVariant& value)
{
    Q_D(QContactActionFilter);
    d->m_value = value;
}

void QContactActionFilter::setVendor(const QString& vendorName, int version)
{
    Q_D(QContactActionFilter);
    d->m_vendorName = vendorName;
    if (!vendorName.isEmpty()) {
        d->m_vendorVersion = version;
        return;
    }

    d->m_vendorVersion = -1;
}

QString QContactActionFilter::actionName() const
{
    Q_D(const QContactActionFilter);
    return d->m_action;
}

QVariant QContactActionFilter::value() const
{
    Q_D(const QContactActionFilter);
    return d->m_value;
}

QString QContactActionFilter::vendorName() const
{
    Q_D(const QContactActionFilter);
    return d->m_vendorName;
}

int QContactActionFilter::vendorVersion() const
{
    Q_D(const QContactActionFilter);
    return d->m_vendorVersion;
}