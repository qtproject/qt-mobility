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

#include "tst_qversitorganizerexporter.h"
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QList<QOrganizerItem>);

void tst_QVersitOrganizerExporter::testExport()
{
    QFETCH(QList<QOrganizerItem>, items);
    QFETCH(QVersitDocument, expectedDocument);

    QVersitOrganizerExporter exporter;
    QVERIFY(exporter.exportItems(items, QVersitDocument::ICalendar20Type));
    QVERIFY(exporter.errors().isEmpty());
    QVersitDocument document = exporter.document();
    if (document != expectedDocument) {
        qDebug() << "Actual:" << document;
        qDebug() << "Expected:" << expectedDocument;
        QCOMPARE(document, expectedDocument);
    }
}

void tst_QVersitOrganizerExporter::testExport_data()
{
    QTest::addColumn<QList<QOrganizerItem> >("items");
    QTest::addColumn<QVersitDocument>("expectedDocument");

    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));
    QVersitDocument nested(QVersitDocument::ICalendar20Type);
    nested.setComponentType(QLatin1String("VEVENT"));
    QVersitProperty property;
    property.setName(QLatin1String("SUMMARY"));
    property.setValue(QLatin1String("Bastille Day Party"));
    nested.addProperty(property);
    property.setName(QLatin1String("DTSTART"));
    property.setValue(QLatin1String("19970714T170000Z"));
    nested.addProperty(property);
    property.setName(QLatin1String("DTEND"));
    property.setValue(QLatin1String("19970715T035959Z"));
    nested.addProperty(property);
    document.addSubDocument(nested);

    QOrganizerEvent event;
    event.setDisplayLabel(QLatin1String("Bastille Day Party"));
    event.setStartDateTime(QDateTime(QDate(1997, 7, 14), QTime(17, 0, 0), Qt::UTC));
    event.setEndDateTime(QDateTime(QDate(1997, 7, 15), QTime(3, 59, 59), Qt::UTC));

    QList<QOrganizerItem> items;
    items << static_cast<QOrganizerItem>(event);

    QTest::newRow("sample event") << items << document;
}

QTEST_MAIN(tst_QVersitOrganizerExporter)
