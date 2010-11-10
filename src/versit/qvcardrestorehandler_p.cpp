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

#include "qvcardrestorehandler_p.h"
#include "qcontactdetail.h"
#include "qversitproperty.h"
#include <QList>
#include <QDateTime>
#include <QUrl>

QTM_USE_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::PropertyName, "X-NOKIA-QCONTACTFIELD");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DetailDefinitionParameter, "DETAIL");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::FieldParameter, "FIELD");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameter, "DATATYPE");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterVariant, "VARIANT");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterDate, "DATE");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterDateTime, "DATETIME");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterTime, "TIME");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterBool, "BOOL");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterInt, "INT");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterUInt, "UINT");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::DatatypeParameterUrl, "URL");
Q_DEFINE_LATIN1_CONSTANT(QVCardRestoreHandler::GroupPrefix, "G");


/*
 * Returns a list of details generated from a Versit group.
 */
QList<QContactDetail> DetailGroupMap::detailsInGroup(const QString& groupName) const
{
    QList<int> detailIds = mDetailGroupName.keys(groupName);
    QList<QContactDetail> details;
    foreach (int detailId, detailIds) {
        details << mDetailById[detailId];
    }
    return details;
}

/*
 * Inserts the association between \a detail and \a groupName to the map.
 * The detail must have a key (ie. have already been saved in a contact) and the group name must not
 * be the empty string.
 */
void DetailGroupMap::insert(const QString& groupName, const QContactDetail& detail)
{
    Q_ASSERT(!groupName.isEmpty());
    mDetailGroupName[detail.key()] = groupName;
    mDetailById[detail.key()] = detail;
}

/*
 * Replaces the detail currently in the map with \a detail.
 * The detail must have a key (ie. have already been saved in a contact).
 */
void DetailGroupMap::update(const QContactDetail& detail)
{
    Q_ASSERT(detail.key());
    mDetailById[detail.key()] = detail;
}

/*!
 * Removes details and groups from the map.
 */
void DetailGroupMap::clear()
{
    mDetailGroupName.clear();
    mDetailById.clear();
}




bool QVCardRestoreHandler::propertyProcessed(
        const QVersitProperty& property,
        QList<QContactDetail>* updatedDetails)
{
    bool success = false;
    QString group;
    if (!property.groups().isEmpty())
        group = property.groups().first();
    if (property.name() == PropertyName) {
        if (property.groups().size() != 1)
            return false;
        QMultiHash<QString,QString> parameters = property.parameters();
        QString definitionName = parameters.value(DetailDefinitionParameter);
        QString fieldName = parameters.value(FieldParameter);

        // Find a detail previously seen with the same definitionName, which was generated from
        // a property from the same group
        QContactDetail detail(definitionName);
        foreach (const QContactDetail& previousDetail, mDetailGroupMap.detailsInGroup(group)) {
            if (previousDetail.definitionName() == definitionName) {
                detail = previousDetail;
            }
        }
        // If not found, it's a new empty detail with the definitionName set.

        detail.setValue(fieldName, deserializeValue(property));

        // Replace the equivalent detail in updatedDetails with the new one
        QMutableListIterator<QContactDetail> it(*updatedDetails);
        while (it.hasNext()) {
            if (it.next().key() == detail.key()) {
                it.remove();
                break;
            }
        }
        updatedDetails->append(detail);
        success = true;
    }
    if (!group.isEmpty()) {
        // Keep track of which details were generated from which Versit groups
        foreach (const QContactDetail& detail, *updatedDetails) {
            mDetailGroupMap.insert(group, detail);
        }
    }
    return success;
}

QVariant QVCardRestoreHandler::deserializeValue(const QVersitProperty& property)
{
    // Import the field
    if (property.parameters().contains(DatatypeParameter, DatatypeParameterVariant)) {
        // The value was stored as a QVariant serialized in a QByteArray
        QDataStream stream(property.variantValue().toByteArray());
        QVariant value;
        stream >> value;
        return value;
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterDate)) {
        // The value was a QDate serialized as a string
        return QDate::fromString(property.value(), Qt::ISODate);
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterTime)) {
        // The value was a QTime serialized as a string
        return QTime::fromString(property.value(), Qt::ISODate);
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterDateTime)) {
        // The value was a QDateTime serialized as a string
        return QDateTime::fromString(property.value(), Qt::ISODate);
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterBool)) {
        // The value was a bool serialized as a string
        return property.value().toInt() != 0;
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterInt)) {
        // The value was an int serialized as a string
        return property.value().toInt();
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterUInt)) {
        // The value was a uint serialized as a string
        return property.value().toUInt();
    } else if (property.parameters().contains(DatatypeParameter, DatatypeParameterUrl)) {
        // The value was a QUrl serialized as a string
        return QUrl(property.value());
    } else {
        // The value was stored as a QString or QByteArray
        return property.variantValue();
    }
}

void QVCardRestoreHandler::documentProcessed()
{
    mDetailGroupMap.clear();
}
