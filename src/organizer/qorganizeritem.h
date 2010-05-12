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

#ifndef QORGANIZERITEM_H
#define QORGANIZERITEM_H

#include <QVariant>
#include <QString>
#include <QPair>
#include <QMultiMap>
#include <QList>
#include <QDateTime>
#include <QSharedDataPointer>

#include "qtorganizer.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemManager;
class QOrganizerItemData;
class QOrganizerItemName;

class Q_ORGANIZER_EXPORT QOrganizerItem
{
public:
    QOrganizerItem();

    ~QOrganizerItem();

    QOrganizerItem(const QOrganizerItem& other);
    QOrganizerItem& operator=(const QOrganizerItem& other);

    bool operator==(const QOrganizerItem &other) const;
    bool operator!=(const QOrganizerItem &other) const {return !(other == *this);}

    /* Unique ID */
    QOrganizerItemId id() const;
    void setId(const QOrganizerItemId& id);
    QOrganizerItemLocalId localId() const;

    /* Type - event, todo, journal, invitation, ...  is the type backend synthesised? */
    QString type() const;
    void setType(const QString& type);
    void setType(const QOrganizerItemType& type);

    /* The (backend synthesized) display label of the organizeritem */
    QString displayLabel() const;

    /* Is this an empty organizeritem? */
    bool isEmpty() const;
    void clearDetails();

    QOrganizerItemDetail detail(const QString& definitionId) const;
    QList<QOrganizerItemDetail> details(const QString& definitionId = QString()) const;

    QList<QOrganizerItemDetail> details(const QString& definitionName, const QString& fieldName, const QString& value) const;

    QOrganizerItemDetail detail(const char* definitionId) const;
    QList<QOrganizerItemDetail> details(const char* definitionId) const;
    QList<QOrganizerItemDetail> details(const char* definitionId, const char* fieldName, const QString& value) const;

    /* Templated retrieval for definition names */
#ifdef Q_QDOC
    QOrganizerItemDetail detail(const QLatin1Constant& definitionName) const;
    QList<QOrganizerItemDetail> details(const QLatin1Constant& definitionName) const;
    QList<QOrganizerItemDetail> details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value);
#else
    template <int N> QOrganizerItemDetail detail(const QLatin1Constant<N>& definitionName) const
    {
        return detail(definitionName.latin1());
    }
    template <int N> QList<QOrganizerItemDetail> details(const QLatin1Constant<N>& definitionName) const
    {
        return details(definitionName.latin1());
    }
    template <int N, int M> QList<QOrganizerItemDetail> details(const QLatin1Constant<N>& definitionName, const QLatin1Constant<M>& fieldName, const QString& value)
    {
        return details(definitionName.latin1(), fieldName.latin1(), value);
    }
#endif

    /* Templated (type-specific) detail retrieval */
    template<typename T> QList<T> details() const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName.latin1());
        QList<T> ret;
        foreach(QOrganizerItemDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    /* Templated (type-specific) detail retrieval base on given detail field name and field value */
    template<typename T> QList<T> details(const QString& fieldName, const QString& value) const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName, fieldName, value);
        QList<T> ret;
        foreach(QOrganizerItemDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> QList<T> details(const char* fieldName, const QString& value) const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName.latin1(), fieldName, value);
        QList<T> ret;
        foreach(QOrganizerItemDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> T detail() const
    {
        return T(detail(T::DefinitionName.latin1()));
    }

    /* generic detail addition/removal functions */
    bool saveDetail(QOrganizerItemDetail* detail);   // modifies the detail - sets its ID if detail already exists
    bool removeDetail(QOrganizerItemDetail* detail); // modifies the detail - unsets its ID

    // Some common convenience detail accessors
    // has to be things that all subclasses (including Occurrences) have.
    void setNote(QString note);  // ??
    QString note() const;        // ??

protected:
    friend class QOrganizerItemManager;
    friend class QOrganizerItemManagerData;
    friend class QOrganizerItemManagerEngine;

    QSharedDataPointer<QOrganizerItemData> d;
};

Q_ORGANIZER_EXPORT uint qHash(const QOrganizerItem& key);
#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItem& organizeritem);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItem), Q_MOVABLE_TYPE);


#endif

