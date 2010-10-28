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

#include "qmdegallerycategoryresultset_p.h"

#include"qgallerymdsutility_p.h"
#include "qmdesession_p.h"

#include "qdocumentgallery.h"

#include <mdeconstants.h>
#include <mdeobjectdef.h>

QTM_BEGIN_NAMESPACE

namespace
{
    template <typename T, int N> static int qt_arrayLength(const T(&)[N]) { return N; }

    template <typename T> struct QMdeArray
    {
        const T *values;
        int count;

        const T &operator [](int i) const { return values[i]; }
    };

    #define QT_MDE_ARRAY(array) { array, qt_arrayLength(array) }

#define QT_MDE_CONSTANT(Constant) reinterpret_cast<const TDesC *>(&MdeConstants::Constant)

    struct QMdeGalleryCategoryType
    {
        const char *name;
        const TDesC *mdeObject;
        const TDesC *mdeProperty;
        QMdeArray<const char *> properties;
    };

    static const char *qt_mdeArtistProperties[] = { "artist", "title" };
    static const char *qt_mdeAlbumArtistProperties[] = { "albumArtist", "artist", "title" };
    static const char *qt_mdeAlbumProperties[] = { "albumTitle", "title" };
    static const char *qt_mdeGenreProperties[] = { "genre", "title" };

    static QMdeGalleryCategoryType qt_mdeGalleryCategoryTypes[] =
    {
        {
            "Artist",
            QT_MDE_CONSTANT(Audio::KAudioObject),
            QT_MDE_CONSTANT(MediaObject::KArtistProperty),
            QT_MDE_ARRAY(qt_mdeArtistProperties)
        }, {
            "AlbumArtist",
            QT_MDE_CONSTANT(Audio::KAudioObject),
            QT_MDE_CONSTANT(Audio::KAlbumArtistProperty),
            QT_MDE_ARRAY(qt_mdeAlbumArtistProperties)
        }, {
            "Album",
            QT_MDE_CONSTANT(Audio::KAudioObject),
            QT_MDE_CONSTANT(Audio::KAlbumProperty),
            QT_MDE_ARRAY(qt_mdeAlbumProperties)
        }, {
            "Genre",
            QT_MDE_CONSTANT(Audio::KAudioObject),
            QT_MDE_CONSTANT(MediaObject::KGenreProperty),
            QT_MDE_ARRAY(qt_mdeGenreProperties)
        }
    };

    template <typename T, int N>
    static int qt_indexOf(const T &itemType, const QMdeGalleryCategoryType (&array)[N])
    {
        for (int i = 0; i < N; ++i) {
            if (QLatin1String(array[i].name) == itemType)
                return i;
        }
        return -1;
    }

    template <typename T>
    static bool qt_contains(const T &string, const QMdeGalleryCategoryType &type)
    {
        for (int i = 0; i < type.properties.count; ++i) {
            if (string == QLatin1String(type.properties[i]))
                return true;
        }
        return false;
    }


    static CMdEObjectDef *qt_getObjectDef(CMdENamespaceDef &namespaceDef, const TDesC &objectName)
    {
        CMdEObjectDef *objectDef = 0;
#if defined(MDS_25_COMPILATION_ENABLED)
        TRAPD(err, objectDef = &namespaceDef.GetObjectDefL(objectName));
#else
        TRAPD(err, objectDef = namespaceDef.GetObjectDefL(objectName));
#endif
        return err == KErrNone
                ? objectDef
                : 0;
    }

    static CMdEPropertyDef *qt_getPropertyDef(CMdEObjectDef &objectDef, const TDesC &propertyName)
    {
        CMdEPropertyDef *propertyDef = 0;
#if defined(MDS_25_COMPILATION_ENABLED)
        TRAPD(err, propertyDef = &objectDef.GetPropertyDefL(propertyName));
#else
        TRAPD(err, propertyDef = objectDef.GetPropertyDefL(propertyName));
#endif
        return err == KErrNone
                ? propertyDef
                : 0;
    }

