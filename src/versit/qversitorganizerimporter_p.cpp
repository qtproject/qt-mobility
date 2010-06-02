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

#include "qversitorganizerimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"
#include "qtorganizer.h"


QTM_USE_NAMESPACE

QVersitOrganizerImporterPrivate::QVersitOrganizerImporterPrivate() :
    mPropertyHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler)
{
}

QVersitOrganizerImporterPrivate::~QVersitOrganizerImporterPrivate()
{
    delete mDefaultResourceHandler;
}

bool QVersitOrganizerImporterPrivate::importDocument(
        const QVersitDocument& document,
        QOrganizerItem* item,
        QVersitOrganizerImporter::Error* error)
{
    if (document.componentType() == QLatin1String("VEVENT")) {
        item->setType(QOrganizerItemType::TypeEvent);
    } else {
        *error = QVersitOrganizerImporter::InvalidDocumentError;
        return false;
    }
    const QList<QVersitProperty> properties = document.properties();
    if (properties.isEmpty()) {
        *error = QVersitOrganizerImporter::EmptyDocumentError;
        return false;
    }

    foreach (const QVersitProperty& property, properties) {
        importProperty(document, property, item);
    }
    return true;
}

void QVersitOrganizerImporterPrivate::importProperty(
        const QVersitDocument& document,
        const QVersitProperty& property,
        QOrganizerItem* item)
{
    QList<QOrganizerItemDetail> updatedDetails;

    bool success = false;
    if (property.name() == QLatin1String("SUMMARY")) {
        success = createDisplayLabel(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("CREATED")) {
        success = createTimestampCreated(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("LAST-MODIFIED")) {
        success = createTimestampModified(property, item, &updatedDetails);
    }

    if (document.componentType() == QLatin1String("VEVENT")) {
        if (property.name() == QLatin1String("DTSTART")) {
            success = createStartDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DTEND")) {
            success = createEndDateTime(property, item, &updatedDetails);
        }
    }

    foreach (QOrganizerItemDetail detail, updatedDetails) {
        item->saveDetail(&detail);
    }
}

bool QVersitOrganizerImporterPrivate::createDisplayLabel(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails)
{
    if (property.value().isEmpty())
        return false;
    QOrganizerItemDisplayLabel displayLabel(item->detail<QOrganizerItemDisplayLabel>());
    displayLabel.setLabel(property.value());
    updatedDetails->append(displayLabel);
    return true;
}

bool QVersitOrganizerImporterPrivate::createTimestampCreated(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemTimestamp timestamp(item->detail<QOrganizerItemTimestamp>());
    timestamp.setCreated(datetime);
    updatedDetails->append(timestamp);
    return true;
}

bool QVersitOrganizerImporterPrivate::createTimestampModified(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemTimestamp timestamp(item->detail<QOrganizerItemTimestamp>());
    timestamp.setLastModified(datetime);
    updatedDetails->append(timestamp);
    return true;
}

bool QVersitOrganizerImporterPrivate::createStartDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
    etr.setStartDateTime(datetime);
    updatedDetails->append(etr);
    return true;
}

bool QVersitOrganizerImporterPrivate::createEndDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
    etr.setEndDateTime(datetime);
    updatedDetails->append(etr);
    return true;
}

QDateTime QVersitOrganizerImporterPrivate::parseDateTime(QString str)
{
    bool utc = str.endsWith(QLatin1Char('Z'), Qt::CaseInsensitive);
    if (utc)
        str.chop(1); // take away z from end;
    QDateTime dt(QDateTime::fromString(str, QLatin1String("yyyyMMddTHHmmss")));
    if (utc)
        dt.setTimeSpec(Qt::UTC);
    return dt;
}
