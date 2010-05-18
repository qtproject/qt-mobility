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
#include "qversitcontactdefaulthandlers.h"
#include "qcontact.h"
#include "qcontactdetail.h"
#include "qversitdocument.h"
#include "qversitproperty.h"

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

/*!
 * This is a map from Versit group names to the details that were generated from properties with the
 * said groups.  Multiple details can be associated with a single group.
 */
class DetailGroupMap
{
public:
    QList<QContactDetail> detailsInGroup(const QString& groupName) const;
    void insert(const QString& groupName, const QContactDetail& detail);
    void update(const QContactDetail& detail);
    void clear();

private:
    QHash<int, QString> mDetailGroupName; // detailid -> group name
    QHash<int, QContactDetail> mDetailById; // detailid -> detail
};

class QVersitContactImporterDefaultPropertyHandlerPrivate
{
public:
    DetailGroupMap mDetailGroupMap; // remembers which details came from which groups
};

class QVersitContactExporterDefaultDetailHandlerPrivate
{
public:
    QVersitContactExporterDefaultDetailHandlerPrivate();
    int mDetailNumber;
};

QTM_END_NAMESPACE

/*!
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

/*!
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

/*!
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

/*!
 * Constructs a QVersitContactImporterDefaultPropertyHandler.
 */
QVersitContactImporterDefaultPropertyHandler::QVersitContactImporterDefaultPropertyHandler()
    : d(new QVersitContactImporterDefaultPropertyHandlerPrivate())
{
}

/* eg. if the document has the properties:
 * G0.X-NOKIA-QCONTACTFIELD;DETAIL=Detail1;FIELD=Field1:Value1
 * G0.X-NOKIA-QCONTACTFIELD;DETAIL=Detail1;FIELD=Field2:Value2
 * G1.X-NOKIA-QCONTACTFIELD;DETAIL=Detail1;FIELD=Field1:Value3
 * This will generate two details, both with definition name "Detail1".  The first detail will have
 * fields "Field1"="Value1" and "Field2"="Value2" and the second will have "Field1"="Value3"
 * ie. the vCard groups determine which properties form a single detail.
 */
bool QVersitContactImporterDefaultPropertyHandler::afterProcessProperty(
        const QVersitDocument& document,
        const QVersitProperty& property,
        bool alreadyProcessed,
        const QContact& contact,
        QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document)
    QString group;
    if (!property.groups().isEmpty())
        group = property.groups().first();
    if (!alreadyProcessed) {
        if (property.name() != QLatin1String("X-NOKIA-QCONTACTFIELD"))
            return false;
        if (property.groups().size() != 1)
            return false;
        QMultiHash<QString,QString> parameters = property.parameters();
        QString definitionName = parameters.value(QLatin1String("DETAIL"));
        QString fieldName = parameters.value(QLatin1String("FIELD"));

        // Find a detail previously seen with the same definitionName, which was generated from
        // a property from the same group
        QContactDetail detail(definitionName);
        foreach (const QContactDetail& previousDetail, d->mDetailGroupMap.detailsInGroup(group)) {
            if (previousDetail.definitionName() == definitionName) {
                detail = previousDetail;
            }
        }
        // If not found, it's a new empty detail with the definitionName set.

        // Import the field
        detail.setValue(fieldName, property.variantValue());

        // Replace the equivalent detail in updatedDetails with the new one
        QMutableListIterator<QContactDetail> it(*updatedDetails);
        while (it.hasNext()) {
            if (it.next().key() == detail.key())
                it.remove();
        }
        updatedDetails->append(detail);
    }
    if (!group.isEmpty()) {
        // Keep track of which details were generated from which Versit groups
        foreach (const QContactDetail& detail, *updatedDetails) {
            d->mDetailGroupMap.insert(group, detail);
        }
    }
    return !alreadyProcessed;
}

void QVersitContactImporterDefaultPropertyHandler::endDocument(const QVersitDocument& document,
                                                               QContact* contact)
{
    Q_UNUSED(document)
    Q_UNUSED(contact)
    d->mDetailGroupMap.clear();
}

QVersitContactExporterDefaultDetailHandlerPrivate::QVersitContactExporterDefaultDetailHandlerPrivate()
    : mDetailNumber(0)
{
}

QVersitContactExporterDefaultDetailHandler::QVersitContactExporterDefaultDetailHandler()
    : d(new QVersitContactExporterDefaultDetailHandlerPrivate)
{
}

/* eg. a detail with definition name "Detail" and fields "Field1"="Value1" and
 * "Field2"="Value2" will be exported to the vCard properties:
 * G0.X-NOKIA-QCONTACTFIELD;DETAIL=Detail;FIELD=Field1:Value1
 * G0.X-NOKIA-QCONTACTFIELD;DETAIL=Detail;FIELD=Field2:Value2
 * And the next detail (say, "Detail" with a field "Field1"="Value3" will generate:
 * G1.X-NOKIA-QCONTACTFIELD;DETAIL=Detail;FIELD=Field1:Value3
 * ie. Different details will have different vCard groups.
 */
bool QVersitContactExporterDefaultDetailHandler::afterProcessDetail(
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
    if (detail.definitionName() == QContactType::DefinitionName)
        return false; // special case of an unhandled detail that we don't export
    QVariantMap fields = detail.variantValues();
    // fields from the same detail have the same group so the importer can collate them
    QString detailGroup = QLatin1String("G") + QString::number(d->mDetailNumber++);
    int toBeAddedCount = toBeAdded->count();
    bool propertiesSynthesized = false;
    for (QVariantMap::const_iterator it = fields.constBegin(); it != fields.constEnd(); it++) {
        if (!processedFields.contains(it.key()) && !it.value().toString().isEmpty()) {
            // Generate a property for the unknown field
            QVersitProperty property;
            property.setGroups(QStringList(detailGroup));
            property.setName(QLatin1String("X-NOKIA-QCONTACTFIELD"));
            property.insertParameter(QLatin1String("DETAIL"), detail.definitionName());
            property.insertParameter(QLatin1String("FIELD"), it.key());
            property.setValue(it.value().toString());
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
    return true;
}

void QVersitContactExporterDefaultDetailHandler::endContact(
        const QContact& contact,
        QVersitDocument* document)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
    d->mDetailNumber = 0;
}
