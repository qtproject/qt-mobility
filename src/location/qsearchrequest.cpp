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

#include "qmobilityglobal.h"
#include "qsearchrequest.h"
#include "qsearchrequest_p.h"

QTM_BEGIN_NAMESPACE

QSearchRequest::QSearchRequest()
    : d_ptr(new QSearchRequestPrivate)
{
}

QSearchRequest::~QSearchRequest()
{
    Q_D(QSearchRequest);
    delete d;
}

QLocationFilter QSearchRequest::locationFilter()
{
    Q_D(QSearchRequest);
    return d->m_locationFilter;
}

QString QSearchRequest::language() const
{
    Q_D(const QSearchRequest);
    return d->m_language;
}

void QSearchRequest::setLanguage(const QString& language)
{
    Q_D(QSearchRequest);
    d->m_language = language;
}

QString QSearchRequest::searchText() const
{
    Q_D(const QSearchRequest);
    return d->m_searchText;
}

void QSearchRequest::setSearchText(const QString& search)
{
    Q_D(QSearchRequest);
    d->m_searchText = search;
}

QString QSearchRequest::additionalData(const QString& key) const
{
    Q_D(const QSearchRequest);
    return d->m_additionalData[key];
}

void QSearchRequest::setAdditionalData(const QString& key, const QString& data)
{
    Q_D(QSearchRequest);
    d->m_additionalData.insert(key, data);
}

void QSearchRequest::removeAdditionalData(const QString& key)
{
    Q_D(QSearchRequest);
    d->m_additionalData.remove(key);
}

QSearchRequest::ReverseGeocodeMode QSearchRequest::reverseGeocodeMode() const
{
    Q_D(const QSearchRequest);
    return d->m_mode;
}

void QSearchRequest::setReverseGeocodeMode(ReverseGeocodeMode mode)
{
    Q_D(QSearchRequest);
    d->m_mode = mode;
}


QTM_END_NAMESPACE
