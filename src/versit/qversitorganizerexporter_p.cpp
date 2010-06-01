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
#include "qversitdocument.h"
#include "qversitproperty.h"

QTM_USE_NAMESPACE

QVersitOrganizerExporterPrivate::QVersitOrganizerExporterPrivate() :
    mDetailHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler)
{
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

    if (detail.definitionName() == QOrganizerItemDisplayLabel::DefinitionName) {
        encodeDisplayLabel(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    } else if (detail.definitionName() == QOrganizerItemEventTimeRange::DefinitionName) {
        encodeTimeRange(detail, *document, &removedProperties, &generatedProperties, &processedFields);
    }

    foreach(const QVersitProperty& property, removedProperties) {
        document->removeProperty(property);
    }
    foreach(const QVersitProperty& property, generatedProperties) {
        document->addProperty(property);
    }
}

void QVersitOrganizerExporterPrivate::encodeDisplayLabel(
        const QOrganizerItemDetail& detail,
        const QVersitDocument& document,
        QList<QVersitProperty>* removedProperties,
        QList<QVersitProperty>* generatedProperties,
        QSet<QString>* processedFields)
{
    QOrganizerItemDisplayLabel displayLabel = static_cast<QOrganizerItemDisplayLabel>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("SUMMARY"), removedProperties);
    property.setName(QLatin1String("SUMMARY"));
    property.setValue(displayLabel.label());
    *generatedProperties << property;
    *processedFields << QOrganizerItemDisplayLabel::FieldLabel;
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

QString QVersitOrganizerExporterPrivate::encodeDateTime(const QDateTime& dateTime)
{
    if (dateTime.timeSpec() == Qt::UTC)
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmssZ"));
    else
        return dateTime.toString(QLatin1String("yyyyMMddTHHmmss"));
}
