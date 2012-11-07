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
    return m_cursorPosition >= 0 && m_cursorPosition < m_itemArray.Count();
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
        const QUrl url = QUrl::fromLocalFile(
                QDocumentGalleryMDSUtility::s60DescToQString(m_itemArray[m_cursorPosition]->Uri()));
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
    TRAPD( err,
        CMdEObject* object = m_session->OpenObjectL(m_itemArray[m_cursorPosition]->Id(),
                                                    m_itemArray[m_cursorPosition]->Def());
        if (!object)
            User::Leave(KErrNotFound);
        CleanupStack::PushL(object);
        QDocumentGalleryMDSUtility::SetMetaDataFieldL(object, value, key);
        m_session->CommitObjectL(*object);
        CleanupStack::PopAndDestroy(object);
        );

    return err == KErrNone ? true: false;
}

int QMDEGalleryResultSet::currentIndex() const
{
    return m_cursorPosition;
}

bool QMDEGalleryResultSet::fetch(int index)
{
    const bool isValid = index >= 0 && index < m_itemArray.Count();

    if (index != m_cursorPosition) {
        const bool wasValid = m_isValid;
        m_isValid = isValid;

        m_cursorPosition = index;

        if (isValid || wasValid)
            emit currentItemChanged();

        emit currentIndexChanged(m_cursorPosition);
    }
    return isValid;
}

#include "moc_qmdegalleryresultset_p.cpp"
QTM_END_NAMESPACE
