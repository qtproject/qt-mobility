/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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


#include "qorganizercollection.h"
#include "qorganizercollection_p.h"

QTM_BEGIN_NAMESPACE

QOrganizerCollection::QOrganizerCollection()
    : d(new QOrganizerCollectionData)
{
}

QOrganizerCollection::~QOrganizerCollection()
{
}

QOrganizerCollection::QOrganizerCollection(const QOrganizerCollection& other)
    : d(other.d)
{
}

QOrganizerCollection& QOrganizerCollection::operator=(const QOrganizerCollection& other)
{
    this->d = other.d;
    return *this;
}

/*!
  Returns true if the id of the collection is the same as that of the \a other collection.
  Does not check that the metadata of the collections is equal.
 */
bool QOrganizerCollection::operator==(const QOrganizerCollection &other) const
{
    return d->m_id == other.d->m_id;
}

QOrganizerCollectionId QOrganizerCollection::id() const
{
    return d->m_id;
}

void QOrganizerCollection::setId(const QOrganizerCollectionId& id)
{
    d->m_id = id;
}

void QOrganizerCollection::setMetaData(const QVariantMap& metaData)
{
    d->m_metaData = metaData;
}

QVariantMap QOrganizerCollection::metaData() const
{
    return d->m_metaData;
}

void QOrganizerCollection::setMetaData(const QString& key, const QVariant& value)
{
    d->m_metaData.insert(key, value);
}

QVariant QOrganizerCollection::metaData(const QString& key)
{
    return d->m_metaData.value(key);
}

QTM_END_NAMESPACE
