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

#include "qversitorganizerexporter.h"
#include "qversitorganizerexporter_p.h"
#include "qtorganizer.h"
#include "versitutils_p.h"
#include "qversitdefs_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"

QTM_USE_NAMESPACE

QVersitOrganizerExporterPrivate::QVersitOrganizerExporterPrivate() :
    mDetailHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler)
{
    int versitPropertyCount =
        sizeof(versitOrganizerDetailMappings)/sizeof(VersitDetailMapping);
    for (int i = 0; i < versitPropertyCount; i++) {
        mPropertyMappings.insert(
                QLatin1String(versitOrganizerDetailMappings[i].detailDefinitionName),
                QPair<QString,QString>(
                    QLatin1String(versitOrganizerDetailMappings[i].detailFieldName),
                    QLatin1String(versitOrganizerDetailMappings[i].versitPropertyName)));
    }
}

QVersitOrganizerExporterPrivate::~QVersitOrganizerExporterPrivate()
{
    delete mDefaultResourceHandler;
}

bool QVersitOrganizerExporterPrivate::exportItem(
    const QOrganizerItem& item,
    QVersitDocument* document,
    QVersitOrganizerExporter::Error* error)
{
    if (item.type() == QOrganizerItemType::TypeEvent) {
        document->setComponentType(QLatin1String("VEVENT"));
    } else {
        *error = QVersitOrganizerExporter::UnknownComponentTypeError;
        return false;
    }
    QList<QOrganizerItemDetail> allDetails = item.details();
    if (allDetails.isEmpty()) {
        *error = QVersitOrganizerExporter::EmptyOrganizerError;
        return false;
    }
    foreach (const QOrganizerItemDetail& detail, allDetails) {
        exportDetail(item, detail, document);
    }
    return true;
}

void QVersitOrganizerExporterPrivate::exportDetail(
        const QOrganizerItem& item,
        const QOrganizerItemDetail& detail,
        QVersitDocument* document)
{
    QList<QVersitProperty> removedProperties;
    QList<QVersitProperty> generatedProperties;
    QSet<QString> processedFields;

    if (detail.definitionName() == QOrganizerItemEventTimeRange::DefinitionName) {
        encodeTimeRange(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemTimestamp::DefinitionName) {
        encodeTimestamp(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (mPropertyMappings.contains(detail.definitionName())) {
        encodeSimpleProperty(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    }

    foreach(const QVersitProperty& property, removedProperties) {
        document->removeProperty(property);
    }
    foreach(const QVersitProperty& property, generatedProperties) {
        document->addProperty(property);
    }
}

void QVersitOrganizerExporterPrivate::encodeTimeRange(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemEventTimeRange etr = static_cast<QOrganizerItemEventTimeRange>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("DTSTART"), removedProperties);
    property.setName(QLatin1String("DTSTART"));
    property.setValue(encodeDateTime(etr.startDateTime()));
    *generatedProperties << property;

    property =
        VersitUtils::takeProperty(document, QLatin1String("DTEND"), removedProperties);
    property.setName(QLatin1String("DTEND"));
    property.setValue(encodeDateTime(etr.endDateTime()));
    *generatedProperties << property;
    *processedFields << QOrganizerItemEventTimeRange::FieldStartDateTime
                     << QOrganizerItemEventTimeRange::FieldEndDateTime;
}

void QVersitOrganizerExporterPrivate::encodeTimestamp(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemTimestamp timestamp = static_cast<QOrganizerItemTimestamp>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("CREATED"), removedProperties);
    property.setName(QLatin1String("CREATED"));
    property.setValue(encodeDateTime(timestamp.created().toUTC()));
    *generatedProperties << property;

    property =
        VersitUtils::takeProperty(document, QLatin1String("LAST-MODIFIED"), removedProperties);
    property.setName(QLatin1String("LAST-MODIFIED"));
    property.setValue(encodeDateTime(timestamp.lastModified().toUTC()));
    *generatedProperties << property;
    *processedFields << QOrganizerItemTimestamp::FieldCreationTimestamp
                     << QOrganizerItemTimestamp::FieldModificationTimestamp;
}

QString QVersitOrganizerExporterPrivate::encodeDateTime(const QDateTime& dateTime)
{
    if (dateTime.timeSpec() == Qt::UTC)
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmssZ"));
    else
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmss"));
}

void QVersitOrganizerExporterPrivate::encodeSimpleProperty(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QPair<QString, QString> fieldPropertyMap = mPropertyMappings[detail.definitionName()];
    const QString& fieldName = fieldPropertyMap.first;
    const QString& propertyName = fieldPropertyMap.second;
    QVersitProperty property =
        VersitUtils::takeProperty(document, propertyName, removedProperties);
    property.setName(propertyName);
    property.setValue(detail.value(fieldName));
    *generatedProperties << property;
    *processedFields << fieldName;
}
