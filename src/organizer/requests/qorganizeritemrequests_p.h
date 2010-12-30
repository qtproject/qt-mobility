/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemSaveRequest(\n"
                      << "* items=" << m_organizeritems << ",\n"
                      << "* definitionMask=" << m_definitionMask << ",\n"
                      << "* errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QList<QOrganizerItem> m_organizeritems;
    QMap<int, QOrganizerManager::Error> m_errors;
    QStringList m_definitionMask;
};

class QOrganizerItemFetchRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemFetchRequestPrivate()
        : QOrganizerAbstractRequestPrivate(),
        m_maxCount(-1)
    {
    }

    ~QOrganizerItemFetchRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemFetchRequest;
    }

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemFetchRequest(\n"
                      << "* items=" << m_organizeritems << ",\n"
                      << "* filter=" << m_filter << ",\n"
                      << "* sorting=" << m_sorting << ",\n"
                      << "* startDate=" << m_startDate << ",\n"
                      << "* endDate=" << m_endDate << ",\n"
                      << "* fetchHint=" << m_fetchHint << ",\n"
                      << "* maxCount=" << m_maxCount;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;

    QDateTime m_startDate;
    QDateTime m_endDate;

    int m_maxCount;
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemFetchForExportRequest(\n"
                      << "* items=" << m_organizeritems << ",\n"
                      << "* filter=" << m_filter << ",\n"
                      << "* sorting=" << m_sorting << ",\n"
                      << "* startDate=" << m_startDate << ",\n"
                      << "* endDate=" << m_endDate << ",\n"
                      << "* fetchHint=" << m_fetchHint;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;

    QDateTime m_startDate;
    QDateTime m_endDate;
};

class QOrganizerItemFetchByIdRequestPrivate : public QOrganizerAbstractRequestPrivate
{
public:
    QOrganizerItemFetchByIdRequestPrivate()
        : QOrganizerAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemFetchByIdRequestPrivate()
    {
    }

    QOrganizerAbstractRequest::RequestType type() const
    {
        return QOrganizerAbstractRequest::ItemFetchByIdRequest;
    }

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemFetchByIdRequest(\n"
                      << "* items=" << m_items << ",\n"
                      << "* ids=" << m_ids << ",\n"
                      << "* fetchHint=" << m_fetchHint << ",\n"
                      << "* errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QList<QOrganizerItemId> m_ids;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_items;
    QMap<int, QOrganizerManager::Error> m_errors;
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemOccurrenceFetchRequest(\n"
                      << "* itemOccurrences=" << m_organizeritems << ",\n"
                      << "* parentItem=" << m_generator << ",\n"
                      << "* startDate=" << m_startDate << ",\n"
                      << "* endDate=" << m_endDate << ",\n"
                      << "* fetchHint=" << m_fetchHint << ",\n"
                      << "* maxOccurrences=" << m_maxOccurrences;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemRemoveRequest("
                      << "itemIds=" << m_organizeritemIds << ","
                      << "errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemIdFetchRequest(\n"
                      << "* itemIds=" << m_ids << ",\n"
                      << "* filter=" << m_filter << ",\n"
                      << "* sorting=" << m_sorting << ",\n"
                      << "* startDate=" << m_startDate << ",\n"
                      << "* endDate=" << m_endDate;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemDetailDefinitionFetchRequest(\n"
                      << "* definitionNames=" << m_names << ",\n"
                      << "* definitions=" << m_definitions << ",\n"
                      << "* itemType=" << m_organizeritemType << ",\n"
                      << "* errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QString m_organizeritemType;
    QStringList m_names;
    QMap<QString, QOrganizerItemDetailDefinition> m_definitions;
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemDetailDefinitionSaveRequest(\n"
                      << "* definitions=" << m_definitions << ",\n"
                      << "* itemType=" << m_organizeritemType << ",\n"
                      << "* errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif
    QString m_organizeritemType;
    QList<QOrganizerItemDetailDefinition> m_definitions;
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerItemDetailDefinitionRemoveRequest(\n"
                      << "* definitionNames=" << m_names << ",\n"
                      << "* itemType=" << m_organizeritemType << ",\n"
                      << "* errorMap=" << m_errors;
        dbg.nospace() << "\n)";
        return dbg.maybeSpace();
    }
#endif

    QString m_organizeritemType;
    QStringList m_names;
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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerCollectionFetchRequest("
                      << "collections=" << m_collections;
        dbg.nospace() << ")";
        return dbg.maybeSpace();
    }
#endif

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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerCollectionRemoveRequest("
                      << "collectionIds=" << m_collectionIds << ","
                      << "errorMap=" << m_errors;
        dbg.nospace() << ")";
        return dbg.maybeSpace();
    }
#endif

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

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        dbg.nospace() << "QOrganizerCollectionSaveRequest("
                      << "collections=" << m_collections << ","
                      << "errorMap=" << m_errors;
        dbg.nospace() << ")";
        return dbg.maybeSpace();
    }
#endif

    QList<QOrganizerCollection> m_collections;
    QMap<int, QOrganizerManager::Error> m_errors;
};



QTM_END_NAMESPACE

#endif