    static CMdEPropertyDef *qt_getPropertyDef(
            CMdENamespaceDef &namespaceDef, const TDesC &objectName, const TDesC &propertyName)
    {
        CMdEPropertyDef *propertyDef = 0;
#if defined(MDS_25_COMPILATION_ENABLED)
        TRAPD(err, propertyDef
                = &namespaceDef.GetObjectDefL(objectName).GetPropertyDefL(propertyName));
#else
        CMdEObjectDef *objectDef = 0;
        TRAPD(err, objectDef = namespaceDef.GetObjectDefL(objectName));
        if (err == KErrNone && objectDef) {
            TRAP(err, propertyDef = objectDef->GetPropertyDefL(propertyName));
        }
#endif
        return err == KErrNone
                ? propertyDef
                : 0;
    }

    static CMdERelationDef *qt_getRelationDef(
            CMdENamespaceDef &namespaceDef, const TDesC &relationName)
    {
        CMdERelationDef *relationDef = 0;
#if defined(MDS_25_COMPILATION_ENABLED)
        TRAPD(err, relationDef = &namespaceDef.GetRelationDefL(relationName));
#else
        TRAPD(err, relationDef = namespaceDef.GetRelationDefL(relationName));
#endif
        return err == KErrNone
                ? relationDef
                : 0;
    }

    static int qt_appendScopeCondition(
            CMdELogicCondition *condition,
            const QMdeGalleryCategoryType &type,
            const QString &categoryId,
            CMdENamespaceDef &namespaceDef)
    {
        if (CMdEPropertyDef *propertyDef = qt_getPropertyDef(
                namespaceDef, *type.mdeObject, *type.mdeProperty)) {
            TRAPD(err, condition->AddPropertyConditionL(
                    *propertyDef,
                    ETextPropertyConditionCompareEquals,
                    TPtrC16(categoryId.utf16(), categoryId.length())));

            if (err == KErrNone)
                return QDocumentGallery::NoError;
        }
        return QDocumentGallery::ItemTypeError;
    }

    static int qt_appendScopeCondition(
            CMdELogicCondition *condition,
            uint itemId,
            CMdENamespaceDef &namespaceDef)
    {
        if (CMdERelationDef *relationDef = qt_getRelationDef(
                namespaceDef, MdeConstants::Relations::KContains)) {
            TRAPD(err, condition->AddRelationConditionL(*relationDef, ERelationConditionSideRight)
                    .LeftL().AddObjectConditionL(itemId));
            if (err == KErrNone)
                return QDocumentGallery::NoError;
        }
        return QDocumentGallery::ItemTypeError;
    }

    static int qt_appendCondition(
            CMdELogicCondition *condition,
            const QGalleryMetaDataFilter &filter,
            const QMdeGalleryCategoryType &type,
            const CMdEPropertyDef &propertyDef)
    {
        if (qt_contains(filter.propertyName(), type)) {
            int err = KErrNone;
            CMdEPropertyCondition *propertyCondition = 0;

            const QString value = filter.value().toString();
            TPtrC16 text(value.utf16(), value.length());

            switch (filter.comparator()) {
            case QGalleryFilter::Equals:
                TRAP(err, propertyCondition = &condition->AddPropertyConditionL(
                        propertyDef, ETextPropertyConditionCompareEquals, text));
                break;
            case QGalleryFilter::Contains:
                TRAP(err, propertyCondition = &condition->AddPropertyConditionL(
                        propertyDef, ETextPropertyConditionCompareContains, text));
                break;
            case QGalleryFilter::StartsWith:
                TRAP(err, propertyCondition = &condition->AddPropertyConditionL(
                        propertyDef, ETextPropertyConditionCompareBeginsWith, text));
                break;
            case QGalleryFilter::EndsWith:
                TRAP(err, propertyCondition = &condition->AddPropertyConditionL(
                        propertyDef, ETextPropertyConditionCompareEndsWith, text));
                break;
            default:
                return QDocumentGallery::FilterError;
            }
            if (err == KErrNone) {
                if (filter.isNegated())
                    propertyCondition->SetNegate(ETrue);
                return QDocumentGallery::NoError;
            }
         }

        return QDocumentGallery::FilterError;
    }

