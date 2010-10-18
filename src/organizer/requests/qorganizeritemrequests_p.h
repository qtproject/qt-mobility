/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERITEMREQUESTS_P_H
#define QORGANIZERITEMREQUESTS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qorganizercollectionid.h"
#include "qorganizerabstractrequest_p.h"
#include "qorganizeritemfilter.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritem.h"
#include "qtorganizerglobal.h"

#include <QStringList>
#include <QList>

QTM_BEGIN_NAMESPACE

class QOrganizerItemSaveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemSaveRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemSaveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemSaveRequest;
    }

    QList<QOrganizerItem> m_organizeritems;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerItemFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemFetchRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;

    QDateTime m_startDate;
    QDateTime m_endDate;
};

class QOrganizerItemFetchForExportRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemFetchForExportRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemFetchForExportRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemFetchForExportRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;

    QDateTime m_startDate;
    QDateTime m_endDate;
};

class QOrganizerItemOccurrenceFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemOccurrenceFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate(),
          m_maxOccurrences(-1)
    {
    }

    ~QOrganizerItemOccurrenceFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemOccurrenceFetchRequest;
    }

    QOrganizerItem m_generator;
    QDateTime m_startDate;
    QDateTime m_endDate;
    int m_maxOccurrences;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;
};

class QOrganizerItemRemoveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemRemoveRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemRemoveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemRemoveRequest;
    }

    QList<QOrganizerItemId> m_organizeritemIds;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerItemIdFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemIdFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemIdFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemIdFetchRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;

    QList<QOrganizerItemId> m_ids;

    QDateTime m_startDate;
    QDateTime m_endDate;
};

class QOrganizerItemDetailDefinitionFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::DetailDefinitionFetchRequest;
    }

    QString m_organizeritemType;
    QStringList m_names;
    QMap<QString, QOrganizerItemDetailDefinition> m_definitions;
    QOrganizerCollectionId m_collectionId;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerItemDetailDefinitionSaveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionSaveRequestPrivate()
        : QOrganizerAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionSaveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::DetailDefinitionSaveRequest;
    }

    QString m_organizeritemType;
    QList<QOrganizerItemDetailDefinition> m_definitions;
    QOrganizerCollectionId m_collectionId;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerItemDetailDefinitionRemoveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionRemoveRequestPrivate()
        : QOrganizerAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionRemoveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::DetailDefinitionRemoveRequest;
    }

    QString m_organizeritemType;
    QStringList m_names;
    QOrganizerCollectionId m_collectionId;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerCollectionFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerCollectionFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerCollectionFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::CollectionFetchRequest;
    }

    QList<QOrganizerCollection> m_collections;
};

class QOrganizerCollectionRemoveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerCollectionRemoveRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerCollectionRemoveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::CollectionRemoveRequest;
    }

    QList<QOrganizerCollectionId> m_collectionIds;
    QMap<int, QOrganizerManager::Error> m_errors;
};

class QOrganizerCollectionSaveRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerCollectionSaveRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerCollectionSaveRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::CollectionSaveRequest;
    }

    QList<QOrganizerCollection> m_collections;
    QMap<int, QOrganizerManager::Error> m_errors;
};



QTM_END_NAMESPACE

#endif
