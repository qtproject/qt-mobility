/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitcontacthandler.h"
#include "qversitproperty.h"
#include "qcontactname.h"

#include <QObject>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class TestFactory1 : public QVersitContactHandlerFactory
{
    public:
        QString name() const { return "factory1"; }
        QVersitContactHandler* createHandler() const { return NULL; }
};

class tst_QVersitContactPlugins : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultFactory();
    void testImporterPlugins();
    void testExporterPlugins();
};

void tst_QVersitContactPlugins::testDefaultFactory() {
    TestFactory1 factory;
    QCOMPARE(factory.profiles(), QSet<QString>());
    QCOMPARE(factory.index(), 0);
}

void tst_QVersitContactPlugins::testImporterPlugins() {
    QVersitContactImporter importer("Test");
    QVersitDocument document;
    document.setComponentType("VCARD");
    QVersitProperty property;
    property.setName("FN");
    property.setValue("Bob");
    document.addProperty(property);
    QVERIFY(importer.importDocuments(QList<QVersitDocument>() << document));
    QCOMPARE(importer.contacts().size(), 1);
    QList<QContactDetail> details(importer.contacts().first().details("TestDetail"));
    QCOMPARE(details.size(), 5);
    // The plugins have had their index set such that they should be executed in reverse order
    // Check that they are all loaded, and run in the correct order
    QCOMPARE(details.at(0).value<int>("Plugin"), 5);
    QCOMPARE(details.at(1).value<int>("Plugin"), 4);
    QCOMPARE(details.at(2).value<int>("Plugin"), 3);
    QCOMPARE(details.at(3).value<int>("Plugin"), 2);
    QCOMPARE(details.at(4).value<int>("Plugin"), 1);
}

void tst_QVersitContactPlugins::testExporterPlugins() {
    QVersitContactExporter exporter("Test");
    QContact contact;
    QContactName name;
    name.setFirstName("first name");
    contact.saveDetail(&name);
    QVERIFY(exporter.exportContacts(QList<QContact>() << contact));
    QCOMPARE(exporter.documents().size(), 1);
    QList<QVersitProperty> properties(exporter.documents().first().properties());

    // The plugins have had their index set such that they should be executed in reverse order
    // Check that they are all loaded, and run in the correct order
    int n = 0;
    foreach (QVersitProperty property, properties) {
        if (property.name() == "TEST-PROPERTY") {
            switch (n) {
                case 0: QCOMPARE(property.value(), QLatin1String("5")); break;
                case 1: QCOMPARE(property.value(), QLatin1String("4")); break;
                case 2: QCOMPARE(property.value(), QLatin1String("3")); break;
                case 3: QCOMPARE(property.value(), QLatin1String("2")); break;
                case 4: QCOMPARE(property.value(), QLatin1String("1")); break;
            }
            n++;
        }
    }
    QCOMPARE(n, 5);
}

QTEST_MAIN(tst_QVersitContactPlugins)

#include "tst_qversitcontactplugins.moc"
