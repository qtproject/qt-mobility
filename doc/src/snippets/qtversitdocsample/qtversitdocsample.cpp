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

#include "qmobilityglobal.h"
#include "qtcontacts.h"
#include "qversitreader.h"
#include "qversitcontactimporter.h"
#include "qversitwriter.h"
#include "qversitcontactexporter.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qversitresourcehandler.h"
#include <QCoreApplication>
#include <QBuffer>
#include <QList>
#include <QFile>

QTM_USE_NAMESPACE

void completeExample();
void exportExample();
void importExample();

//! [Detail handler]
class MyDetailHandler : public QVersitContactExporterDetailHandler {
public:
    bool preProcessDetail(const QContact& contact, const QContactDetail& detail,
                          QVersitDocument* document) {
        Q_UNUSED(contact) Q_UNUSED(detail) Q_UNUSED(document)
        return false;
    }
    bool postProcessDetail(const QContact& contact, const QContactDetail& detail,
                           bool alreadyProcessed, QVersitDocument* document) {
        Q_UNUSED(contact) Q_UNUSED(document)
        if (!alreadyProcessed)
            mUnknownDetails.append(detail);
        return false;
    }
    QList<QContactDetail> mUnknownDetails;
};
//! [Detail handler]

//! [Property handler]
class MyPropertyHandler : public QVersitContactImporterPropertyHandler {
public:
   bool preProcessProperty(const QVersitDocument& document, const QVersitProperty& property,
                           int contactIndex, QContact* contact) {
       Q_UNUSED(document) Q_UNUSED(property) Q_UNUSED(contactIndex) Q_UNUSED(contact)
       return false;
   }
   bool postProcessProperty(const QVersitDocument& document, const QVersitProperty& property,
                            bool alreadyProcessed, int contactIndex, QContact* contact) {
       Q_UNUSED(document) Q_UNUSED(contactIndex) Q_UNUSED(contact)
       if (!alreadyProcessed)
           mUnknownProperties.append(property);
       return false;
   }
   QList<QVersitProperty> mUnknownProperties;
};
//! [Property handler]

//! [Resource handler]
class MyResourceHandler : public QVersitDefaultResourceHandler {
public:
   bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                     QString* location) {
       Q_UNUSED(property)
       *location = QString::number(qrand());
       QFile file(*location);
       file.open(QIODevice::WriteOnly);
       file.write(contents); // In a real implementation, consider when this file will be deleted.
       return true;
   }
   bool loadResource(const QString& location, QByteArray* contents, QString* mimeType)
   {
       return QVersitDefaultResourceHandler::loadResource(location, contents, mimeType);
   }
};
//! [Resource handler]

int main(int argc, char *argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)
    completeExample();
    exportExample();
    importExample();
}

void completeExample()
{
    //! [Complete example]
    // Create the input vCard
    QBuffer input;
    input.open(QBuffer::ReadWrite);
    QByteArray inputVCard =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John Citizen\r\nN:Citizen;John;Q;;\r\nEND:VCARD\r\n";
    input.write(inputVCard);
    input.seek(0);

    // Parse the input into QVersitDocuments
    QVersitReader reader;
    reader.setDevice(&input);
    reader.startReading(); // Remember to check the return value
    reader.waitForFinished();

    // Convert the QVersitDocuments to QContacts
    QList<QVersitDocument> inputDocuments = reader.results();
    QVersitContactImporter importer;
    QList<QContact> contacts = importer.importContacts(inputDocuments);
    // Note that the QContacts are not saved yet.
    // Use QContactManager::saveContacts() for saving if necessary

    // Export the QContacts back to QVersitDocuments
    QVersitContactExporter exporter;
    QList<QVersitDocument> outputDocuments = exporter.exportContacts(contacts);

    // Encode the QVersitDocument back to a vCard
    QBuffer output;
    output.open(QBuffer::ReadWrite);
    QVersitWriter writer;
    writer.setDevice(&output);
    writer.startWriting(outputDocuments); // Remember to check the return value
    writer.waitForFinished();

    // Read the vCard back to a QByteArray
    output.seek(0);
    QByteArray outputVCard(output.readAll());
    //! [Complete example]
}

void exportExample()
{
    //! [Export example]
    QVersitContactExporter contactExporter;

    MyDetailHandler detailHandler;
    contactExporter.setDetailHandler(&detailHandler);

    QContact contact;
    // Create a name
    QContactName name;
    name.setFirstName(QString::fromAscii("John"));
    contact.saveDetail(&name);

    // Create an avatar type which is not supported by the exporter
    QContactAvatar contactAvatar;
    contactAvatar.setAvatar(QString::fromAscii("/my/image/avatar_path/texture.type"));
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);

    QList<QContact> contactList;
    contactList.append(contact);
    QList<QVersitDocument> versitDocuments = contactExporter.exportContacts(contactList);

    // detailHandler.mUnknownDetails now contains the list of unknown details
    //! [Export example]
}

void importExample()
{
    //! [Import example]
    QVersitContactImporter importer;

    MyPropertyHandler propertyHandler;
    importer.setPropertyHandler(&propertyHandler);

    QVersitDocument document;

    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    property.setValue("Citizen;John;Q;;");
    document.addProperty(property);

    property.setName(QString::fromAscii("X-UNKNOWN-PROPERTY"));
    property.setValue("some value");
    document.addProperty(property);

    QList<QVersitDocument> list;
    list.append(document);

    QList<QContact> contactList = importer.importContacts(list);
    // contactList.first() now contains the "N" property as a QContactName
    // propertyHandler.mUnknownProperties contains the list of unknown properties
    //! [Import example]
}
