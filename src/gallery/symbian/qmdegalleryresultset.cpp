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

#include "qgalleryresource.h"
//Backend
#include "qmdegalleryresultset_p.h"
#include "qgallerymdsutility_p.h"
#include "qmdesession_p.h"
//Symbian
#include <mdeproperty.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>

QTM_BEGIN_NAMESPACE

QMDEGalleryResultSet::QMDEGalleryResultSet(QObject *parent)
    : QGalleryResultSet(parent)
    , m_cursorPosition(-1)
    , m_live(false)
    , m_isValid(false)

{
    m_itemArray.Reset();
}

QMDEGalleryResultSet::~QMDEGalleryResultSet()
{
    m_itemArray.ResetAndDestroy();
}

int QMDEGalleryResultSet::propertyKey(const QString &property) const
{
    return QDocumentGalleryMDSUtility::GetPropertyKey( property );
}

QGalleryProperty::Attributes QMDEGalleryResultSet::propertyAttributes(int key) const
{
    if (key == QDocumentGalleryMDSUtility::EUri || key == QDocumentGalleryMDSUtility::EFileName ||
        key == QDocumentGalleryMDSUtility::EFilePath)
        return (QGalleryProperty::CanRead | QGalleryProperty::CanSort | QGalleryProperty::CanFilter );
    else if (key >= QDocumentGalleryMDSUtility::EPropertyKeysLength)
        return QGalleryProperty::Attributes();
    else
        return (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter );
}

QVariant::Type QMDEGalleryResultSet::propertyType(int key) const
{
    return QDocumentGalleryMDSUtility::GetPropertyType( key );
}

int QMDEGalleryResultSet::itemCount() const
{
    return m_itemArray.Count();
}

bool QMDEGalleryResultSet::isValid() const
{
    // Index based check
    if ( itemCount() != 0 && itemCount() >= currentIndex() )
        return true;
    else
        return false;
}

QVariant QMDEGalleryResultSet::itemId() const
{
    uint idVar = 0;

    if (isValid())
        idVar = m_itemArray[m_cursorPosition]->Id();

    return QVariant(idVar);
}

QUrl QMDEGalleryResultSet::itemUrl() const
{
    if ( isValid()) {
        const QUrl url =
        QUrl(QDocumentGalleryMDSUtility::s60DescToQString( m_itemArray[m_cursorPosition]->Uri()));
        return url;
    } else {
        return QUrl();
    }

}

QString QMDEGalleryResultSet::itemType() const
{
    if (isValid()) {
        return QDocumentGalleryMDSUtility::GetItemTypeFromMDEObject( m_itemArray[m_cursorPosition] );
    } else {
        QString null;
        return null;
    }
}

QList<QGalleryResource> QMDEGalleryResultSet::resources() const
{
    QList<QGalleryResource> resources;

    if (m_itemArray[m_cursorPosition]) {
        const QUrl url = QUrl(
                QDocumentGalleryMDSUtility::s60DescToQString(m_itemArray[m_cursorPosition]->Uri()));

        QStringList propertyList;
        QDocumentGalleryMDSUtility::GetDataFieldsForItemType( propertyList, itemType() );

        const int count = propertyList.count();
        QMap<int, QVariant> attributes;

        for (int i = 0; i < count; i++) {
            int propertyKey = QDocumentGalleryMDSUtility::GetPropertyKey( propertyList[i] );
            QVariant value = metaData( propertyKey );
            if (!value.isNull())
                attributes.insert(propertyKey, value);
        }
        resources.append(QGalleryResource(url, attributes));
    }
    return resources;
}

QVariant QMDEGalleryResultSet::metaData(int key) const
{
    QVariant retval;

    if( !m_isValid ) {
        retval.clear();
        return retval;
    }

    CMdEObject *item = m_itemArray[m_cursorPosition];
    if (item) {
        TRAPD( err, QDocumentGalleryMDSUtility::GetMetaDataFieldL( item, retval, key ) );
        if (err) {
            retval.clear();
        }
    }

    return retval;
}

bool QMDEGalleryResultSet::setMetaData(int key, const QVariant &value)
{
    bool ret = false;

    TRAPD( err, QDocumentGalleryMDSUtility::SetMetaDataFieldL(
            m_itemArray[m_cursorPosition], value, key) );
    if (err) {
        return false;
    } else {
        if (ret) {
            TRAP( err, m_session->CommitObjectL( *m_itemArray[m_cursorPosition] ) );
            if (err) {
                return false;
            } else {
                return true;
            }
        }
    }
    return false;
}

int QMDEGalleryResultSet::currentIndex() const
{
    return m_cursorPosition;
}

bool QMDEGalleryResultSet::fetch(int index)
{
    if (m_itemArray.Count() <= 0 || index < 0 || index > m_itemArray.Count()) {
        return false;
    } else {
        m_cursorPosition = index;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryResultSet::fetchNext()
{
    int newIndex = m_cursorPosition + 1;
    if (m_itemArray.Count() <= 0 || newIndex < 0 || newIndex > m_itemArray.Count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryResultSet::fetchPrevious()
{
    int newIndex = m_cursorPosition - 1;
    if (m_itemArray.Count() <= 0 || newIndex < 0 || newIndex > m_itemArray.Count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryResultSet::fetchFirst()
{
    int newIndex = 0; // first item
    if (m_itemArray.Count() <= 0) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryResultSet::fetchLast()
{
    int newIndex = m_itemArray.Count() - 1; // last item
    if (m_itemArray.Count() <= 0 || newIndex < 0 || newIndex > m_itemArray.Count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

#include "moc_qmdegalleryresultset_p.cpp"
QTM_END_NAMESPACE