    static int qt_appendConditions(
            CMdELogicCondition *condition,
            const QList<QGalleryFilter> &filters,
            const QMdeGalleryCategoryType &type,
            const CMdEPropertyDef &propertyDef)
    {
        int error = QDocumentGallery::NoError;

        typedef QList<QGalleryFilter>::const_iterator iterator;
        for (iterator it = filters.begin(), end = filters.end(); it != end; ++it) {
            switch (it->type()) {
            case QGalleryFilter::MetaData:
                error = qt_appendCondition(condition, it->toMetaDataFilter(), type, propertyDef);
                break;
            case QGalleryFilter::Intersection:
                {
                    CMdELogicCondition *andCondition = 0;
                    TRAPD(err, andCondition = &condition->AddLogicConditionL(
                            ELogicConditionOperatorAnd));
                    error = err == 0
                            ? qt_appendConditions(
                                  andCondition,
                                  it->toIntersectionFilter().filters(),
                                  type,
                                  propertyDef)
                            : QDocumentGallery::FilterError;
                }
                break;
            case QGalleryFilter::Union:
                {
                    CMdELogicCondition *orCondition = 0;
                    TRAPD(err, orCondition = &condition->AddLogicConditionL(
                              ELogicConditionOperatorOr));
                    error = err == 0
                            ? qt_appendConditions(
                                  orCondition,
                                  it->toUnionFilter().filters(),
                                  type,
                                  propertyDef)
                            : QDocumentGallery::FilterError;
                }
                break;
            default:
                break;
            }
            if (error != QDocumentGallery::NoError)
                return error;
        }

        return error;
    }

    static int qt_appendFilterCondition(
            CMdELogicCondition *condition,
            const QGalleryFilter &filter,
            const QMdeGalleryCategoryType &type,
            const CMdEPropertyDef &propertyDef)
     {
        int error = QDocumentGallery::NoError;

        switch (filter.type()) {
        case QGalleryFilter::MetaData:
            error = qt_appendCondition(
                    condition, filter.toMetaDataFilter(), type, propertyDef);
            break;
        case QGalleryFilter::Intersection:
            error = qt_appendConditions(
                    condition,
                    filter.toIntersectionFilter().filters(),
                    type,
                    propertyDef);
            break;
        case QGalleryFilter::Union: {
                CMdELogicCondition *orCondition = 0;
                TRAPD(err, orCondition = &condition->AddLogicConditionL(ELogicConditionOperatorOr));
                error = err == 0
                        ? qt_appendConditions(
                                orCondition,
                                filter.toUnionFilter().filters(),
                                type,
                                propertyDef)
                        : QDocumentGallery::FilterError;
            }
            break;
        default:
            break;
        }

        return error;
    }

    static QStringList qt_filterMdeCategoryPropertyNames(
            const QString &itemType, const QStringList &propertyNames)
    {
        QStringList filteredNames;
        const int index = qt_indexOf(itemType, qt_mdeGalleryCategoryTypes);
        if (index != -1) {
            typedef QStringList::const_iterator iterator;
            for (iterator it = propertyNames.begin(), end = propertyNames.end(); it != end; ++it) {
                if (qt_contains(*it, qt_mdeGalleryCategoryTypes[index]))
                    filteredNames.append(*it);
            }
        }
        return filteredNames;
    }

    static int qt_addPropertyFilter(CMdEObjectQuery *query, const CMdEPropertyDef &propertyDef)
    {
        TRAPD(err, query->AddPropertyFilterL(&propertyDef));

        return err == KErrNone
                ? QDocumentGallery::NoError
                : QDocumentGallery::ItemTypeError;
    }

    static int qt_setSortOrder(
            CMdEObjectQuery *query,
            const QStringList &propertyNames,
            const QMdeGalleryCategoryType &type,
            CMdENamespaceDef &namespaceDef)
    {
        if (CMdEPropertyDef *propertyDef = qt_getPropertyDef(
                namespaceDef, *type.mdeObject, *type.mdeProperty)) {
            typedef QStringList::const_iterator iterator;
            for (iterator it = propertyNames.begin(), end = propertyNames.end(); it != end; ++it) {
                bool ascending = true;
                bool validName = false;
                if (it->startsWith(QLatin1Char('+'))) {
                    validName = qt_contains(QStringRef(&(*it), 1, it->length() - 1), type);
                } else if(it->startsWith(QLatin1Char('-'))) {
                    validName = qt_contains(QStringRef(&(*it), 1, it->length() - 1), type);
                    ascending = false;
                } else {
                    validName = qt_contains(*it, type);
                }

                if (validName) {
                    TRAPD(err, query->AppendOrderRuleL(
                            TMdEOrderRule(*propertyDef, ascending ? ETrue : EFalse)));

                    if (err == KErrNone)
                        return QDocumentGallery::NoError;
                }
            }
        }
        return QDocumentGallery::NoError;
    }
}

