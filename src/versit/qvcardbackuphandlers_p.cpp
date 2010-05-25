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

#include <QList>
#include <QString>
#include <QTextStream>
#include "qvcardbackuphandlers_p.h"
#include "qcontact.h"
#include "qcontactdetail.h"
#include "qversitdocument.h"
#include "qversitproperty.h"

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(PropertyName, "X-NOKIA-QCONTACTFIELD");
Q_DEFINE_LATIN1_CONSTANT(DetailDefinitionParameter, "DETAIL");
Q_DEFINE_LATIN1_CONSTANT(FieldParameter, "FIELD");
Q_DEFINE_LATIN1_CONSTANT(DatatypeParameter, "DATATYPE");
Q_DEFINE_LATIN1_CONSTANT(DatatypeParameterValue, "V");
Q_DEFINE_LATIN1_CONSTANT(GroupPrefix, "G");

QTM_END_NAMESPACE

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


QVCardImporterBackupHandler::QVCardImporterBackupHandler()
{
}

void QVCardImporterBackupHandler::propertyProcessed(
        const QVersitDocument& document,
        const QVersitProperty& property,
        bool alreadyProcessed,
        const QContact& contact,
        QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document)
    Q_UNUSED(contact)
    QString group;
    if (!property.groups().isEmpty())
        group = property.groups().first();
    if (!alreadyProcessed) {
        if (property.name() != PropertyName)
            return;
        if (property.groups().size() != 1)
            return;
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

        // Import the field
        if (parameters.contains(DatatypeParameter, DatatypeParameterValue)) {
            // The value was stored as a QVariant serialized in a QByteArray
            QDataStream stream(property.variantValue().toByteArray());
            QVariant value;
            stream >> value;
            detail.setValue(fieldName, value);
        } else {
            // The value was stored as a QString or QByteArray
            detail.setValue(fieldName, property.variantValue());
        }

        // Replace the equivalent detail in updatedDetails with the new one
        QMutableListIterator<QContactDetail> it(*updatedDetails);
        while (it.hasNext()) {
            if (it.next().key() == detail.key()) {
                it.remove();
                break;
            }
        }
        updatedDetails->append(detail);
    }
    if (!group.isEmpty()) {
        // Keep track of which details were generated from which Versit groups
        foreach (const QContactDetail& detail, *updatedDetails) {
            mDetailGroupMap.insert(group, detail);
        }
    }
}

void QVCardImporterBackupHandler::documentProcessed(
        const QVersitDocument& document,
        QContact* contact)
{
    Q_UNUSED(document)
    Q_UNUSED(contact)
    mDetailGroupMap.clear();
}

QVCardExporterBackupHandler::QVCardExporterBackupHandler()
    : mDetailNumber(0)
{
}

void QVCardExporterBackupHandler::detailProcessed(
        const QContact& contact,
        const QContactDetail& detail,
        const QSet<QString>& processedFields,
        const QVersitDocument& document,
        QList<QVersitProperty>* toBeRemoved,
        QList<QVersitProperty>* toBeAdded)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
    Q_UNUSED(toBeRemoved)
    QVariantMap fields = detail.variantValues();
    // fields from the same detail have the same group so the importer can collate them
    QString detailGroup = GroupPrefix + QString::number(mDetailNumber++);
    int toBeAddedCount = toBeAdded->count();
    bool propertiesSynthesized = false;
    for (QVariantMap::const_iterator it = fields.constBegin(); it != fields.constEnd(); it++) {
        if (!processedFields.contains(it.key()) && !it.value().toString().isEmpty()) {
            // Generate a property for the unknown field
            QVersitProperty property;
            property.setGroups(QStringList(detailGroup));
            property.setName(PropertyName);
            property.insertParameter(DetailDefinitionParameter, detail.definitionName());
            property.insertParameter(FieldParameter, it.key());

            // serialize the value
            if (it.value().type() == QVariant::String
                || it.value().type() == QVariant::ByteArray) {
                // store QStrings and QByteArrays as-is
                property.setValue(it.value());
            } else {
                // store other types by serializing the QVariant in a QByteArray
                QByteArray valueBytes;
                QDataStream stream(&valueBytes, QIODevice::WriteOnly);
                stream << it.value();
                property.insertParameter(DatatypeParameter, DatatypeParameterValue);
                property.setValue(valueBytes);
            }

            toBeAdded->append(property);
            propertiesSynthesized = true;
        }
    }
    if (propertiesSynthesized) {
        // We need to group the already-generated properties with the newly synthesized ones
        for (int i = 0; i < toBeAddedCount; i++) {
            QVersitProperty& property = (*toBeAdded)[i];
            property.setGroups(property.groups() << detailGroup);
        }
    }
}

void QVCardExporterBackupHandler::contactProcessed(
        const QContact& contact,
        QVersitDocument* document)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
    mDetailNumber = 0;
}
