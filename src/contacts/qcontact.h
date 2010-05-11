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

#ifndef QCONTACT_H
#define QCONTACT_H

#include <QVariant>
#include <QString>
#include <QPair>
#include <QMultiMap>
#include <QList>
#include <QDateTime>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"

#include "qcontactdetail.h"
#include "qcontactdetaildefinition.h"
#include "qcontactdisplaylabel.h"
#include "qcontactrelationship.h"
#include "qcontactrelationshipfilter.h"
#include "qcontacttype.h"

QTM_BEGIN_NAMESPACE

class QContactManager;
class QContactData;
class QContactName;
class QContactAction;

class Q_CONTACTS_EXPORT QContact
{
public:
    QContact();

    ~QContact();

    QContact(const QContact& other);
    QContact& operator=(const QContact& other);

    bool operator==(const QContact &other) const;
    bool operator!=(const QContact &other) const {return !(other == *this);}

    /* Unique ID */
    QContactId id() const;
    void setId(const QContactId& id);
    QContactLocalId localId() const;

    /* Type - contact, group, metacontact, ... */
    QString type() const;
    void setType(const QString& type);
    void setType(const QContactType& type);

    /* The (backend synthesized) display label of the contact */
    QString displayLabel() const;

    /* Is this an empty contact? */
    bool isEmpty() const;
    void clearDetails();

    /* Access details of particular type or which support a particular action */
    QContactDetail detail(const QString& definitionId) const;
    QList<QContactDetail> details(const QString& definitionId = QString()) const;
    QContactDetail detailWithAction(QContactAction* action) const;
    QList<QContactDetail> detailsWithAction(QContactAction* action) const;

    QList<QContactDetail> details(const QString& definitionName, const QString& fieldName, const QString& value) const;

    QContactDetail detail(const char* definitionId) const;
    QList<QContactDetail> details(const char* definitionId) const;
    QList<QContactDetail> details(const char* definitionId, const char* fieldName, const QString& value) const;

    /* Templated retrieval for definition names */
#ifdef Q_QDOC
    QContactDetail detail(const QLatin1Constant& definitionName) const;
    QList<QContactDetail> details(const QLatin1Constant& definitionName) const;
    QList<QContactDetail> details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value);
#else
    template <int N> QContactDetail detail(const QLatin1Constant<N>& definitionName) const
    {
        return detail(definitionName.latin1());
    }
    template <int N> QList<QContactDetail> details(const QLatin1Constant<N>& definitionName) const
    {
        return details(definitionName.latin1());
    }
    template <int N, int M> QList<QContactDetail> details(const QLatin1Constant<N>& definitionName, const QLatin1Constant<M>& fieldName, const QString& value)
    {
        return details(definitionName.latin1(), fieldName.latin1(), value);
    }
#endif

    /* Templated (type-specific) detail retrieval */
    template<typename T> QList<T> details() const
    {
        QList<QContactDetail> props = details(T::DefinitionName.latin1());
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    /* Templated (type-specific) detail retrieval base on given detail field name and field value */
    template<typename T> QList<T> details(const QString& fieldName, const QString& value) const
    {
        QList<QContactDetail> props = details(T::DefinitionName, fieldName, value);
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> QList<T> details(const char* fieldName, const QString& value) const
    {
        QList<QContactDetail> props = details(T::DefinitionName.latin1(), fieldName, value);
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> T detail() const
    {
        return T(detail(T::DefinitionName.latin1()));
    }

    /* generic detail addition/removal functions */
    bool saveDetail(QContactDetail* detail);   // modifies the detail - sets its ID if detail already exists
    bool removeDetail(QContactDetail* detail); // modifies the detail - unsets its ID

    /* Relationships that this contact was involved in when it was retrieved from the manager */
    QList<QContactRelationship> relationships(const QString& relationshipType = QString()) const;
    QList<QContactId> relatedContacts(const QString& relationshipType = QString(), QContactRelationship::Role role = QContactRelationship::Either) const;

    /* Actions available to be performed on this contact */
    QList<QContactActionDescriptor> availableActions(const QString& vendorName = QString(), int implementationVersion = -1) const;

    /* Preferences (eg, set a particular detail preferred for the SMS action) - subject to change! */
    bool setPreferredDetail(const QString& actionName, const QContactDetail& preferredDetail);
    bool isPreferredDetail(const QString& actionName, const QContactDetail& detail) const;
    QContactDetail preferredDetail(const QString& actionName) const;
    QMap<QString, QContactDetail> preferredDetails() const;

private:
    friend class QContactManager;
    friend class QContactManagerData;
    friend class QContactManagerEngine;

    QSharedDataPointer<QContactData> d;
};

Q_CONTACTS_EXPORT uint qHash(const QContact& key);
#ifndef QT_NO_DEBUG_STREAM
Q_CONTACTS_EXPORT QDebug operator<<(QDebug dbg, const QContact& contact);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContact), Q_MOVABLE_TYPE);


#endif