int QMDEGalleryCategoryResultSet::appendScopeCondition(
        CMdELogicCondition *condition,
        const QVariant &rootItem,
        CMdENamespaceDef &namespaceDef)
{
    switch (rootItem.type()) {
    case QVariant::String:
        {
            QString idString = rootItem.toString();
            const int colonIndex = idString.indexOf(QLatin1Char(':'));
            if (colonIndex != -1) {
                QStringRef itemType(&idString, 0, colonIndex);

                const int typeIndex = qt_indexOf(itemType, qt_mdeGalleryCategoryTypes);
                if (typeIndex != -1) {
                    return qt_appendScopeCondition(
                            condition,
                            qt_mdeGalleryCategoryTypes[typeIndex],
                            idString.mid(colonIndex + 1),
                            namespaceDef);
                }
            }
            return QDocumentGallery::ItemTypeError;
        }
    case QVariant::UInt:
        return qt_appendScopeCondition(condition, rootItem.toUInt(), namespaceDef);
    case QVariant::Invalid:
        return QDocumentGallery::NoError;
    default:
        return QDocumentGallery::ItemTypeError;
    }
}

bool QMDEGalleryCategoryResultSet::isCategoryType(const QString &itemType)
{
    return qt_indexOf(itemType, qt_mdeGalleryCategoryTypes) != -1;
}

QString QMDEGalleryCategoryResultSet::itemIdType(const QString &itemId)
{
    const int colonIndex = itemId.indexOf(QLatin1Char(':'));
    return colonIndex != -1
            ? itemId.mid(colonIndex + 1)
            : QString();
}

QMDEGalleryCategoryResultSet::QMDEGalleryCategoryResultSet(
        QMdeSession *session,
        const QString &itemType,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames,
        const QVariant &rootItem,
        const QGalleryFilter &filter,
        int offset,
        int limit,
        QObject *parent)
    : QGalleryResultSet(parent)
    , m_itemType(itemType)
    , m_propertyNames(qt_filterMdeCategoryPropertyNames(itemType, propertyNames))
    , m_sortPropertyNames(sortPropertyNames)
    , m_rootItem(rootItem)
    , m_filter(filter)
    , m_offset(offset)
    , m_limit(limit)
    , m_count(0)
    , m_currentIndex(-1)
{
    const int index = qt_indexOf(itemType, qt_mdeGalleryCategoryTypes);
    if (index == -1) {
        error(QDocumentGallery::ItemTypeError);
        return;
    }

    const QMdeGalleryCategoryType &type = qt_mdeGalleryCategoryTypes[index];

    CMdENamespaceDef *namespaceDef = 0;
    TRAPD(err, namespaceDef = &session->GetDefaultNamespaceDefL());
    if (err != KErrNone) {
        error(QDocumentGallery::ConnectionError);
        return;
    }

    CMdEObjectDef *objectDef = qt_getObjectDef(
            *namespaceDef, *type.mdeObject);
    if (!objectDef) {
        error(QDocumentGallery::ConnectionError);
        return;
    }

    CMdEPropertyDef *propertyDef = qt_getPropertyDef(*objectDef, *type.mdeProperty);
    if (!propertyDef) {
        error(QDocumentGallery::ConnectionError);
        return;
    }

    m_query.reset(session->NewObjectQuery(*namespaceDef, *objectDef, this));

    if (!m_query) {
        error(QDocumentGallery::ConnectionError);
    } else {
        QGalleryFilter filter(m_filter);

        m_query->SetResultMode(EQueryResultModeDistinctValues);
        m_query->Conditions().SetOperator(ELogicConditionOperatorAnd);

        int error;
        if ((error = qt_addPropertyFilter(m_query.data(), *propertyDef))
                != QDocumentGallery::NoError) {
            QGalleryAbstractResponse::error(error);
        } else if ((error = appendScopeCondition(
                &m_query->Conditions(), m_rootItem, *namespaceDef)) != QDocumentGallery::NoError) {
            QGalleryAbstractResponse::error(error);
        } else if ((error = qt_appendFilterCondition(
                &m_query->Conditions(),
                filter,
                type,
                *propertyDef)) != QDocumentGallery::NoError) {
            QGalleryAbstractResponse::error(error);
        } else if ((error = qt_setSortOrder(
                m_query.data(),
                sortPropertyNames,
                type,
                *namespaceDef)) != QDocumentGallery::NoError) {
            QGalleryAbstractResponse::error(error);
        } else {
            TRAP(err, m_query->FindL(m_limit != 0
                    ? TUint(m_offset + m_limit)
                    : KMdEQueryDefaultMaxCount));

            if (err != KErrNone)
                QGalleryAbstractResponse::error(QDocumentGallery::ConnectionError);
        }
    }
}

