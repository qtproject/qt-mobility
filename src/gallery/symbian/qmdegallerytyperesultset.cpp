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

//Backend
#include "qmdegallerytyperesultset_p.h"
#include "qgallerymdsutility_p.h"
#include "qmdesession_p.h"
//API
#include "qgallerytyperequest.h"

QTM_BEGIN_NAMESPACE

QMDEGalleryTypeResultSet::QMDEGalleryTypeResultSet(QMdeSession *session, QObject *parent)
: QMDEGalleryResultSet(parent)
{
    m_request = static_cast<QGalleryTypeRequest *>(parent);
    m_session = session;

    createQuery();
}

QMDEGalleryTypeResultSet::~QMDEGalleryTypeResultSet()
{

}

void QMDEGalleryTypeResultSet::createQuery()
{
    QDocumentGalleryMDSUtility::GetDataFieldsForItemType( m_propertyList, m_request->itemType() );
    finish(false);
}

int QMDEGalleryTypeResultSet::itemCount() const
{
    return m_propertyList.count();
}

QVariant QMDEGalleryTypeResultSet::itemId() const
{
    int unvalidId = -1;
    QVariant id( unvalidId );
    return id;
}

QUrl QMDEGalleryTypeResultSet::itemUrl() const
{
    QUrl url;
    url.clear();
    return url;
}

QVariant QMDEGalleryTypeResultSet::metaData(int key) const
{
    QVariant tempNull;
    tempNull.clear();
    return tempNull;
}

bool QMDEGalleryTypeResultSet::setMetaData(int key, const QVariant &value)
{
    return false;
}

bool QMDEGalleryTypeResultSet::fetch(int index)
{
    if (m_propertyList.count() <= 0 || index < 0 || index > m_propertyList.count()) {
        return false;
    } else {
        m_cursorPosition = index;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryTypeResultSet::fetchNext()
{
    int newIndex = m_cursorPosition + 1;
    if (m_propertyList.count() <= 0 || newIndex < 0 || newIndex > m_propertyList.count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryTypeResultSet::fetchPrevious()
{
    int newIndex = m_cursorPosition - 1;
    if (m_propertyList.count() <= 0 || newIndex < 0 || newIndex > m_propertyList.count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryTypeResultSet::fetchFirst()
{
    int newIndex = 0; // first item
    if (m_propertyList.count() <= 0) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

bool QMDEGalleryTypeResultSet::fetchLast()
{
    int newIndex = m_itemArray.Count() - 1; // last item
    if (m_propertyList.count() <= 0 || newIndex < 0 || newIndex > m_propertyList.count()) {
        return false;
    } else {
        m_cursorPosition = newIndex;
        m_isValid = true;
        return true;
    }
}

#include "moc_qmdegallerytyperesultset_p.cpp"
QTM_END_NAMESPACE
