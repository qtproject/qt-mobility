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


#ifndef QCONTACTACTIONDESCRIPTOR_H
#define QCONTACTACTIONDESCRIPTOR_H

#include "qtcontactsglobal.h"
#include "qcontactfilter.h"
#include "qcontactdetail.h"

#include <QSet>
#include <QVariant>
#include <QMap>
#include <QString>
#include <QSharedDataPointer>

QTM_BEGIN_NAMESPACE

class QContact;
class QContactActionFactory;
class QContactActionTarget;
class QContactActionDescriptorPrivate;

// MSVC needs the declaration before the friend statement
class QContactActionDescriptor;
Q_CONTACTS_EXPORT uint qHash(const QContactActionDescriptor& key);
#ifndef QT_NO_DEBUG_STREAM
Q_CONTACTS_EXPORT QDebug& operator<<(QDebug dbg, const QContactActionDescriptor& descriptor);
#endif

class Q_CONTACTS_EXPORT QContactActionDescriptor
{
public:
    QContactActionDescriptor();
    QContactActionDescriptor(const QContactActionDescriptor& other);
    QContactActionDescriptor& operator=(const QContactActionDescriptor& other);
    ~QContactActionDescriptor();

    bool isValid() const;
    bool operator==(const QContactActionDescriptor& other) const;
    bool operator!=(const QContactActionDescriptor& other) const;

    QString actionName() const;
    QString serviceName() const;
    QString actionIdentifier() const;
    int implementationVersion() const;

    /* The descriptor provides the client with all information required in UI. */
    QSet<QContactActionTarget> supportedTargets(const QContact& contact) const;
    QContactFilter contactFilter() const;
    bool supportsContact(const QContact& contact) const;
    QVariant metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters = QVariantMap()) const;

    /* Convenience meta data accessors */
    QVariant metaData(const QString& key, const QVariantMap& parameters = QVariantMap()) const;
    QVariant metaData(const QString& key, const QContactActionTarget& target, const QVariantMap& parameters = QVariantMap()) const;
    QVariant metaData(const QString& key, const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap()) const;

    // default meta-data keys
#ifdef Q_QDOC
    static const QLatin1Constant MetaDataIcon;
    static const QLatin1Constant MetaDataLabel;
    static const QLatin1Constant MetaDataSecondLabel;
    static const QLatin1Constant MetaDataOptionalParameterKeys;
    static const QLatin1Constant MetaDataMandatoryParameterKeys;
#else
    Q_DECLARE_LATIN1_CONSTANT(MetaDataIcon, "Icon");
    Q_DECLARE_LATIN1_CONSTANT(MetaDataLabel, "Label");
    Q_DECLARE_LATIN1_CONSTANT(MetaDataSecondLabel, "SecondLabel");
    Q_DECLARE_LATIN1_CONSTANT(MetaDataOptionalParameterKeys, "OptionalParameterKeys");
    Q_DECLARE_LATIN1_CONSTANT(MetaDataMandatoryParameterKeys, "MandatoryParameterKeys");
#endif

private:
    QSharedDataPointer<QContactActionDescriptorPrivate> d;
    friend class QContactActionFactory;
    friend class QContactActionServiceManager;
#ifndef QT_NO_DEBUG_STREAM
    Q_CONTACTS_EXPORT friend QDebug& operator<<(QDebug dbg, const QContactActionDescriptor& descriptor);
#endif
    Q_CONTACTS_EXPORT friend uint qHash(const QContactActionDescriptor& key);
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactActionDescriptor), Q_MOVABLE_TYPE);

#endif