QMDEGalleryCategoryResultSet::~QMDEGalleryCategoryResultSet()
{
    if (m_query) {
        m_query->RemoveObserver(*this);
        m_query->Cancel();
    }
}

int QMDEGalleryCategoryResultSet::propertyKey(const QString &property) const
{
    return m_propertyNames.indexOf(property);
}

QGalleryProperty::Attributes QMDEGalleryCategoryResultSet::propertyAttributes(int key) const
{
    return key >= 0 && key < m_propertyNames.count()
            ? QGalleryProperty::CanRead | QGalleryProperty::CanSort | QGalleryProperty::CanFilter
            : QGalleryProperty::Attributes();
}

QVariant::Type QMDEGalleryCategoryResultSet::propertyType(int key) const
{
    return key >= 0 && key < m_propertyNames.count()
            ? QVariant::String
            : QVariant::Invalid;
}

int QMDEGalleryCategoryResultSet::itemCount() const
{
    return m_count;
}

bool QMDEGalleryCategoryResultSet::isValid() const
{
    return m_currentIndex >= m_offset && m_currentIndex < m_count;
}

QVariant QMDEGalleryCategoryResultSet::itemId() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_count) {
        TPtrC16 value = m_query->ResultDistinctValue(m_currentIndex + m_offset);

        return m_itemType + QLatin1Char(':') + QString::fromUtf16(value.Ptr(), value.Length());
    } else {
        return QVariant();
    }
}

QUrl QMDEGalleryCategoryResultSet::itemUrl() const
{
    return QUrl();
}

QString QMDEGalleryCategoryResultSet::itemType() const
{
    return m_currentIndex >= 0 && m_currentIndex < m_count
            ? m_itemType
            : QString();
}

QList<QGalleryResource> QMDEGalleryCategoryResultSet::resources() const
{
    return QList<QGalleryResource>();
}

QVariant QMDEGalleryCategoryResultSet::metaData(int key) const
{
    if (m_currentIndex >= 0
            && m_currentIndex < m_count
            && key >=0
            && key < m_propertyNames.count()) {
        TPtrC16 value = m_query->ResultDistinctValue(m_currentIndex + m_offset);

        return QString::fromUtf16(value.Ptr(), value.Length());
    } else {
        return QVariant();
    }
}

bool QMDEGalleryCategoryResultSet::setMetaData(int, const QVariant &)
{
    return false;
}

int QMDEGalleryCategoryResultSet::currentIndex() const
{
    return m_currentIndex;
}

bool QMDEGalleryCategoryResultSet::fetch(int index)
{
    if (index >= 0 && index < m_count) {
        if (index != m_currentIndex) {
            m_currentIndex = index;

            emit currentItemChanged();
            emit currentIndexChanged(m_currentIndex);
        }
        return true;
    } else {
        return false;
    }
}

void QMDEGalleryCategoryResultSet::cancel()
{
    if (m_query)
        m_query->Cancel();
}

void QMDEGalleryCategoryResultSet::HandleQueryNewResults(
        CMdEQuery &aQuery, TInt, TInt)
{
    const int insertIndex = m_count;
    const int currentIndex = m_currentIndex;

    m_count = qMax(aQuery.Count() - m_offset, 0);

    const int insertCount = m_count - insertIndex;

    if (insertCount > 0) {
        if (currentIndex >= insertIndex)
            m_currentIndex += insertCount;

        emit itemsInserted(insertIndex, insertCount);

        if (m_currentIndex != currentIndex)
            emit currentIndexChanged(m_currentIndex);
    }
}

void QMDEGalleryCategoryResultSet::HandleQueryCompleted(CMdEQuery &, TInt aError)
{
    switch (aError) {
    case KErrNone:
        finish();
        break;
    case KErrCancel:
        QGalleryResultSet::cancel();
        break;
    default:
        error(QDocumentGallery::ConnectionError);
        break;
    }
}

#include "moc_qmdegallerycategoryresultset_p.cpp"

QTM_END_NAMESPACE
