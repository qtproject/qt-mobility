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
#include "qmdegallerycategoryresultset_p.h"
#include "qgallerymdsutility_p.h"
#include "qmdesession_p.h"
//API
#include "qgallerytyperequest.h"

QTM_BEGIN_NAMESPACE


QMDEGalleryTypeResultSet::QMDEGalleryTypeResultSet(QMdeSession *session, QGalleryTypeRequest *request)
    : m_itemType(request->itemType())
    , m_count(0)
    , m_itemCount(0)
    , m_currentIndex(0)
{
    if (QMDEGalleryCategoryResultSet::isCategoryType(m_itemType)) {
        int error = QMDEGalleryCategoryResultSet::createTypeQuery(
                &m_query, session, m_itemType, this);

        if (error != QDocumentGallery::NoError) {
            QGalleryResultSet::error(error);
        } else {
            m_query->SetResultMode(EQueryResultModeDistinctValues);

            TRAPD(err, m_query->FindL());
            if (err != KErrNone)
                QGalleryResultSet::error(QDocumentGallery::ConnectionError);
        }
    } else {
        TRAPD(err,
            CMdENamespaceDef &namespaceDef = session->GetDefaultNamespaceDefL();
            CMdEObjectDef &objectDef = QDocumentGalleryMDSUtility::ObjDefFromItemTypeL(
                    namespaceDef, m_itemType);

            m_query.reset(session->NewObjectQuery(namespaceDef, objectDef, this));
        );
        if (err != KErrNone)
            QGalleryResultSet::error(QDocumentGallery::ItemTypeError);

        m_query->SetResultMode(EQueryResultModeCount);

        TRAP(err, m_query->FindL());
        if (err != KErrNone)
            QGalleryResultSet::error(QDocumentGallery::ConnectionError);
    }
}

QMDEGalleryTypeResultSet::~QMDEGalleryTypeResultSet()
{
    if (m_query) {
        m_query->RemoveObserver(*this);
        m_query->Cancel();
    }
}

int QMDEGalleryTypeResultSet::propertyKey(const QString &property) const
{
    return property == QLatin1String("count") ? 0 : -1;
}

QGalleryProperty::Attributes QMDEGalleryTypeResultSet::propertyAttributes(int key) const
{
    return key == 0
            ? QGalleryProperty::CanRead
            : QGalleryProperty::Attributes();
}

QVariant::Type QMDEGalleryTypeResultSet::propertyType(int key) const
{
    return key == 0
            ? QVariant::Int
            : QVariant::Invalid;
}

int QMDEGalleryTypeResultSet::itemCount() const
{
    return m_itemCount;
}

bool QMDEGalleryTypeResultSet::isValid() const
{
    return m_currentIndex == 0 && m_itemCount == 1;
}

QVariant QMDEGalleryTypeResultSet::itemId() const
{
    return QVariant();
}

QUrl QMDEGalleryTypeResultSet::itemUrl() const
{
    return QUrl();
}

QString QMDEGalleryTypeResultSet::itemType() const
{
    return QString();
}

QVariant QMDEGalleryTypeResultSet::metaData(int key) const
{
    return m_currentIndex == 0 && m_itemCount == 1 && key == 0
            ? QVariant(m_count)
            : QVariant();
}

bool QMDEGalleryTypeResultSet::setMetaData(int, const QVariant &)
{
    return false;
}

int QMDEGalleryTypeResultSet::currentIndex() const
{
    return m_currentIndex;
}

bool QMDEGalleryTypeResultSet::fetch(int index)
{
    const bool isValid = index == 0 && m_itemCount == 1;

    if (index != m_currentIndex) {
        const bool wasValid = m_currentIndex == 0 && m_itemCount == 1;

        m_currentIndex = index;

        if (isValid || wasValid)
            emit currentItemChanged();

        emit currentIndexChanged(m_currentIndex);
    }
    return isValid;
}

void QMDEGalleryTypeResultSet::cancel()
{
    if (m_query)
        m_query->Cancel();
}

void QMDEGalleryTypeResultSet::HandleQueryNewResults(CMdEQuery &, TInt, TInt)
{
}

void QMDEGalleryTypeResultSet::HandleQueryCompleted(CMdEQuery &aQuery, TInt aError)
{
    switch (aError) {
    case KErrNone:
        {
            m_count = aQuery.Count();

            m_itemCount = 1;
            const int currentIndex = m_currentIndex;
            if (m_currentIndex >= 0)
                m_currentIndex += 1;

            emit itemsInserted(0, 1);

            if (currentIndex != m_currentIndex)
                emit currentIndexChanged(m_currentIndex);

            finish();
        }
        break;
    case KErrCancel:
        QGalleryResultSet::cancel();
        break;
    default:
        error(QDocumentGallery::ConnectionError);
        break;
    }
}

#include "moc_qmdegallerytyperesultset_p.cpp"

QTM_END_NAMESPACE
